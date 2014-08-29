


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

//    swAnimation::SWMod mod;
//    mod.loadModFile("../data/mod/Stelarc_head.mod");

//    swAnimation::SWSeq seq;
//    seq.loadSeqFile("../data/seq/Block_1.seq");

//    swAnimation::SWMsh msh;
//    msh.loadMshFile("../data/msh/Stelarc_head.msh");

//    swAnimation::SWAnimation animation;
//    animation.init(mod,seq,msh);


    swMesh::SWMesh mesh("../data/meshes/processed/generic.obj");
//    animation.retrieveTransformedMesh(7, mesh, true);

    swCloud::SWRigidMotion rigidMotion(0.f,180.f,0.f);
    mesh.cloud()->transform(rigidMotion.m_aFRotation, rigidMotion.m_aFTranslation);


    std::vector<float> l_offsetToApply(3,0.f);

    float l_zMin = -FLT_MAX;
    int l_idZMax = 0;
    for(int ii = 0; ii < mesh.pointsNumber(); ++ii)
    {
        if(mesh.cloud()->coord(2)[ii] > l_zMin)
        {
            l_zMin = mesh.cloud()->coord(2)[ii];
            l_idZMax = ii;
        }
    }

    l_offsetToApply[0] = -mesh.cloud()->coord(0)[l_idZMax];
    l_offsetToApply[1] = -mesh.cloud()->coord(1)[l_idZMax];
    l_offsetToApply[2] = -mesh.cloud()->coord(2)[l_idZMax];
    (*mesh.cloud()) += l_offsetToApply;

    mesh.updateNonOrientedTrianglesNormals();
    mesh.updateNonOrientedVerticesNormals();
    std::string l_sName("generic3");
    mesh.saveToObj("../data/meshes/processed/", l_sName + ".obj", l_sName + ".mtl", l_sName + ".png");



//    animation.init(mod,seq,msh);

//    swCloud::SWCloud cloud;
//    swMesh::SWMesh mesh;

//    swMesh::SWMesh meshGenericClean("../data/meshes/processed/generic.obj");
//    animation.constructCorrId(meshGenericClean,true);

//    swMesh::SWMesh meshMorphed("../data/meshes/results_mort_flo_stelarc/26_08_guillaume.obj");

//    for(int ii = 0; ii < 20; ++ii)
//    {
//        QString l_nameObj("transfo_");
//        QString l_name2Obj("generic_trans_");
//        QString l_name3Obj("morphed_trans_");
//        l_nameObj += QString::number(ii +1);
//        l_name2Obj += QString::number(ii +1);
//        l_name3Obj += QString::number(ii +1);
//        l_nameObj += ".obj";
//        l_name2Obj += ".obj";
//        l_name3Obj += ".obj";

//        animation.retrieveTransformedCloud(ii, cloud, true);
//        cloud.saveToObj("../data/clouds/", l_nameObj.toStdString());

//        animation.retrieveTransformedMesh(ii, mesh, true);
//        mesh.saveToObj("../data/meshes/", l_nameObj.toStdString());


//        swMesh::SWMesh currentMeshGenericTrans = meshGenericClean;
//        animation.transformMeshWithCorrId(ii,currentMeshGenericTrans);
//        currentMeshGenericTrans.saveToObj("../data/meshes/", l_name2Obj.toStdString());

//        swMesh::SWMesh currentMeshMorphedTrans = meshMorphed;
//        animation.transformMeshWithCorrId(ii,currentMeshMorphedTrans);
//        currentMeshMorphedTrans.saveToObj("../data/meshes/", l_name3Obj.toStdString());
//    }











//    swMesh::SWMesh mesh(argv[1]);

//    if(mesh.m_meshLoadSucess)
//    {
//        std::cout << "Mesh loaded. " << std::endl;

////        (*mesh.cloud()) *= 0.025f;

//        swCloud::SWRigidMotion l_transfo(0.f,180.f,0.f);
//        mesh.cloud()->transform(l_transfo.m_aFRotation, l_transfo.m_aFTranslation);

//        (*mesh.cloud()) *= 40.f;

//        mesh.updateNonOrientedTrianglesNormals();
//        mesh.updateNonOrientedVerticesNormals();

////        mesh.deletePointsWithNoFaces();

//        std::string l_sName = argv[2];
//        mesh.saveToObj("../data/meshes/processed/", l_sName + ".obj", l_sName + ".mtl", l_sName + ".png");
//        std::cout << "Mesh saved. " << std::endl;
//    }

    return 0;
}
