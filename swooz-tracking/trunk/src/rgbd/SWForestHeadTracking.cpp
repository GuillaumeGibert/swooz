#include "rgbd/SWForestHeadTracking.h"
#include "SWTrackingDevice.h"

/*
// Authors: Gabriele Fanelli, Thibaut Weise, Juergen Gall, BIWI, ETH Zurich
// Email: fanelli@vision.ee.ethz.ch

// You may use, copy, reproduce, and distribute this Software for any
// non-commercial purpose, subject to the restrictions of the
// Microsoft Research Shared Source license agreement ("MSR-SSLA").
// Some purposes which can be non-commercial are teaching, academic
// research, public demonstrations and personal experimentation. You
// may also distribute this Software with books or other teaching
// materials, or publish the Software on websites, that are intended
// to teach the use of the Software for academic or other
// non-commercial purposes.
// You may not use or distribute this Software or any derivative works
// in any form for commercial purposes. Examples of commercial
// purposes would be running business operations, licensing, leasing,
// or selling the Software, distributing the Software for use with
// commercial products, using the Software in the creation or use of
// commercial products or any other activity which purpose is to
// procure a commercial gain to you or others.
// If the Software includes source code or data, you may create
// derivative works of such portions of the Software and distribute
// the modified Software for non-commercial purposes, as provided
// herein.

// THE SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO
// EXPRESS, IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT
// LIMITATION, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
// PARTICULAR PURPOSE, ANY WARRANTY AGAINST INTERFERENCE WITH YOUR
// ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE OR
// NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL
// FULFILL ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST
// PASS THIS DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR
// DERIVATIVE WORKS.

// NEITHER MICROSOFT NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE
// LIABLE FOR ANY DAMAGES RELATED TO THE SOFTWARE OR THIS MSR-SSLA,
// INCLUDING DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL
// DAMAGES, TO THE MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT
// LEGAL THEORY IT IS BASED ON. ALSO, YOU MUST PASS THIS LIMITATION OF
// LIABILITY ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE
// WORKS.

// When using this software, please acknowledge the effort that
// went into development by referencing the paper:
//
// Fanelli G., Weise T., Gall J., Van Gool L., Real Time Head Pose Estimation from Consumer Depth Cameras
// 33rd Annual Symposium of the German Association for Pattern Recognition (DAGM'11), 2011

*/

std::string deviceName = "rgbd";
std::string libraryName = "forest";
std::string effectorName= "head";
std::string configfileName;
// yarp ports & bottle
std::string headTrackingPortName;
yarp::os::BufferedPort<yarp::os::Bottle> headTrackingPort;

// Path to trees
string g_treepath;
// Number of trees
int g_ntrees;
// Patch width
int g_p_width;
// Patch height
int g_p_height;
//maximum distance form the sensor - used to segment the person
int g_max_z = 0;
//head threshold - to classify a cluster of votes as a head
int g_th = 400;
//threshold for the probability of a patch to belong to a head
float g_prob_th = 1.0f;
//threshold on the variance of the leaves
float g_maxv = 1000.f;
//stride (how densely to sample test patches - increase for higher speed)
int g_stride = 5;
//radius used for clustering votes into possible heads
float g_larger_radius_ratio = 1.f;
//radius used for mean shift
float g_smaller_radius_ratio = 6.f;
//
int g_frame_no = 0;
//opengl window size
int w,h;
//pointer to the actual estimator
CRForestEstimator* g_Estimate;
//input 3D image
Mat g_im3D;
//input image size
int g_im_w = 640;
int g_im_h = 480;
//kinect's frame rate
int g_fps = 30;

XnUInt64 g_focal_length;
XnDouble g_pixel_size;

xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;

DepthMetaData g_depthMD;

XnStatus g_RetVal;

bool g_first_rigid = true;
bool g_show_votes = false;
bool g_draw_triangles = false;

//for interactive visualization
gl_camera g_camera;

std::vector< cv::Vec<float,POSE_SIZE> > g_means; //outputs
std::vector< std::vector< Vote > > g_clusters; //full clusters of votes
std::vector< Vote > g_votes; //all votes returned by the forest

math_vector_3f g_face_curr_dir, g_face_dir(0,0,-1);



void drawCylinder( const math_vector_3f& p1, const math_vector_3f& p2 , float radius, GLUquadric *quadric)
{
    math_vector_3f d = p2 - p1;
    if (d[2] == 0)
        d[2] = .0001f;

    float n = length(d);
    float ax = ( d[2] < 0.0 ) ? -57.295779f*acos( d[2]/n ) : 57.295779f*acos( d[2]/n );

    glPushMatrix();

    glTranslatef( p1[0],p1[1],p1[2] );
    glRotatef( ax, -d[1]*d[2], d[0]*d[2], 0.0);
    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    gluCylinder(quadric, radius, radius, n, 10, 1);

    gluQuadricOrientation(quadric,GLU_INSIDE);
    gluDisk( quadric, 0.0, radius, 10, 1);
    glTranslatef( 0,0,n );

    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    gluDisk( quadric, 0.0, radius, 10, 1);
    glPopMatrix();
}

void initialize(){

    std::cout << "initializing kinect... " << endl;

    // Initialize context object
    g_RetVal = g_Context.Init();

    g_RetVal = g_DepthGenerator.Create(g_Context);
    if (g_RetVal != XN_STATUS_OK)
        printf("Failed creating DEPTH generator %s\n", xnGetStatusString(g_RetVal));

    XnMapOutputMode outputMode;
    outputMode.nXRes = g_im_w;
    outputMode.nYRes = g_im_h;
    outputMode.nFPS = g_fps;
    g_RetVal = g_DepthGenerator.SetMapOutputMode(outputMode);
    if (g_RetVal != XN_STATUS_OK)
        printf("Failed setting the DEPTH output mode %s\n", xnGetStatusString(g_RetVal));

    g_RetVal = g_Context.StartGeneratingAll();
    if (g_RetVal != XN_STATUS_OK)
        printf("Failed starting generating all %s\n", xnGetStatusString(g_RetVal));

    // get the focal length in mm (ZPS = zero plane distance)
    g_DepthGenerator.GetIntProperty ("ZPD", g_focal_length);
    // get the pixel size in mm ("ZPPS" = pixel size at zero plane)
    g_DepthGenerator.GetRealProperty ("ZPPS", g_pixel_size);
    g_pixel_size *= 2.f;

    g_im3D.create(g_im_h,g_im_w,CV_32FC3);


    headTrackingPortName =  "/tracking/" + deviceName + "/"+ libraryName + "/"+ effectorName;
    headTrackingPort.open(headTrackingPortName.c_str());

}

// load config file
void loadConfig(const char* filename) {

    ifstream in(filename);
    string dummy;

    if(in.is_open()) {

        // Path to trees
        in >> dummy;
        in >> g_treepath;

        // Number of trees
        in >> dummy;
        in >> g_ntrees;

        in >> dummy;
        in >> g_maxv;

        in >> dummy;
        in >> g_larger_radius_ratio;

        in >> dummy;
        in >> g_smaller_radius_ratio;

        in >> dummy;
        in >> g_stride;

        in >> dummy;
        in >> g_max_z;

        in >> dummy;
        in >> g_th;


    } else {
        cerr << "File not found " << filename << endl;
        exit(-1);
    }
    in.close();

    cout << endl << "------------------------------------" << endl << endl;
    cout << "Estimation:       " << endl;
    cout << "Trees:            " << g_ntrees << " " << g_treepath << endl;
    cout << "Stride:           " << g_stride << endl;
    cout << "Max Variance:     " << g_maxv << endl;
    cout << "Max Distance:     " << g_max_z << endl;
    cout << "Head Threshold:   " << g_th << endl;

    cout << endl << "------------------------------------" << endl << endl;

}


bool read_data( ){

    // Wait for new data to be available
    g_RetVal = g_Context.WaitAndUpdateAll();
    if (g_RetVal != XN_STATUS_OK)
    {
        printf("Failed updating data: %s\n", xnGetStatusString(g_RetVal));
        return false;
    }

    // Take current depth map
    g_DepthGenerator.GetMetaData(g_depthMD);

    float f = (float)(g_focal_length/g_pixel_size);
    int valid_pixels = 0;

    //generate 3D image
    for(int y = 0; y < g_im3D.rows; y++)
    {
        Vec3f* Mi = g_im3D.ptr<Vec3f>(y);
        for(int x = 0; x < g_im3D.cols; x++){

            float d = (float)g_depthMD(x,y);

            if ( d < g_max_z && d > 0 ){

                valid_pixels++;

                Mi[x][0] = ( float(d * (x - 320)) / f );
                Mi[x][1] = ( float(d * (y - 240)) / f );
                Mi[x][2] = d;

            }
            else
                Mi[x] = 0;

        }
    }

    //this part is to set the camera position, depending on what's in the scene
    if (g_first_rigid ) {

        if( valid_pixels > 50000){ //wait for something to be in the image

            // calculate gravity center
            Vec3f gravity(0,0,0);
            int count = 0;
            for(int y=0;y<g_im3D.rows;++y){
                const Vec3f* Mi = g_im3D.ptr<Vec3f>(y);
                for(int x=0;x<g_im3D.cols;++x){

                    if( Mi[x][2] > 0 ) {

                        gravity = gravity + Mi[x];
                        count++;
                    }
                }
            }

            float maxDist = 0;
            if(count > 0) {

                gravity = (1.f/(float)count)*gravity;

                for(int y=0;y<g_im3D.rows;++y){
                    const Vec3f* Mi = g_im3D.ptr<Vec3f>(y);
                    for(int x=0;x<g_im3D.cols;++x){

                        if( Mi[x][2] > 0 ) {

                            maxDist = MAX(maxDist,(float)norm( Mi[x]-gravity ));
                        }
                    }
                }
            }

            g_camera.resetview( math_vector_3f(gravity[0],gravity[1],gravity[2]), maxDist );
            g_camera.rotate_180();
            g_first_rigid = false;
        }
    }

    return true;
}

bool process() {

    read_data( );

    g_means.clear();
    g_votes.clear();
    g_clusters.clear();

    //do the actual estimation
    g_Estimate->estimate( 	g_im3D,
                            g_means,
                            g_clusters,
                            g_votes,
                            g_stride,
                            g_maxv,
                            g_prob_th,
                            g_larger_radius_ratio,
                            g_smaller_radius_ratio,
                            false,
                            g_th
                        );


    if(g_means.size() == 0)
    {
        return true;
    }

    Bottle &target=headTrackingPort.prepare();
        target.clear();
        target.addInt(swTracking::FOREST_LIB); // device lib id
        target.addDouble(g_means[0][3]);
        target.addDouble(g_means[0][4]);
        target.addDouble(g_means[0][5]);
    headTrackingPort.write();

    return true;
}

// ##############################################################################
void key(unsigned char _k, int, int) {

    switch(_k) {

        case 's': {

            g_show_votes = !g_show_votes;
            cout << "toggled votes " << g_show_votes << endl;
            break;

        }

        case 't': {

            g_draw_triangles = !g_draw_triangles;
            cout << "toggled triangles " << g_draw_triangles << endl;
            break;

        }

        case '+': {

            g_stride++;
            cout << "stride : " << g_stride << endl;
            break;

        }
        case '-':{

            g_stride = MAX(0, g_stride-1);
            cout << "stride : " << g_stride << endl;
            break;

        }
        case '*': {

            g_th += 20;
            cout << "head threshold : " << g_th << endl;
            break;
        }

        case '/':{

            g_th -= 20;
            cout << "head threshold : " << g_th << endl;
            break;
        }

        case 'h':{

            printf("\nAvailable commands:\n");

            printf("\t 's' : toggle votes display \n");

            printf("\t 't' : toggle triangles display \n");

            printf("\t '+' : increase stride \n");
            printf("\t '-' : decrease stride \n");

            printf("\t '*' : increase head threshold \n");
            printf("\t '/' : decrease head threshold \n");

            break;

        }

        case 'q':{
            headTrackingPort.close();
            break;
        }

    default:
        break;

    }
    glutSwapBuffers();

}

// ##############################################################################
void resize(int _w, int _h) {
    w = _w;
    h = _h;
}

// ##############################################################################
void mm(int x, int y)
{
    y = h-y;
    g_camera.mouse_move(x,y);

}

// ##############################################################################
void mb(int button, int state, int x, int y)
{
    y = h-y;

    Mouse::button b = Mouse::NONE;

    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        b = Mouse::ROTATE;
        g_camera.mouse(x,y,b);

    }
    else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

        b = Mouse::MOVEXY;
        g_camera.mouse(x,y,b);
    }
    else if((button & 3) == 3) {

        g_camera.mouse_wheel(20);
    }
    else if ((button & 4) == 4) {

        g_camera.mouse_wheel(-20);
    }


}

void idle(){

    process();
    g_frame_no++;

}

// draws the scan and the estimated head pose
void draw()
{

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    g_camera.set_viewport(0,0,w,h);
    g_camera.setup();
    g_camera.use_light(true);

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    glPushMatrix();
    glColor3f(0.9f,0.9f,1.f);

    if(g_draw_triangles){

        math_vector_3f d1,d2;
        glBegin(GL_TRIANGLES);

        for(int y = 0; y < g_im3D.rows-1; y++)
        {

            const Vec3f* Mi = g_im3D.ptr<Vec3f>(y);
            const Vec3f* Mi1 = g_im3D.ptr<Vec3f>(y+1);

            for(int x = 0; x < g_im3D.cols-1; x++){

                if( Mi[x][2] <= 0 || Mi1[x][2] <= 0 || Mi[x+1][2] <= 0 || Mi1[x+1][2] <= 0 )
                    continue;

                d1[0] = Mi[x][0] - Mi1[x][0];// v1 - v2;
                d1[1] = Mi[x][1] - Mi1[x][1];
                d1[2] = Mi[x][2] - Mi1[x][2];

                d2[0] = Mi[x+1][0] - Mi1[x][0];// v1 - v2;
                d2[1] = Mi[x+1][1] - Mi1[x][1];
                d2[2] = Mi[x+1][2] - Mi1[x][2];

                if ( fabs(d2[2])>20 || fabs(d1[2])>20 )
                    continue;

                math_vector_3f norm = cross_product(d2,d1);

                glNormal3f(norm[0],norm[1],norm[2]);
                glVertex3f(Mi[x][0],Mi[x][1],Mi[x][2]);
                glVertex3f(Mi1[x][0],Mi1[x][1],Mi1[x][2]);
                glVertex3f(Mi[x+1][0],Mi[x+1][1],Mi[x+1][2]);
                glVertex3f(Mi1[x][0],Mi1[x][1],Mi1[x][2]);
                glVertex3f(Mi1[x+1][0],Mi1[x+1][1],Mi1[x+1][2]);
                glVertex3f(Mi[x+1][0],Mi[x+1][1],Mi[x+1][2]);

            }
        }
        glEnd();

    }
    else{

        math_vector_3f d1,d2;
        glBegin(GL_POINTS);

        for(int y = 0; y < g_im3D.rows-1; y++)
        {

            const Vec3f* Mi = g_im3D.ptr<Vec3f>(y);
            const Vec3f* Mi1 = g_im3D.ptr<Vec3f>(y+1);

            for(int x = 0; x < g_im3D.cols-1; x++){

                if( Mi[x][2] <= 0 || Mi[x][2] <= 0 )
                    continue;

                d1[0] = Mi[x][0] - Mi1[x][0];// v1 - v2;
                d1[1] = Mi[x][1] - Mi1[x][1];
                d1[2] = Mi[x][2] - Mi1[x][2];

                d2[0] = Mi[x+1][0] - Mi1[x][0];// v1 - v2;
                d2[1] = Mi[x+1][1] - Mi1[x][1];
                d2[2] = Mi[x+1][2] - Mi1[x][2];

                math_vector_3f norm = cross_product(d2,d1);
                glNormal3f(norm[0],norm[1],norm[2]);
                glVertex3f(Mi[x][0],Mi[x][1],Mi[x][2]);

            }
        }
        glEnd();

    }

    glPopMatrix();

    GLUquadric* point = gluNewQuadric();
    GLUquadric *quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);



    //draw head poses
    if(g_means.size()>0){

        glColor3f( 0, 1, 0);
        float mult = 0.0174532925f;

        for(unsigned int i=0;i<g_means.size();++i){

            rigid_motion<float> rm;

            //~ cout << "(" << g_means[i][3] << ", " << g_means[i][4] << ", " << g_means[i][5] << ")" << endl;

            rm.m_rotation = euler_to_rotation_matrix( mult*g_means[i][3], mult*g_means[i][4], mult*g_means[i][5] );
            math_vector_3f head_center( g_means[i][0], g_means[i][1], g_means[i][2] );

            glPushMatrix();
            glTranslatef( head_center[0], head_center[1], head_center[2] );
            gluSphere( point, 10.f, 10, 10 );
            glPopMatrix();

            g_face_curr_dir = rm.m_rotation * (g_face_dir);
            math_vector_3f head_front(head_center + 150.f*g_face_curr_dir);

            drawCylinder(head_center, head_front, 8, quadric);

        }

    }

    //draw the single votes
    if(g_show_votes){

        int rate = 1;
        glColor3f( 0 , 0, 1);

        for (unsigned int i = 0; i<g_votes.size();i+=rate){

            glPushMatrix();
            glTranslatef( g_votes[i].vote[0], g_votes[i].vote[1], g_votes[i].vote[2] );
            gluSphere( point, 2.f, 10, 10 );
            glPopMatrix();

        }

        for(unsigned int c=0;c<g_clusters.size();c++){

            switch(c%5){

                case 0 : glColor3f( 1.f, 0.f, 0.f); break;
                case 1 : glColor3f( 0.f, 1.f, 0.f); break;
                case 2 : glColor3f( 0.f, 0.f, 1.f); break;
                case 3 : glColor3f( 1.f, 0.f, 1.f); break;
                case 4 : glColor3f( 0.2f, 0.f, 0.8f); break;
                default : glColor3f( 0.f, 1.f, 1.f); break;

            }

            for(unsigned int i=0;i<g_clusters[c].size();i+=rate){

                glPushMatrix();
                glTranslatef(  g_clusters[c][i].vote[0],  g_clusters[c][i].vote[1],  g_clusters[c][i].vote[2] );
                gluSphere( point, 3.f, 10, 10 );
                glPopMatrix();

            }

        }

    } //show votes

    gluDeleteQuadric(point);
    gluDeleteQuadric(quadric);

    glutSwapBuffers();
    glutPostRedisplay();

}

int main(int argc, char* argv[])
{

    // YARP
    /* initialize yarp network */
    Network yarp;
    if (!yarp.checkNetwork())
    {
        std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
        return-1;
    }

    if( argc != 2 ){

        cout << "usage: ./SWForestHeadTracking <config_file>" << endl;
        exit(-1);
    }

    loadConfig( argv[1] );
    g_Estimate =  new CRForestEstimator();
    if( !g_Estimate->loadForest(g_treepath.c_str(), g_ntrees) ){

        cerr << "could not read forest data files !" << endl;
        exit(-1);
    }

    initialize();

    // initialize GLUT
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInit(&argc, argv);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    glutCreateWindow("Forest head tracking (press h for list of available commands)");
    std::cout << "!IMPORTANT : Do not use Ctr+c for leaving, click on the window's cross instead. " << std::endl;
    glutDisplayFunc(draw);
    glutMouseFunc(mb);
    glutMotionFunc(mm);
    glutKeyboardFunc(key);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    glutMainLoop();

    headTrackingPort.interrupt();
    headTrackingPort.close();

    return 0;


}
