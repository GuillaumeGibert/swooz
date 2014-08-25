


/**
 * \file rapideProcessMesh_main.cpp
 * \author Florian Lance
 * \date 19-08-2014
 * \brief Apply some process to the input mesh
 */

#include <iostream>
#include <QtGui>
#include <mesh/SWMesh.h>

#include <animation/SWAnimation.h>

//#include <Qt/QtString>
//#include <QtGui>



static void extractPointFromModFile(QString pathModFile, swCloud::SWCloud &cloud)
{
    qDebug() << "start extractPointFromModFile";
    QFile file(pathModFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    qDebug() << "mod file opened";

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line.left(3) == "  1")
        {
            break;
        }
    }


    int ii = 0;

    std::vector<float> vx,vy,vz;
    while (!in.atEnd())
    {
        double px,py,pz;

        in >> px;
        QString line = in.readLine();
        in >> py;
        line = in.readLine();
        in >> pz;
        line = in.readLine();
        line = in.readLine();
//        qDebug() << ii << " " << px << " " << py << " " << pz;
        ++ii;
        vx.push_back(px);
        vy.push_back(py);
        vz.push_back(pz);
    }

    cloud.set(vx,vy,vz);
}

static void extractFacesFromMshFile(QString pathMshFile, const swCloud::SWCloud &cloud, swMesh::SWMesh &mesh)
{

    qDebug() << "start extractFacesFromMshFile";
    QFile file(pathMshFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    qDebug() << "msh file opened";

    QTextStream in(&file);

    int l_nbFaces;
    in >> l_nbFaces;
    QString line = in.readLine();
//    QString line


    std::vector<std::vector<float> > l_vertices;
    std::vector<std::vector<uint> > l_idFaces;
    std::vector<std::vector<float> > l_texture(cloud.size(), std::vector<float>(2,0.f));




    int ii = 0;
    while (!in.atEnd() && ii < l_nbFaces)
    {
        uint id1,id2,id3;
        std::vector<uint> l_idFace;

//        line = in.readLine();

        in >> id1;
        in >> id2;
        in >> id3;

        line = in.readLine();

//        qDebug() << ii << " " << id1 << " " << id2 << " " << id3;

        l_idFace.push_back(id1+1);
        l_idFace.push_back(id2+1);
        l_idFace.push_back(id3+1);
        l_idFaces.push_back(l_idFace);

        ++ii;
    }

    for(int ii = 0; ii < cloud.size(); ++ii)
    {
        std::vector<float> l_pt;
        l_pt.push_back(cloud.coord(0)[ii]);
        l_pt.push_back(cloud.coord(1)[ii]);
        l_pt.push_back(cloud.coord(2)[ii]);
        l_vertices.push_back(l_pt);
    }

    mesh.set(l_vertices, l_idFaces, l_texture);
}




int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "Specify a mesh file path to be processed. " << std::endl;
        return -1;
    }

    if(argc < 3)
    {
        std::cerr << "Specify the name of the new mesh file. " << std::endl;
        return -1;
    }

    std::cout << "Open mesh file : " << argv[1] << std::endl;

    swAnimation::SWMod mod;

    mod.loadModFile("../data/mod/Stelarc_face.mod");
//    mod.loadModFile("../data/mod/Stelarc_leftCornea.mod");

    swAnimation::SWSeq seq;
    seq.loadSeqFile("../data/seq/Block_1.seq");


//    swCloud::SWCloud cloud;
//    swMesh::SWMesh mesh;
//    extractPointFromModFile("../data/mod/Stelarc_face.mod",cloud);
//    extractFacesFromMshFile("../data/msh/Stelarc_face.msh",cloud,mesh);
//    cloud.saveToObj("../data/clouds/", "Stelarc_face.obj");
//    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_face.obj");

//    extractPointFromModFile("../data/mod/Stelarc_head.mod",cloud);
//    extractFacesFromMshFile("../data/msh/Stelarc_head.msh",cloud,mesh);
//    cloud.saveToObj("../data/clouds/", "Stelarc_head.obj");
//    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_head.obj");

////    extractPointFromModFile("../data/mod/Stelarc_leftCornea.mod",cloud);
////    extractFacesFromMshFile("../data/msh/Stelarc_cornea.msh",cloud,mesh);
////    cloud.saveToObj("../data/clouds/", "Stelarc_leftCornea.obj");
////    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_leftCornea.obj");

//    extractPointFromModFile("../data/mod/Stelarc_leftEye.mod",cloud);
//    extractFacesFromMshFile("../data/msh/Stelarc_eye.msh",cloud,mesh);
//    cloud.saveToObj("../data/clouds/", "Stelarc_leftEye.obj");
//    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_leftEye.obj");

////    extractPointFromModFile("../data/mod/Stelarc_rightCornea.mod",cloud);
////    extractFacesFromMshFile("../data/msh/Stelarc_cornea.msh",cloud,mesh);
////    cloud.saveToObj("../data/clouds/", "Stelarc_rightCornea.obj");
////    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_rightCornea.obj");

//    extractPointFromModFile("../data/mod/Stelarc_rightEye.mod",cloud);
//    extractFacesFromMshFile("../data/msh/Stelarc_eye.msh",cloud,mesh);
//    cloud.saveToObj("../data/clouds/", "Stelarc_rightEye.obj");
//    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_rightEye.obj");

//    extractPointFromModFile("../data/mod/Stelarc_tongue.mod",cloud);
//    extractFacesFromMshFile("../data/msh/Stelarc_tongue.msh",cloud,mesh);
//    cloud.saveToObj("../data/clouds/", "Stelarc_tongue.obj");
//    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_tongue.obj");

//    extractPointFromModFile("../data/mod/Stelarc_tongue_original.mod",cloud);
//    extractFacesFromMshFile("../data/msh/Stelarc_tongue.msh",cloud,mesh);
//    cloud.saveToObj("../data/clouds/", "Stelarc_tongue_original.obj");
//    mesh.saveToObj("../data/meshes/original_stelarc/", "Stelarc_tongue_original.obj");


//    swMesh::SWMesh mesh(argv[1]);

//    if(mesh.m_meshLoadSucess)
//    {
//        std::cout << "Mesh loaded. " << std::endl;

//        (*mesh.cloud()) *= 0.025f;
//        swCloud::SWRigidMotion l_transfo(0.f,180.f,0.f);
//        mesh.cloud()->transform(l_transfo.m_aFRotation, l_transfo.m_aFTranslation);

//        mesh.updateNonOrientedTrianglesNormals();
//        mesh.updateNonOrientedVerticesNormals();

//        mesh.deletePointsWithNoFaces();

//        std::string l_sName = argv[2];
//        mesh.saveToObj("../data/meshes/processed/", l_sName + ".obj", l_sName + ".mtl", l_sName + ".png");
//        std::cout << "Mesh saved. " << std::endl;
//    }

    return 0;
}
