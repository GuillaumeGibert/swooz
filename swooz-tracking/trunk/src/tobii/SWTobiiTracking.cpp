


/**
 * \file SWTobiiTracking.cpp
 * \brief Defines SWTobiiTracking class
 * \author Florian Lance
 * \date 29/07/14
 */

#include "tobii/SWTobiiTracking.h"

#include "SWTrackingDevice.h"


SWTobiiTracking::SWTobiiTracking()
{
    char l_tobiiId[] = "TX120-301-14600516";

    m_i32Fps = 120;

    ts::init_library();
    m_tobii = boost::shared_ptr<TobiiTracker::CTobii>(new TobiiTracker::CTobii(l_tobiiId));

    //Tobii module initiliazation
    m_tobii->init();
    m_ui64LastUpdate = 0;

    /*Wait for initial data.*/
    while(m_tobii->getLastUpdate() <= m_ui64LastUpdate);
        std::cout << ">INFO: Initial data from Tobii available..." << std::endl;

    m_pGazeData = NULL;

    std::string l_sDeviceName  = "tobii";
    std::string l_sLibraryName = "tobiiSDK";

    // set ports name
        std::string l_gazeTrackingPortName    = "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/gaze";

    // open ports
        m_gazeTrackingPort.open(l_gazeTrackingPortName.c_str());
}

bool SWTobiiTracking::configure(yarp::os::ResourceFinder &oRf)
{
    m_i32ScreenHeight   = oRf.check("screenHeight", yarp::os::Value(320), "Height of the display (int)").asInt();
    m_i32ScreenWidth    = oRf.check("screenWidth", yarp::os::Value(580), "Width of the display (int)").asInt();
    m_i32DistanceInterEyes = oRf.check("distanceInterEyes", yarp::os::Value(67), "Distance inter eyes (double)").asInt();

    return true;
}

int main(int argc, char* argv[])
{

    // initialize yarp network
    yarp::os::Network l_oYarp;
    if (!l_oYarp.checkNetwork())
    {
        std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
        return -1;
    }

    SWTobiiTracking l_oTobiiTracking;

    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("tobii_tracking.ini"); //overridden by --from parameter
    rf.setDefaultContext("swooz-tracking/conf");   //overridden by --context parameter
    rf.configure("ICUB_ROOT", argc, argv);

    if (!l_oTobiiTracking.configure(rf))
    {
        std::cerr << "Error configuring module tobii tracking returning. " << std::endl;
        return -1;
    }

    std::cout << "Starting the tobii tracking module..." << std::endl;
    l_oTobiiTracking.runModule();

    return 0;
}


bool SWTobiiTracking::updateModule()
{
    m_pGazeData = m_tobii->grabGaze().get();

    if(m_pGazeData)
    {
        yarp::os::Bottle &l_gazeBottle= m_gazeTrackingPort.prepare();
        l_gazeBottle.clear();
            l_gazeBottle.addInt(swTracking::TOBII_LIB); // Gaze : TOBII_LIB id / get(0).asInt()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_validity));            // Gaze : left_validity  / get(1).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_validity));           // Gaze : right_validity / get(2).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_pupil_diameter));      // Gaze : left_pupil_diameter / get(3).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_pupil_diameter));     // Gaze : right_pupil_diameter / get(4).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_eye_position_3d.x));     // Gaze : left_eye_position_3d x / get(5).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_eye_position_3d.y));     // Gaze : left_eye_position_3d y / get(6).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_eye_position_3d.z));     // Gaze : left_eye_position_3d z / get(7).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_eye_position_3d.x));     // Gaze : right_eye_position_3d x / get(8).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_eye_position_3d.y));     // Gaze : right_eye_position_3d y / get(9).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_eye_position_3d.z));     // Gaze : right_eye_position_3d z / get(10).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_eye_position_3d_relative.x));     // Gaze : left_eye_position_3d_relative x / get(11).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_eye_position_3d_relative.y));     // Gaze : left_eye_position_3d_relative y / get(12).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_eye_position_3d_relative.z));     // Gaze : left_eye_position_3d_relative z / get(13).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_eye_position_3d_relative.x));     // Gaze : right_eye_position_3d_relative x / get(14).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_eye_position_3d_relative.y));     // Gaze : right_eye_position_3d_relative y / get(15).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_eye_position_3d_relative.z));     // Gaze : right_eye_position_3d_relative z / get(16).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_gaze_point_3d.x));     // Gaze : left_gaze_point_3d x / get(17).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_gaze_point_3d.y));     // Gaze : left_gaze_point_3d y / get(18).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_gaze_point_3d.z));     // Gaze : left_gaze_point_3d z / get(19).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_gaze_point_3d.x));     // Gaze : right_gaze_point_3d x / get(20).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_gaze_point_3d.y));     // Gaze : right_gaze_point_3d y / get(21).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_gaze_point_3d.z));     // Gaze : right_gaze_point_3d z / get(22).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_gaze_point_2d.x));     // Gaze : left_gaze_point_2d x / get(23).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->left_gaze_point_2d.y));     // Gaze : left_gaze_point_2d y / get(24).addDouble()

            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_gaze_point_2d.x));     // Gaze : right_gaze_point_2d x / get(25).addDouble()
            l_gazeBottle.addDouble(static_cast<double>(m_pGazeData->right_gaze_point_2d.y));     // Gaze : right_gaze_point_2d y / get(26).addDouble()

            l_gazeBottle.addInt(m_i32ScreenHeight);  // Gaze : screen height / get(27).addInt()
            l_gazeBottle.addInt(m_i32ScreenWidth);   // Gaze : screen width  / get(28).addInt()
            l_gazeBottle.addInt(m_i32DistanceInterEyes);   // Gaze : distance inter eyes  / get(29).addInt()

        m_gazeTrackingPort.write();
    }

    return true;

}

double SWTobiiTracking::getPeriod()
{
    return 1. / m_i32Fps;
}


bool SWTobiiTracking::interruptModule()
{
    m_gazeTrackingPort.interrupt();
    return true;
}


bool SWTobiiTracking::close()
{
    m_gazeTrackingPort.close();
    return true;
}
