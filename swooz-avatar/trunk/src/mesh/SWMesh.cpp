/**
 * \file SWMesh.cpp
 * \brief defines SWMesh
 * \author Florian Lance
 * \date 08/07/13
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "mesh/SWMesh.h"
#include "geometryUtility.h"

using namespace swMesh;
using namespace swCloud;
using namespace std;


SWMesh::SWMesh() : m_ui32EdgesNumber(0),  m_ui32TrianglesNumber(0)
{}

SWMesh::SWMesh(const std::string &sPathObjFile) : m_ui32TrianglesNumber(0), m_ui32EdgesNumber(0)
{   
    bool l_bIsNormal  = false;
    bool l_bIsTexture = false;

    if(m_oCloud.loadObj(sPathObjFile))
    {
        ifstream l_oFileStream(sPathObjFile);

        bool l_bFirstFace = true;
        bool l_bEndFile   = false;
        string l_sLine, l_sType = " ";
        string l_sMTLNameFile   = " ";

        m_vVertexIdTriangle = vector<vector<uint> >(m_oCloud.size(), vector<uint>());

        while(!l_bEndFile)
        {
            l_oFileStream >> l_sType;

            if(l_sType.size() > 0)
            {
                if(l_sType.at(0) == '#' || l_sType == "v") // ignore comments and vertices coordinates
                {
                    getline(l_oFileStream, l_sLine);
                    l_sType = "";

                }
                else if(l_sType == "vt" || l_sType == "vn") // vt x y z       // vn x y z
                {
                    if(l_sType == "vn")
                    {
                        l_bIsNormal  = true;
                    }
                    if(l_sType == "vt" )
                    {
                        l_bIsTexture = true;
                    }

                    float l_fCoordX, l_fCoordY, l_fCoordZ;

                    l_oFileStream >> l_fCoordX;
                    l_oFileStream >> l_fCoordY;

                    if(l_sType == "vt") // texture -> 2 coordinates
                    {
                        m_a2FTextures.push_back(l_fCoordX);
                        m_a2FTextures.push_back(l_fCoordY);
                    }
                    else                // normal  -> 3 coordinates
                    {
                        l_oFileStream >> l_fCoordZ;

                        m_a3FNormals.push_back(l_fCoordX);
                        m_a3FNormals.push_back(l_fCoordY);
                        m_a3FNormals.push_back(l_fCoordZ);
                    }

                    l_sType = "";
                }
                else if(l_sType == "f")
                {                    
                    // check if the obj file contains texture and normal coordinates
                    if(l_bFirstFace)
                    {
                        checkObjFile(l_oFileStream, l_bIsTexture, l_bIsNormal);
                        l_bFirstFace = false;
                    }

                    // check if the current f-line in the obj file contains 1 or two face
//                    bool l_bQuadFace = isQuadFace(l_oFileStream);

                    vector<uint> l_aIdTriangle;                           

                    for(uint ii = 0; ii < 3; ++ii) // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                    {
                        char l_cSeparator;
                        uint l_ui32V1, l_ui32V2, l_ui32V3;                        

                        l_oFileStream >> l_ui32V1;

                        m_aIdFaces.push_back(l_ui32V1-1);
                        l_aIdTriangle.push_back(l_ui32V1-1);

                        m_vVertexIdTriangle[l_ui32V1-1].push_back(static_cast<uint>(m_aIdTriangles.size()));

                        if(l_bIsNormal && !l_bIsTexture) // f v//vn
                        {
                            l_oFileStream >> l_cSeparator;
                            l_oFileStream >> l_cSeparator;
                            l_oFileStream >> l_ui32V3;
                            m_aIdNormals.push_back(l_ui32V3-1);
                        }
                        else if(!l_bIsNormal && l_bIsTexture) // f v/vt
                        {
                            l_oFileStream >> l_cSeparator;
                            l_oFileStream >> l_ui32V2;
                            m_aIdTextures.push_back(l_ui32V2-1);
                        }
                        else if(l_bIsNormal && l_bIsTexture) // f v/vt/vn
                        {
                            l_oFileStream >> l_cSeparator;
                            l_oFileStream >> l_ui32V2;
                            m_aIdTextures.push_back(l_ui32V2-1);
                            l_oFileStream >> l_cSeparator;
                            l_oFileStream >> l_ui32V3;
                            m_aIdNormals.push_back(l_ui32V3-1);
                        }
                    }

                    m_aIdTriangles.push_back(l_aIdTriangle);
                    l_sType = "";
                }
                else if(l_sType == "mtllib")
                {
                    l_oFileStream >> l_sMTLNameFile;
//                    std::cout << "file mtl : " << l_sMTLNameFile << std::endl;
                    l_sType = "";
                }
            }
            else
            {
                l_oFileStream.close();
                l_bEndFile = true;
            }
        }
    }
    else
    {
        cerr << "Error SWMesh constructor (bad parameter). " << endl;
    }

    m_ui32TrianglesNumber = static_cast<uint>(m_aIdFaces.size()) / 3;

    // build links data
        buildEdgeVertexGraph();
        buildVerticesNeighbors();    
}

SWMesh::SWMesh(const std::vector<std::vector<float> > &v3FPoints,
               const std::vector<std::vector<uint> >  &v3UIFaces,
               const std::vector<std::vector<float> > &v2FTextureCoords) : m_ui32EdgesNumber(0),  m_ui32TrianglesNumber(0)
{
    set(v3FPoints, v3UIFaces, v2FTextureCoords);
}

SWMesh::~SWMesh()
{}


SWMesh &SWMesh::operator=(const SWMesh &oMesh)
{
    clean();
    m_oCloud.copy(oMesh.m_oCloud);

    m_ui32EdgesNumber     = oMesh.m_ui32EdgesNumber;
    m_ui32TrianglesNumber = oMesh.m_ui32TrianglesNumber;

    m_a2FTextures = oMesh.m_a2FTextures;
    m_a3FNormals  = oMesh.m_a3FNormals;

    m_a3FNonOrientedVerticesNormals  = oMesh.m_a3FNonOrientedVerticesNormals;
    m_a3FNonOrientedTrianglesNormals = oMesh.m_a3FNonOrientedTrianglesNormals;

    m_aIdFaces          = oMesh.m_aIdFaces;
    m_aIdTextures       = oMesh.m_aIdTextures;
    m_aIdNormals        = oMesh.m_aIdNormals;
    m_vVertexIdTriangle = oMesh.m_vVertexIdTriangle;

    m_aIdTriangles      = oMesh.m_aIdTriangles;
    m_a2VertexLinks     = oMesh.m_a2VertexLinks;
    m_a2VertexNeighbors = oMesh.m_a2VertexNeighbors;

    return *this;
}


void SWMesh::set(const std::vector<std::vector<float> > &v3FPoints,
                 const std::vector<std::vector<uint> >  &v3UIFaces,
                 const std::vector<std::vector<float> > &v2FTextureCoords)
{
    clean();

    // set points
    std::vector<float> vX(v3FPoints.size()), vY(v3FPoints.size()), vZ(v3FPoints.size());
        for(uint ii = 0; ii < v3FPoints.size(); ++ii)
        {
            vX[ii] = v3FPoints[ii][0];
            vY[ii] = v3FPoints[ii][1];
            vZ[ii] = v3FPoints[ii][2];
        }
        m_oCloud.set(vX,vY,vZ);

    // set triangles
        m_aIdFaces = std::vector<uint>(v3UIFaces.size()*3);
        m_vVertexIdTriangle = std::vector<std::vector<uint> >(m_aIdFaces.size(), vector<uint>());
        for(uint ii = 0; ii < v3UIFaces.size(); ++ii)
        {
            uint l_ui32V1, l_ui32V2, l_ui32V3;
            l_ui32V1 = v3UIFaces[ii][0]-1;
            l_ui32V2 = v3UIFaces[ii][1]-1;
            l_ui32V3 = v3UIFaces[ii][2]-1;
            std::vector<uint> l_v3UIFaces;
            l_v3UIFaces.push_back(l_ui32V1);
            l_v3UIFaces.push_back(l_ui32V2);
            l_v3UIFaces.push_back(l_ui32V3);

            m_aIdFaces[ii*3]  = l_ui32V1;
            m_aIdFaces[ii*3+1]= l_ui32V2;
            m_aIdFaces[ii*3+2]= l_ui32V3;

            uint l_ui32SizeIdTriangle = static_cast<uint>(m_aIdTriangles.size());
            m_vVertexIdTriangle[l_ui32V1].push_back(l_ui32SizeIdTriangle);
            m_vVertexIdTriangle[l_ui32V2].push_back(l_ui32SizeIdTriangle);
            m_vVertexIdTriangle[l_ui32V3].push_back(l_ui32SizeIdTriangle);

            m_aIdTriangles.push_back(l_v3UIFaces);
        }
        m_ui32TrianglesNumber = static_cast<uint>(m_aIdFaces.size()) / 3;

    // set texture
        m_aIdTextures.assign(m_aIdFaces.cbegin(), m_aIdFaces.cend());
        m_a2FTextures = std::vector<float>(v2FTextureCoords.size()*2);
        for(uint ii = 0; ii < v2FTextureCoords.size(); ++ii)
        {
            m_a2FTextures[ii*2]   = v2FTextureCoords[ii][0];
            m_a2FTextures[ii*2+1] = v2FTextureCoords[ii][1];
        }

    // build links data
        buildEdgeVertexGraph();
        buildVerticesNeighbors();

    // build normals
        m_aIdNormals.assign(m_aIdFaces.cbegin(), m_aIdFaces.cend());
        updateNonOrientedTrianglesNormals();
        updateNonOrientedVerticesNormals();
}

void SWMesh::clean()
{
    m_oCloud.erase();

    m_a2FTextures.clear();
    m_a3FNormals.clear();

    m_a3FNonOrientedVerticesNormals.clear();
    m_a3FNonOrientedTrianglesNormals.clear();

    m_aIdFaces.clear();
    m_aIdTextures.clear();
    m_aIdNormals.clear();
    m_vVertexIdTriangle.clear();

    m_aIdTriangles.clear();
    m_a2VertexLinks.clear();
    m_a2VertexNeighbors.clear();

    m_ui32EdgesNumber     = 0;
    m_ui32TrianglesNumber = 0;
}

void SWMesh::point(float *aFXYZ, cuint ui32IdVertex) const
{
    m_oCloud.point(&aFXYZ[0], ui32IdVertex);
}

void SWMesh::trianglePoints(float *aFXYZ, cuint ui32IdTriangle) const
{
    m_oCloud.point(&aFXYZ[0],m_aIdTriangles[ui32IdTriangle][0]);
    m_oCloud.point(&aFXYZ[3],m_aIdTriangles[ui32IdTriangle][1]);
    m_oCloud.point(&aFXYZ[6],m_aIdTriangles[ui32IdTriangle][2]);
}

void SWMesh::vertexNormal(float *a3FNormal, cuint ui32IdVertex) const
{
    a3FNormal[0] = m_a3FNonOrientedVerticesNormals[ui32IdVertex][0];
    a3FNormal[1] = m_a3FNonOrientedVerticesNormals[ui32IdVertex][1];
    a3FNormal[2] = m_a3FNonOrientedVerticesNormals[ui32IdVertex][2];
}

bool SWMesh::saveToObj(const std::string &sPath, const std::string &sNameObj, const std::string sNameMaterial, const std::string sNameTexture) // TODO : finish
{
    if(sPath.size() == 0 || sNameObj.size() == 0)
    {
        std::cerr << "-ERROR : SWMesh::saveToObj, bad parameters. " << std::endl;
        return false;
    }

    std::ofstream l_oFlowMaterial;
    if(sNameMaterial.size() > 0)
    {
        l_oFlowMaterial.open(sPath + sNameMaterial);
        l_oFlowMaterial << "# Mesh created with SWoOZ plateform (https://github.com/GuillaumeGibert/swooz)" << std::endl;
        l_oFlowMaterial << "newmtl materialAvatar" << std::endl;
        l_oFlowMaterial << "Ka 1.000000 1.000000 1.000000" << std::endl;
        l_oFlowMaterial << "Kd 1.000000 1.000000 1.000000" << std::endl;
        l_oFlowMaterial << "Ks 0.000000 0.000000 0.000000" << std::endl;
        l_oFlowMaterial << "Tr 1.000000" << std::endl;
        l_oFlowMaterial << "illum 1" << std::endl;
        l_oFlowMaterial << "Ns 0.000000" << std::endl;

        if(sNameTexture.size() > 0)
        {
            l_oFlowMaterial << "map_Kd " << sNameTexture << std::endl;
        }

        l_oFlowMaterial.close();
    }

    std::ofstream l_oFlowOBJ(sPath + sNameObj);
    l_oFlowOBJ << "# Mesh created with SWoOZ plateform (https://github.com/GuillaumeGibert/swooz) " << std::endl;


    if(sNameMaterial.size() > 0)
    {
        l_oFlowOBJ << "mtllib " + sNameMaterial << std::endl;;
    }

    if(l_oFlowOBJ)
    {
        // save vertices
            std::vector<float> l_a3FPoint;
            for(uint ii = 0; ii < pointsNumber(); ++ii)
            {
                std::ostringstream l_osV1, l_osV2, l_osV3;

                point(l_a3FPoint, ii);

                l_osV1 << l_a3FPoint[0];
                l_osV2 << l_a3FPoint[1];
                l_osV3 << l_a3FPoint[2];

                l_oFlowOBJ << "v " + l_osV1.str() + " " + l_osV2.str() + " " + l_osV3.str() << std::endl;
            }
        // save vertex texture coord
            for(uint ii = 0; ii < m_a2FTextures.size()/2; ++ii)
            {
                std::ostringstream l_osVT1, l_osVT2;

                l_osVT1 << m_a2FTextures[2*ii];
                l_osVT2 << m_a2FTextures[2*ii+1];

                l_oFlowOBJ << "vt " + l_osVT1.str() + " " + l_osVT2.str() << std::endl;
            }
        // save vertex normals
            for(uint ii = 0; ii < m_a3FNormals.size()/3; ++ii)
            {
                std::ostringstream l_osVN1, l_osVN2, l_osVN3;

                l_osVN1 << m_a3FNormals[3*ii];
                l_osVN2 << m_a3FNormals[3*ii+1];
                l_osVN3 << m_a3FNormals[3*ii+2];

                l_oFlowOBJ << "vn " + l_osVN1.str() + " " + l_osVN2.str() + " " + l_osVN3.str() << std::endl;
            }
        // save faces
            l_oFlowOBJ << "usemtl materialAvatar" << std::endl;;

            for(uint ii = 0; ii < trianglesNumber(); ++ii)
            {
                std::ostringstream l_osI1, l_osI2, l_osI3;

                l_osI1 << m_aIdFaces[3*ii]+1;
                l_osI2 << m_aIdFaces[3*ii+1]+1;
                l_osI3 << m_aIdFaces[3*ii+2]+1;

                string l_oF1 = l_osI1.str();
                string l_oF2 = l_osI2.str();
                string l_oF3 = l_osI3.str();

                if(m_a2FTextures.size() > 0)
                {
                    l_oF1 += "/" + l_osI1.str();
                    l_oF2 += "/" + l_osI2.str();
                    l_oF3 += "/" + l_osI3.str();
                }

                if(m_a3FNormals.size() > 0 && m_a2FTextures.size() > 0)
                {
                    l_oF1 += "/" + l_osI1.str();
                    l_oF2 += "/" + l_osI2.str();
                    l_oF3 += "/" + l_osI3.str();
                }
                else if(m_a3FNormals.size() > 0)
                {
                    l_oF1 += "//" + l_osI1.str();
                    l_oF2 += "//" + l_osI2.str();
                    l_oF3 += "//" + l_osI3.str();
                }

                l_oFlowOBJ << "f " + l_oF1 + " " + l_oF2 + " " + l_oF3 << std::endl;
            }
    }
    else
    {
        std::cerr << "Error writing obj file : saveToObj " << std::endl;
        return false;
    }

    return true;
}

void SWMesh::scale(cfloat fScaleValue)
{
    m_oCloud *= fScaleValue;
}

float *SWMesh::vertexBuffer() const
{
    return m_oCloud.vertexBuffer();
}

uint32 *SWMesh::indexVertexTriangleBuffer() const
{
    uint32 *l_aUI32Index = new uint32[m_aIdFaces.size()];

    for(uint ii = 0; ii < m_aIdFaces.size(); ++ii)
    {
        l_aUI32Index[ii] = m_aIdFaces[ii];
    }

    return l_aUI32Index;
}

float *SWMesh::normalBuffer() const
{
    float *l_aFNormal = new float[m_a3FNormals.size()];

    for(uint ii = 0; ii < m_a3FNormals.size(); ++ii)
    {
        l_aFNormal[ii] = m_a3FNormals[ii];
    }

    return l_aFNormal;
}

float *SWMesh::textureBuffer() const
{
    float *l_aFTexture = new float[m_a2FTextures.size()];

    for(uint ii = 0; ii < m_a2FTextures.size(); ++ii)
    {
        l_aFTexture[ii] = m_a2FTextures[ii];
    }

    return l_aFTexture;
}

uint SWMesh::edgesNumber() const
{
    return m_ui32EdgesNumber;
}

uint SWMesh::trianglesNumber() const
{
    return m_ui32TrianglesNumber;
}

uint SWMesh::pointsNumber() const
{
    return m_oCloud.size();
}

bool SWMesh::isVerticesNormals() const
{
    return (m_a3FNonOrientedVerticesNormals.size() > 0 && m_a3FNonOrientedVerticesNormals.size() == pointsNumber());
}

bool SWMesh::isTrianglesNormals() const
{
    return (m_a3FNonOrientedTrianglesNormals.size() > 0 && m_a3FNonOrientedTrianglesNormals.size() == trianglesNumber());
}

bool SWMesh::isTriangleOnABorder(cuint ui32IdTriangle)
{
    if(ui32IdTriangle > trianglesNumber())
    {
        cerr << "Error : isTriangleOnABorder SWMesh : input id is invalid. " << endl;
        return false;
    }

    int l_i32NumVertexOnborder = 0;

    for(uint ii = 0; ii < 3; ++ii)
    {
        if(vertexOnBorder(m_aIdFaces[ui32IdTriangle + ii]))
        {
            ++l_i32NumVertexOnborder;
        }
    }

    if(l_i32NumVertexOnborder > 1)
    {
        return true;
    }

    return false;
}


uint SWMesh::idNearestPoint(cint i32IdSourcePoint,  SWMesh &oTarget)// const // TODO : octree ?
{

    int     l_i32IdMin = -1;
    float   l_fDistMax = FLT_MAX;

//    std::vector<float> l_oP1, l_oP2;

    for(uint ii = 0; ii < oTarget.pointsNumber(); ++ii)
    {
//                point(l_oP1,i32IdSourcePoint);
//        oTarget.point(l_oP2,ii);

//        float l_fCurrSquareDist =
//                (l_oP2[0] - l_oP1[0]) * (l_oP2[0] - l_oP1[0]) +
//                (l_oP2[1] - l_oP1[1]) * (l_oP2[1] - l_oP1[1]) +
//                (l_oP2[2] - l_oP1[2]) * (l_oP2[2] - l_oP1[2]);

        float l_fCurrSquareDist =
                (oTarget.cloud()->coord(0)[ii] - cloud()->coord(0)[i32IdSourcePoint]) * (oTarget.cloud()->coord(0)[ii] - cloud()->coord(0)[i32IdSourcePoint]) +
                (oTarget.cloud()->coord(1)[ii] - cloud()->coord(1)[i32IdSourcePoint]) * (oTarget.cloud()->coord(1)[ii] - cloud()->coord(1)[i32IdSourcePoint]) +
                (oTarget.cloud()->coord(2)[ii] - cloud()->coord(2)[i32IdSourcePoint]) * (oTarget.cloud()->coord(2)[ii] - cloud()->coord(2)[i32IdSourcePoint]);

        if(l_fCurrSquareDist < FLT_EPSILON)
        {
            return ii;
        }

        if(l_fCurrSquareDist < l_fDistMax)
        {
            l_fDistMax = l_fCurrSquareDist;
            l_i32IdMin = ii;
        }
    }

    return l_i32IdMin;
}

std::vector<uint> SWMesh::vertexLinks(cuint ui32idVertex) const
{
    return m_a2VertexLinks[ui32idVertex];
}

swCloud::SWCloud *SWMesh::cloud()
{
    return &m_oCloud;
}

void SWMesh::updateNonOrientedTrianglesNormals()
{
    m_a3FNonOrientedTrianglesNormals.clear();

    for(uint ii = 0; ii < trianglesNumber(); ++ii)
    {
        vector<float> l_vP1, l_vP2, l_vP3;
        trianglePoints(l_vP1, l_vP2, l_vP3, ii);

        vector<float> l_vNormal = swUtil::crossProduct(swUtil::vec(l_vP1, l_vP2), swUtil::vec(l_vP3, l_vP1));
        swUtil::normalize(l_vNormal);
        m_a3FNonOrientedTrianglesNormals.push_back(l_vNormal);        
    }
}

void SWMesh::updateNonOrientedVerticesNormals()
{
    if(m_a3FNonOrientedTrianglesNormals.size() > 0)
    {
        // reset array
            m_a3FNonOrientedVerticesNormals.clear();

        // compute mean point for particular cases
            std::vector<float> l_v3FMeanPoint = m_oCloud.meanPoint();

            m_a3FNonOrientedVerticesNormals = vector<vector<float> >(pointsNumber(), vector<float>(3,0.f));

            for(uint ii = 0; ii < trianglesNumber(); ++ii)
            {
                for(uint jj = 0; jj < 3; ++jj)
                {
                    vector<float> l_v3FCurrNormal = m_a3FNonOrientedTrianglesNormals[ii];

                    if(jj >= 1)
                    {
                        if(swUtil::dotProduct(l_v3FCurrNormal, m_a3FNonOrientedVerticesNormals[m_aIdTriangles[ii][jj]]) < 0)
                        {
                            swUtil::inverse(l_v3FCurrNormal);
                        }
                    }
                    swUtil::add(m_a3FNonOrientedVerticesNormals[m_aIdTriangles[ii][jj]], l_v3FCurrNormal);
                }
            }

            for(uint ii = 0; ii < pointsNumber(); ++ii)
            {
                if(swUtil::norm(m_a3FNonOrientedVerticesNormals[ii]) <= 0.0) // in this case vertices doesn't belong to a triangle
                {
                     vector<float> currPoint;
                     point(currPoint, ii);
                     vector<float> currVector = swUtil::vec(currPoint, l_v3FMeanPoint);
                     swUtil::add(m_a3FNonOrientedVerticesNormals[ii], currVector); // l_v3FMeanPoint
                }

                swUtil::normalize(m_a3FNonOrientedVerticesNormals[ii]);
            }


        // udpate normals array with non-oriented vertices normals
        m_a3FNormals.clear();
        for(uint ii = 0; ii < m_a3FNonOrientedVerticesNormals.size(); ++ii)
        {
            for(uint jj = 0; jj < m_a3FNonOrientedVerticesNormals[ii].size(); ++jj)
            {
                m_a3FNormals.push_back(m_a3FNonOrientedVerticesNormals[ii][jj]);
            }
        }
    }       
    else
    {
        cerr << "Error : triangles normals mest be computed before vertices normals. " << endl; // TODO : throw
    }
}

void SWMesh::buildEdgeVertexGraph()
{
    m_a2VertexLinks.clear();
    m_a2VertexLinks = vector<vector<uint> >(pointsNumber(), vector<uint>());

    for(uint ii = 0; ii < trianglesNumber(); ++ii)
    {
        int a = m_aIdFaces[3*ii];
        int b = m_aIdFaces[3*ii+1];
        int c = m_aIdFaces[3*ii+2];

        for(uint jj = 0; jj < 3; ++jj)
        {
            int x,y;

            if(jj == 0)         // add ab
            {
                x = a;
                y = b;
            }
            else if(jj == 1)    // add bc
            {
                x = b;
                y = c;
            }
            else                // add ca
            {
                x = c;
                y = a;
            }

            bool l_bAdd = true;

            // check if 'y' doesn't exist in 'x'
            if(m_a2VertexLinks[x].size() > 0)
            {
                for(uint jj = 0; jj < m_a2VertexLinks[x].size(); ++jj)
                {
                    if(y == m_a2VertexLinks[x][jj])
                    {
                        l_bAdd = false;
                        break;
                    }
                }
            }

            // check if 'x' doesn't exist in 'y'
            if(l_bAdd)
            {
                for(uint jj = 0; jj < m_a2VertexLinks[y].size(); ++jj)
                {
                    if(x == m_a2VertexLinks[y][jj])
                    {
                        l_bAdd = false;
                        break;
                    }
                }
            }

            if(l_bAdd)
            {
                m_a2VertexLinks[x].push_back(y);
                ++m_ui32EdgesNumber;
            }

        }

    }
}

void SWMesh::buildVerticesNeighbors()
{
    if(m_a2VertexLinks.size() == 0)
    {
        cerr << "Error buildVerticesNeighbors, SWMesh, buildEdgeVertexGraph must be called before. " << endl;
        return;
    }

    m_a2VertexNeighbors.clear();
    m_a2VertexNeighbors = vector<vector<uint> >(pointsNumber(), vector<uint>());

    for(uint ii = 0; ii < m_a2VertexLinks.size(); ++ii)
    {
        for(uint jj = 0; jj < m_a2VertexLinks[ii].size(); ++jj)
        {
            m_a2VertexNeighbors[ii].push_back(m_a2VertexLinks[ii][jj]);
            m_a2VertexNeighbors[m_a2VertexLinks[ii][jj]].push_back(ii);
        }
    }

}


bool SWMesh::vertexOnBorder(cuint ui32IdVertex)
{
    if(m_a2VertexNeighbors.size() == 0 )
    {
        cerr << "Error vertexOnBorder, SWMesh, buildVerticesNeighbors must be called before. " << endl;
        return false;
    }

    if(ui32IdVertex > pointsNumber())
    {
        cerr << "Error : vertexOnBorder SWMesh : input id is invalid. " << endl;
        return false;
    }

    return (m_vVertexIdTriangle[ui32IdVertex].size() != m_a2VertexNeighbors[ui32IdVertex].size());
}


bool SWMesh::isQuadFace(ifstream &l_oFileStream)
{
    streampos  l_oPos = l_oFileStream.tellg();
    string l_sLine;
    getline(l_oFileStream, l_sLine);
    l_oFileStream.seekg (l_oPos);

    int l_i32SpaceNumber = 0;
    for(uint ii = 0; ii < l_sLine.size(); ++ii)
    {
        if(l_sLine[ii] == ' ')
        {
            ++l_i32SpaceNumber;
        }
    }

    if(l_i32SpaceNumber == 4)
    {
        return true;
    }

    return false;
}

void SWMesh::checkObjFile(ifstream &l_oFileStream, bool &bIsTextureCoords, bool &bIsNormals)
{
    streampos  l_oPos = l_oFileStream.tellg();
    string l_sLine;
    getline(l_oFileStream, l_sLine);
    l_oFileStream.seekg (l_oPos);

    if(l_sLine.find('/') == std::string::npos) // case : f i
    {
        bIsNormals       = false;
        bIsTextureCoords = false;
        return;
    }

    if(l_sLine.find("//") != std::string::npos) // case : f i//i
    {
        bIsNormals       = true;
        bIsTextureCoords = false;
        return;
    }

    char l_cSeparator;
    uint l_ui32V1,l_ui32V2;
    l_oFileStream >> l_ui32V1;
    l_oFileStream >> l_cSeparator;
    l_oFileStream >> l_ui32V2;
    l_cSeparator  = ' ';
    l_oFileStream >> l_cSeparator;

    if(l_cSeparator == '/') // case : f i/i/i
    {
        bIsNormals       = true;
        bIsTextureCoords = true;
    }
    else // case : f i/i
    {
        bIsNormals       = false;
        bIsTextureCoords = true;
    }

    l_oFileStream.seekg (l_oPos);
}
