
/**
 * \file SWMesh.h
 * \brief defines SWMesh
 * \author Florian Lance
 * \date 08/07/13
 */

#ifndef _SWMESH_
#define _SWMESH_

#include "cloud/SWCloud.h"
#include "geometryUtility.h"

namespace swMesh
{
    /**
     * \class SWMesh
     * \brief Defines a mehs...
     * \author Florian Lance
      * \date 08/07/13
     */
    class SWMesh
    {
        public:

            // ############################################# CONSTRUCTORS / DESTRUCTORS

            /**
             * \brief Default constructor of SWMesh
             */
            SWMesh();

            /**
             * \brief Constructor of SWMesh
             * \param [in] sPathObjFile  : path of the obj file to load
             */
            SWMesh(const std::string &sPathObjFile);

            /**
             * \brief Copy constructor of SWMesh
             * \param [in] oMesh  : mesh to be copied
             */
//            SWMesh(const SWMesh &oMesh);

            /**
             * \brief Destructor of SWMesh
             */
            virtual ~SWMesh(void);

            // ############################################# METHODS

            /**
             * \brief Get the cloud point corresponding to the input id.
             * \param [out] aTPoint         : array containing the point coordinates
             * \param [in] ui32IdPoint      : id of cloud point
             */
            template <typename T>
            void point(std::vector<T> &aTPoint, cuint ui32IdPoint) const
            {
                if(ui32IdPoint < pointsNumber())
                {
                    m_oCloud.point(aTPoint, ui32IdPoint);
                }
                else
                {
                    std::cerr << "Error : point SWMesh. " << std::endl; // TODO : throw
                }
            }

            /**
             * \brief Get the cloud point corresponding to the input id.
             * \param [int,out] aFXYZ       : in -> a 3-size float allocated array, out -> the array is filled with the  point coordinates [x,y,z]
             * \param [in] ui32IdVertex     : id of the point
             */
            void point(float *aFXYZ, cuint ui32IdVertex) const;

            /**
             * \brief Get the triangle's points corresponding to the input triangle id.
             * \param [out] a3TV1           : array containing the first vertex coordinates
             * \param [out] a3DT2           : array containing the second vertex coordinates
             * \param [out] a3DT3           : array containing the third vertex coordinates
             * \param [in] ui32IdTriangle   : id of the triangle
             */
            template <typename T>
            void trianglePoints(std::vector<T> &a3TV1, std::vector<T> &a3TV2, std::vector<T> &a3TV3, cuint ui32IdTriangle) const
            {
                if(ui32IdTriangle < trianglesNumber())
                {
                    // fill arrays with vertex coordinates
                    m_oCloud.point(a3TV1, m_aIdTriangles[ui32IdTriangle][0]);
                    m_oCloud.point(a3TV2, m_aIdTriangles[ui32IdTriangle][1]);
                    m_oCloud.point(a3TV3, m_aIdTriangles[ui32IdTriangle][2]);
                }
                else
                {
                    std::cerr << "Error : trianglePoints SWMesh. " << std::endl; // TODO : throw
                }
            }

            /**
             * \brief Get the triangle center point corresponding to the input id.
             * \param [out] aTCenterPoint   : array containing the triangle center point coordinates
             * \param [in] ui32IdTriangle   : id of the triangle
             */
            template <typename T>
            void triangleCenterPoint(std::vector<T> &aTCenterPoint, cuint ui32IdTriangle) const
            {
                if(ui32IdTriangle < trianglesNumber())
                {
                    aTCenterPoint = std::vector<T>(3,(T)0);
                    std::vector<T> l_v1, l_v2, l_v3;
                    trianglePoints(l_v1, l_v2, l_v3, ui32IdTriangle);
                    swUtil::add(aTCenterPoint, l_v1);
                    swUtil::add(aTCenterPoint, l_v2);
                    swUtil::add(aTCenterPoint, l_v3);
                    aTCenterPoint[0] /= (T)3;
                    aTCenterPoint[1] /= (T)3;
                    aTCenterPoint[2] /= (T)3;
                }
                else
                {
                    std::cerr << "Error : triangleCenterPoint SWMesh. " << std::endl; // TODO : throw
                }
            }

            /**
             * \brief Get the triangle points corresponding to the input id.
             * \param [int,out] aFXYZ       : in -> a 9-size float allocated array, out -> the array is filled with the triangle point coordinates [x1,y1,z1,x2,yz,...,z3]
             * \param [in] ui32IdTriangle   : id of the triangle
             */
            void trianglePoints(float *aFXYZ, cuint ui32IdTriangle) const;

            /**
             * \brief Get the vertex normal corresonding to the input id.
             * \param [out] aTNormal         : array containing the normal coordinates
             * \param [in] ui32IdVertex      : vertex id
             */
            template <typename T>
            void vertexNormal(std::vector<T> &aTNormal, cuint ui32IdVertex) const
            {
                if(ui32IdVertex < m_a3FNonOrientedVerticesNormals.size())
                {
                    aTNormal.clear();
                    aTNormal.push_back(m_a3FNonOrientedVerticesNormals[ui32IdVertex][0]);
                    aTNormal.push_back(m_a3FNonOrientedVerticesNormals[ui32IdVertex][1]);
                    aTNormal.push_back(m_a3FNonOrientedVerticesNormals[ui32IdVertex][2]);
                }
                else
                {
                    std::cerr << "Error : vertexNormal SWMesh. " << std::endl; // TODO : throw
                }
            }

            /**
             * \brief Get the vertex normal corresonding to the input id. No id validity check.
             * \param [int,out] a3FNormal   : in -> a 3-size float allocated array, out -> the array is filled with the normal coordinates [x,y,z]
             * \param [in] ui32IdVertex     : vertex id
             */
            void vertexNormal(float *a3FNormal, cuint ui32IdVertex) const;

            /**
             * \brief Get the triangle normal corresonding to the input id.
             * \param [out] aTNormal           : array containing the normal coordinates
             * \param [in] ui32IdTriangle      : triangle id
             */
            template <typename T>
            void triangleNormal(std::vector<T> &aTNormal, cuint ui32IdTriangle) const
            {
                if(ui32IdTriangle < m_a3FNonOrientedTrianglesNormals.size())
                {
                    aTNormal.clear();
                    aTNormal.push_back(m_a3FNonOrientedTrianglesNormals[ui32IdTriangle][0]);
                    aTNormal.push_back(m_a3FNonOrientedTrianglesNormals[ui32IdTriangle][1]);
                    aTNormal.push_back(m_a3FNonOrientedTrianglesNormals[ui32IdTriangle][2]);
                }
                else
                {
                    std::cerr << "Error : triangleNormal SWMesh. " << std::endl; // TODO : throw
                }
            }

            /**
             * \brief Save the mesh to obj file.
             * \param [in] sPath             : file path for the obj
             * \return true if hte save is successful
             */
            bool saveToObj(const std::string &sPath);

            /**
             * \brief Scale the mesh by multipliying all the coordinates points by the input value.
             * \param [in] fScaleValue   : scaling value
             */
            void scale(cfloat fScaleValue);

            /**
             * \brief Get a copy of all the index of the triangles vertex [uint32] (used for opengl)
             * \return the uint32 array
             */
            uint32 *indexVertexTriangleBuffer() const;

            /**
             * \brief Get a copy of all the vertex coordinates in an array [float float float] (used for opengl)
             * \return the float array
             */
            float *vertexBuffer() const;

            /**
             * \brief Get a copy of all the normals coordinates in an array [float float float] (used for opengl)
             * \return the float array
             */
            float *normalBuffer() const;

            /**
             * \brief Get a copy of all the texture coordinates in an array [float float] (used for opengl)
             * \return the float array
             */
            float *textureBuffer() const;

            /**
             * \brief Return the number of edges of the mesh.
             * \return the edges number.
             */
            uint edgesNumber() const;

            /**
             * \brief Return the number of triangles of the mesh.
             * \return the triangles number.
             */
            uint trianglesNumber() const;

            /**
             * \brief Return the number of points of the mesh;
             * \return the points number.
             */
            uint pointsNumber() const;

            /**
             * \brief Does vertices normals exist ?
             * \return true if there is the same number of vertices normals than vertices in the mesh.
             */
            bool isVerticesNormals() const;

            /**
             * \brief Does triangles normals exist ?
             * \return true if there is the same number of triangles normals than triangles in the mesh.
             */
            bool isTrianglesNormals() const;

            /**
             * \brief Check if the triangle of the mesh corresponding to the input id is on a border.
             * \param [in] ui32IdTriangle : triangle id
             * \return true if on a border, else false
             */
            bool isTriangleOnABorder(cuint ui32IdTriangle);


            /**
             * \brief Check if the vertex of the mesh corresponding to the input id is on a border.
             * \param [in] ui32IdVertex : vertex id
             * \return true if on a border, else false
             */
            bool vertexOnBorder(cuint ui32IdVertex);

            /**
             * \brief ...
             * \param [in] i32IdSourcePoint    : id of the source point to compare
             * \param [in] oTarget             : ...
             * \return ...
             */
            uint idNearestPoint(cint i32IdSourcePoint, SWMesh &oTarget); // const;

            /**
             * \brief Return a vector containing the id of the vertex linked with the one corresponding to the input id.
             * \return a copy of an id vector
             */
            std::vector<uint> vertexLinks(cuint ui32idVertex) const;

            /**
             * \brief Update the array containing the non oriented normal for each triangle (m_a3FNonOrientedTrianglesNormals)
             */
            void updateNonOrientedTrianglesNormals();

            /**
             * \brief Update the array containing the non oriented normal for each vertex (m_a3DNonOrientedVerticesNormals)
             */
            void updateNonOrientedVerticesNormals();

            /**
             * \brief Return a pointer on the mesh cloud (memory managed by SWMesh destructor)
             * \return m_oCloud pointer
             */
            swCloud::SWCloud *cloud();

            swCloud::SWCloud m_oCloud;          /**< cloud containg mesh points */ // TODO : no public


        protected:

            /**
             * \brief Check if the triangle of the mesh corresponding to the input id is on a border.
             * \param [in] ui32IdTri : triangle id
             * \return true if on a border, else false
             */
            bool triOnBorder(cuint ui32IdTri);

            /**
             * \brief Build the array containing the links between each vertex (m_a2VertexLinks)
             */
            void buildEdgeVertexGraph();

            /**
             * \brief Build the array containing the linked neighbors for each vertex (m_a2VertexNeighbors)
             */
            void buildVerticesNeighbors();


            bool isQuadFace(std::ifstream &l_oFileStream);
            void checkObjFile(std::ifstream &l_oFileStream, bool &bIsTextureCoords, bool &bIsNormals);


            uint m_ui32EdgesNumber;             /**< number of edges of the mesh */
            uint m_ui32TrianglesNumber;         /**< number of triangles of the mesh */

            std::vector<float> m_a2FTextures;   /**< texture coordinates of each vertex [v0x, v0y, v1x, v1y, ..., vnx, vny] */
            std::vector<float> m_a3FNormals;    /**< normals of each vertex [v0x, v0y, v0z, v1x, v1y, ..., vnx, vny, vnz] */

            std::vector<std::vector<float> > m_a3FNonOrientedVerticesNormals;  /**< non oriented normals of each vertex [v0x, v0y, v0z, v1x, v1y, ..., vnx, vny, vnz] */
            std::vector<std::vector<float> > m_a3FNonOrientedTrianglesNormals;  /**< non oriented normals of each triangle [v0x, v0y, v0z, v1x, v1y, ..., vtnx, vtny, vtnz] */

            std::vector<uint> m_aIdFaces;       /**< id points composing each triangle [f0_id0, f0_id1, f0_id2, f1_id0, ..., ftn_id0, ftn_id1, ftn_id2] */
            std::vector<uint> m_aIdTextures;    /**< id texture for each triangle vertex (m_a2FTextures)[f0_id0, f0_id1, f0_id2, f1_id0, ..., ftn_id0, ftn_id1, ftn_id2] */
            std::vector<uint> m_aIdNormals;     /**< id normal for each triangle vertex (m_a3FNormals) [f0_id0, f0_id1, f0_id2, f1_id0, ..., ftn_id0, ftn_id1, ftn_id2] */
            std::vector<std::vector<uint> > m_vVertexIdTriangle; /**< all the triangles id which are composed by a particular vertex */

            std::vector<std::vector<uint> > m_aIdTriangles;     /**< array of id point for each mesh triangle */
            std::vector<std::vector<uint> > m_a2VertexLinks;    /**< array of id linked point for each vertex, no duplicate */
            std::vector<std::vector<uint> > m_a2VertexNeighbors;/**< array of id neighbors point for each vertex */
    };
}


#endif
