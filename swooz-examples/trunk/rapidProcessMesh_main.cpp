


/**
 * \file rapideProcessMesh_main.cpp
 * \author Florian Lance
 * \date 19-08-2014
 * \brief Apply some process to the input mesh
 */

#include <iostream>

#include <mesh/SWMesh.h>

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


    swMesh::SWMesh mesh(argv[1]);

    if(mesh.m_meshLoadSucess)
    {
        std::cout << "Mesh loaded. " << std::endl;

        (*mesh.cloud()) *= 0.025f;
        swCloud::SWRigidMotion l_transfo(0.f,180.f,0.f);
        mesh.cloud()->transform(l_transfo.m_aFRotation, l_transfo.m_aFTranslation);

        mesh.updateNonOrientedTrianglesNormals();
        mesh.updateNonOrientedVerticesNormals();

        std::string l_sName = argv[2];
        mesh.saveToObj("../data/meshes/processed/", l_sName + ".obj", l_sName + ".mtl", l_sName + ".png");
    }

//    generic_headCleaned.obj

    return 0;
}
