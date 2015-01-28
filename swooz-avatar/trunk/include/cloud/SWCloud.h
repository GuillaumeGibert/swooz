
/*******************************************************************************
**                                                                            **
**  SWoOz is a software platform written in C++ used for behavioral           **
**  experiments based on interactions between people and robots               **
**  or 3D avatars.                                                            **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
** *****************************************************************************
**          Authors: Guillaume Gibert, Florian Lance                          **
**  Website/Contact: http://swooz.free.fr/                                    **
**       Repository: https://github.com/GuillaumeGibert/swooz                 **
********************************************************************************/

/**
 * \file SWCloud.h
 * \brief defines SWCloud and SWCloudBBox
 * \author Florian Lance 
 * \date 06/12/12
 */

/*! \mainpage
 *
 * \section SWoOz
 *
 * \subsection intro_sec Introduction
 *
 * SWoOz is a software platform written in C++ used for behavioral experiments based on
 * interactions between people and robots or 3D avatars.
 *
 * \subsection install_sec Install
 *
 * Go to https://github.com/GuillaumeGibert/swooz/blob/master/scripts/README_SCRIPTS.md for build procedures.
 *
 *
 * \subsection content_sec Content
 *
 * \subsubsection Devices_toolkit
 *
 * - Interfaces (Kinect/Xtion, Leap, FaceLab, Fastrak Polhemus, Tobii..) <br />
 * - Depth sensor data recorder/loader
 *
 * \subsubsection Yarp_modules
 *
 * - Tracking human movements without intrusive devices : head, gaze, face, torso, arms, hands, fingers <br />
 * - Teleoperation for iCub, NAO, 3D Avatars based on the tracking modules <br />
 * - Manipulation for manipulating tracking data and create planifications <br />
 * - Feedback for getting video, audio feedback from the robot
 *
 * \subsubsection Avatar_creation_morphing
 *
 * - Creation from depths sensors devices(Kinect/Xtion) data <br />
 * - Morphing for fitting references avatars meshes <br />
 * - Animation/Viewer
 *
 * \subsection wiki_sec WIKI
 *
 * Go to https://github.com/GuillaumeGibert/swooz/wiki for more infos.
 */


#ifndef _SWCLOUD_
#define _SWCLOUD_

#include "commonTypes.h"
#include <vector>

//! namespace for classes based on the use of SWCloud
namespace swCloud
{
	/**
	 * \struct SWCloudBBox
	 * \brief Defines a Cloud point bbox.
	 * \author Florian Lance
	 * \date 04/01/13	 
	 */	
	struct SWCloudBBox
	{
		float m_fMinX; /**< min x coordinate */
		float m_fMinY; /**< min y coordinate */
		float m_fMinZ; /**< min z coordinate */
		
		float m_fMaxX; /**< max x coordinate */
		float m_fMaxY; /**< max y coordinate */
		float m_fMaxZ; /**< max z coordinate */		
		
		/**
		 * \brief verify if the input point si inside the bbox.
		 * \param [in] fX : x coordinate of the input point
		 * \param [in] fY : y coordinate of the input point
		 * \return true if it's inside else return false
		 */		
		bool isInside(cfloat fX, cfloat fY) const
		{			
			return (fX > m_fMinX && fX < m_fMaxX && fY > m_fMinY && fY < m_fMaxY);						
		}		
		
		/**
		 * \brief verify if the input point si inside the bbox.
		 * \param [in] fX : x coordinate of the input point
		 * \param [in] fY : y coordinate of the input point
		 * \param [in] fZ : z coordinate of the input point
		 * \return true if it's inside else return false
		 */		
		bool isInside(cfloat fX, cfloat fY, cfloat fZ) const
		{			
			return (fX > m_fMinX && fX < m_fMaxX && fY > m_fMinY && fY < m_fMaxY && fZ > m_fMinZ && fZ < m_fMaxZ);						
		}

        /**
         * @brief diagSize
         * @return the diagonal length of the bbox
         */
        float diagLength() const
        {
            float l_fDiffX = m_fMinX - m_fMaxX;
            float l_fDiffY = m_fMinY - m_fMaxY;
            float l_fDiffZ = m_fMinZ - m_fMaxZ;

            return sqrt(l_fDiffX*l_fDiffX + l_fDiffY*l_fDiffY + l_fDiffZ*l_fDiffZ);
        }
	};	

    /**
     * \class SWRigidMotion
     * \author Florian Lance
     * \brief Rigid motion
     * \date 10/12/12
     */
    class SWRigidMotion
    {
        public :

            // ############################################# CONSTRUCTORS / DESTRUCTORS

            /**
             * \brief default constructor of SWRigidMotion
             */
            SWRigidMotion();

            /**
             * \brief constructor of SWRigidMotion
             * \param [in] aFRotation    : input rotation matrix
             * \param [in] aFTranslation : input translation matrix
             * \param [in] bRotMat 	     : is the input rotation array is a 3x3 matrix ?, else 3x1 rotation angles array
             */
            SWRigidMotion(cfloat *aFRotation, cfloat *aFTranslation, bool bRotMat = true);

            /**
             * @brief constructor of SWRigidMotion
             * \param [in] fRotation    : rotation angle value
             * \param [in] v3FAxe       : axe rotation
             */
            SWRigidMotion(cfloat fAngle, std::vector<float> v3FAxe);

            /**
             * @brief constructor of SWRigidMotion with spheric coordinates
             * @param [in] fLati  : latitude
             * @param [in] fLong  : longitude
             * @param [in] fAngle : angle
             */
//            SWRigidMotion(cfloat fLati, cfloat fLong, cfloat fAngle, cbool bTest = false);

            /**
             * \brief constructor of SWRigidMotion (0,0,0) center of the rotation
             * \param [in] fRotationX   : x rotation angle value
             * \param [in] fRotationY   : y rotation angle value
             * \param [in] fRotationZ   : Z rotation angle value
             */
            SWRigidMotion(cfloat fRotationX, cfloat fRotationY, cfloat fRotationZ);

            /**
             * \brief recopy constructor of SWRigidMotion
             * \param [in] oRigidMotion    : input rigid motion to copy
             */
            SWRigidMotion(const SWRigidMotion &oRigidMotion);

            /**
             * \brief destructor of SWRigidMotion
             */
            ~SWRigidMotion();

            /**
             * @brief Compute the rotation matrix from the quaternions
             */
            void computeRotationMatrixWithQuaternions();

            /**
             * \brief Compute the rotation angles from the rotation matrix
             */
            void computeRotationAnglesWithRotationMatrix();

            /**
             * \brief Compute the rotation matrix from the rotation angles
             */
            void computeRotationMatrixWithRotationAngles();

            /**
             * \brief Compute the quaternions from the rotation matrix
             */
            void computeQuaternionsWithRotationMatrix();

            /**
             * \brief set RT matrix
             * \param [in] aFRotation    : input rotation matrix
             * \param [in] aFTranslation : input translation matrix
             */
            void set(cfloat *aFRotation, cfloat *aFTranslation);

            /**
             * \brief display rigid motion
             */
            void display();

            /**
             * @brief displayRotMatrix
             */
            void displayRotMatrix();


            float m_aFRotation[9];    /**< rotation matrix  R: Rx(th1)*Ry(th2)*Rz(th3) */

            float m_aFTranslation[3]; /**< translations */

            float m_aFRotAngles[3];	  /**< rotations */

            float m_aFQuaternions[4]; /**< quaternions */
    };


	
	/**
	 * \class SWCloud
     * \brief Defines a Cloud point. Can be used with emicp cuda algorithm, points are defined on 32bits.
	 * \author Florian Lance
	 * \date 06/12/12
	 */
	class SWCloud
	{
		public:
		
			// ############################################# CONSTRUCTORS / DESTRUCTORS
		
			/**
			 * \brief Default constructor of SWCloud
			 */			
			SWCloud();		
		
			/**
			 * \brief Constructor of SWCloud
             * \param [in] ui32NumberOfPoints   : number of points in the cloud
             * \param [in] aCoords              : coordonitates of the cloud to set
             * \param [in] aUi8Colors           : colors of the cloud to set
			 */			
			SWCloud(cuint ui32NumberOfPoints, float *aCoords, uint8 *aUi8Colors);
		

            /**
             * @brief Constructor of SWCloud
             * @param [in] sPathObjFile : path of the obj file to load
             */
            SWCloud(const std::string &sPathObjFile);

			/**
			 * \brief Constructor of SWCloud
			 * \param [in] vPX : vector of input X float point
			 * \param [in] vPY : vector of input Y float point
			 * \param [in] vPZ : vector of input Z float point
			 */			
			SWCloud(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ);
				
			/**
			 * \brief Constructor of SWCloud
			 * \param [in] vPX : vector of input X float point
			 * \param [in] vPY : vector of input Y float point
			 * \param [in] vPZ : vector of input Z float point
			 * \param [in] vR  : vector of input R color value
			 * \param [in] vG  : vector of input G color value
			 * \param [in] vB  : vector of input B color value
			 */				
			SWCloud(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ, 
				const std::vector<uint8> &vR,  const std::vector<uint8> &vG,  const std::vector<uint8> &vB);
								
			/**
			 * \brief Constructor of SWCloud : create a cube cloud using the input coordinates for the center
			 *  Used for displaying  featured points in a bigger cloud.
			 * \param [in] fPX : X coordinate of the center point
			 * \param [in] fPY : Y coordinate of the center point
			 * \param [in] fPZ : Z coordinate of the center point
			 * \param [in] ui8R: R color value
			 * \param [in] ui8G: G color value
			 * \param [in] ui8B: B color value
			 */						
			SWCloud(cfloat fPX, cfloat fPY, cfloat fPZ, cuint8 ui8R = 0, cuint8 ui8G = 0, cuint8 ui8B = 255);


            /**
             * \brief Constructor of SWCloud
             * \param [in] oCloud : input cloud  to copy
             */
            SWCloud(const SWCloud &oCloud);            

            /**
             * @brief SWCloud::operator =
             * @param [in] oCloud : cloud to be copied
             * @return
             */
            SWCloud &SWCloud::operator=(const SWCloud &oCloud);


            /**
             * @brief saveToObj
             * @param sPath
             * @param sNameObj
             * @return
             */
            bool saveToObj(const std::string &path, const std::string &nameObj);

			/**
			 * \brief destructor of SWCloud
			 */		
			virtual ~SWCloud(void);		
		
			// ############################################# METHODS
		
			/**
			 * \brief get the number of points of the cloud
			 * \return size of the point cloud
			 */
			uint size() const;

			/**
			 * \brief get the size of the vertex/color array
			 * \return size of the vertex/color array
			 */
			uint arraySize() const;

            /**
             * \brief get the point of the cloud corresponding to the input index (no index check validity)
             * \param [int,out] a3FXYZ      : in -> a 3-size float allocated array, out -> the array is filled with the point coordinates [x,y,z]
             * \param [int] ui32IndexPoint  : input index of the cloud point
             */
            void point(float *a3FXYZ, cuint ui32IndexPoint) const;

            /**
             * \brief get the point of the cloud corresponding to the input index
             * \param [out] oPoint         : vector containing the point coordinates
             * \param [int] ui32IndexPoint : input index of the cloud point
             */
            template <typename T>
            void point(std::vector<T> &oPoint, cuint ui32IndexPoint) const
            {
                if(ui32IndexPoint < size())
                {
                    oPoint = std::vector<T>(3);
                    oPoint[0] = coord(0)[ui32IndexPoint];
                    oPoint[1] = coord(1)[ui32IndexPoint];
                    oPoint[2] = coord(2)[ui32IndexPoint];
                }
                else
                {
                    std::cerr << "Error : point SWCloud, input index is not valid. (" << ui32IndexPoint << ")" << std::endl; // TODO : throw
                }
            }

			/**
			 * \brief return a pointer to the choosen part of the coordinate
			 * \param [in] ui32IdCord : 0 -> X, 1 -> Y, 2 -> Z
			 * \return return a pointer on m_aFCoords
			 */		
			float *coord(cuint ui32IdCord) const;
		                       
			/**
			 * \brief return a pointer to the choosen part of the colors
			 * \param [in] ui32IdColor : RGB_R | 0 -> R, RGB_G | 1 -> G, RGB_B | 2 -> B			 
			 * \return return a pointer on m_aFCoords
			 */			
			uint8 *color(cuint ui32IdColor) const;
			
			/**
			 * \brief Retrieve the minimum coordinates values of the SWCloud bbox
			 * \param [in,out] fMinX : minimum X value
			 * \param [in,out] fMinY : minimum Y value		 
			 * \param [in,out] fMinZ : minimum Z value	 
			 */			
			void lowerBBoxPoint(float &fMinX, float &fMinY, float &fMinZ) const;
			
			/**
			 * \brief Retrieve the maximum coordinates values of the SWCloud bbox
			 * \param [in,out] fMaxX : maximum X value 
			 * \param [in,out] fMaxY : maximum Y value		 
			 * \param [in,out] fMaxZ : maximum Z value			 
			 */				
			void upperBBoxPoint(float &fMaxX, float &fMaxY, float &fMaxZ) const;

			/**
			 * \brief get the bounding box of the cloud.			 
			 * \return a SWCloudBBox bbox
			 */	
			SWCloudBBox bBox() const;
				
			/**
			 * \brief Create a SWCloud containing all the ridges of the input bbox.
			 * \param [in]  oBBox 	   	       : input bbox
			 * \param [out] oCloudBBox 	       : the output cloud bbox
			 * \param [in]  i32RidgeNumberOfPoints : the number of points for each ridge.
			 */			
			static void cloudBBox(const SWCloudBBox &oBBox, SWCloud &oCloudBBox, cuint i32RidgeNumberOfPoints);						
						
			/**
			 * \brief Create a SWCloud containing all the ridges of the current bbox.
			 * \param [out] oCloudBBox 	       : the output cloud bbox
			 * \param [in]  i32RidgeNumberOfPoints : the number of points for each ridge.
			 */			
			void cloudBBox(SWCloud &oCloudBBox, cuint i32RidgeNumberOfPoints = 1000);
		
		
			/**
			 * \brief Erase all the points of the current cloud which are not inside the input 2D bbox (the z value is not used).
			 * \param [in]  oBBox	: input bbox
			 */			
			void bBox2DFilter(const SWCloudBBox &oBBox);


			/**
			 * \brief set coordinates array and color array, previous data si deleted.
			 * \param [in] vPX : vector of input X float point
			 * \param [in] vPY : vector of input Y float point
			 * \param [in] vPZ : vector of input Z float point
			 */			
			void set(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ);

            /**
             * @brief set coordinates array and color array, previous data si deleted.
             * \param [in] vPX : vector of input X float point
             * \param [in] vPY : vector of input Y float point
             * \param [in] vPZ : vector of input Z float point
             * @param [in] vR  : vector of input R int color point
             * @param [in] vG  : vector of input G int color point
             * @param [in] vB  : vector of input B int color point
             */
            void set(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ,
                     const std::vector<uint8> &vR, const std::vector<uint8> &vG, const std::vector<uint8> &vB);
		
			/**
			 * \brief set coordinates array and color array, previous data si deleted
			 * (if the SWCloud is destroyed later, input data will be deleted)
			 * \param [in] ui32NumberOfPoints : number of points in the cloud
			 * \param [in] aFCoords   	  : coordonitates of the cloud to set
			 * \param [in] aUi8Colors 	  : colors of the cloud to set
			 */			
			void set(cuint ui32NumberOfPoints, float *aFCoords, uint8 *aUi8Colors);
			
			/**
			 * \brief set the input rgb color for all the points
			 * \param [in] ui8R : R component of the RGB color
			 * \param [in] ui8G : G component of the RGB color
			 * \param [in] ui8B : B component of the RGB color
			 */			
			void setUnicolor(cuint8 ui8R, cuint8 ui8G, cuint8 ui8B);
		
			/**
			 * \brief init the cloud by copying data from the input SWCloud, previous data is deleted
			 * \param [in] oCloud : the cloud to copy
			 */			
			void copy(const SWCloud &oCloud);

			/**
			 * \brief init the cloud by copying the input data, previous data is deleted
			 * \param [in] ui32NumberOfPoints : number of points in the cloud
			 * \param [in] aCoords   	 : coordonitates of the cloud to copy
			 * \param [in] aUi8Colors 	 : colors of the cloud to copy 
			 */			
			void copy(cuint ui32NumberOfPoints, cfloat *aCoords, cuint8 *aUi8Colors);
		
			/**
			 * \brief init the cloud by copying the input data, previous data is deleted
			 * \param [in] ui32NumberOfPoints : number of points in the cloud
			 * \param [in] aCoords  		 : coordonitates of the cloud to copy
			 * \param [in] ui8ColorR		 : R color component of the cloud 
			 * \param [in] ui8ColorG		 : G color component of the cloud 
			 * \param [in] ui8ColorB		 : B color component of the cloud 
			 */	
			void copy(cuint ui32NumberOfPoints, const float *aCoords, cuint8 ui8ColorR, cuint8 ui8ColorG, cuint8 ui8ColorB);
		
			/**
			 * \brief add the input point to all the values of the cloud
			 * \param [in] oPoint : vector containing point coordinates
			 * \return the current cloud with all the points modified
			 */		
			SWCloud& operator+=(const std::vector<float> &oPoint);           
		
			/**
			 * \brief copy and add all the points from the input cloud to the current cloud
			 * \param [in] oCloud : the input cloud to add
			 * \return the current cloud with all the points of the input one added
			 */		
            SWCloud& operator+=(const SWCloud &oCloud);

            /**
             * \brief scale the cloud by multiplying all the points coordinates by the input value.
             * \param [in] fScaleValue : the scaling value
             * \return the current cloud scaled
             */
            SWCloud& operator*=(cfloat fScaleValue);

            /**
             * \brief A basic loader that will initialize the SWCloud with the obj vertices.
             * \param [in] sPathObjFile  : path of the obj file
             * \return true if successfull else return false
             */
            bool loadObj(const std::string &sPathObjFile);

			/**
			 * \brief delete all the cloud data	 
			 */			
			void erase();
		
			/**
			 * \brief reduce the cloud by deleting random points
             * \param [in] fRandomSamplingPercentage :factor of the number of points to keep (0,1)
			 * \param [in] fMinDistBeforeReduction   : minimum distance before reduction
			 * \return true if success, else return false			 
			 */			
			bool reduce(float fRandomSamplingPercentage, float fMinDistBeforeReduction = 0);


            void reduce2(int randomSamplingPercentage);

			/**
			 * \brief apply the input transformation to the cloud
			 * \param [in] m_aFRotationMatrix    : rotation matrix [3x3]
			 * \param [in] m_aFTranslationMatrix : translation matrix [3]
			 * \return true if success, else return false			 
			 */		
			bool transform(cfloat *m_aFRotationMatrix, cfloat *m_aFTranslationMatrix);
			
			/**
             * \brief compute a "distanc" value of the cloud from the input cloud
			 * \param [in] oCloud          : input cloud
             * \param [in] bReduce         : reduce the cloud ?
			 * \param [in] ui32CoeffReduce : reduce factor of the cloud
			 * \return the square distance mean	 
			 */				
			float squareDistanceCloud(const SWCloud &oCloud, cbool bReduce = false, cfloat ui32CoeffReduce = 50);
			
			/**
			 * \brief get the distance of the point from the cloud
			 * \param [in] oCloud       : input cloud
			 * \param [in] ui32IndiceP  : index of hte point
			 * \return the square distance		 
			 */				
			float squareDistancePoint(const SWCloud &oCloud, cuint ui32IndiceP);
			
			/**
			 * \brief Move the SWCloud to the origin by computing a vector with the origin and the mean point of the SWCloud
			 * \return the mean vector		 
			 */			
			std::vector<float> moveToOrigine();

            /**
             * \brief Compute the mean position of all the points of the cloud and return it.
             * \return the mean point of the cloud (3D float point)
             */
            std::vector<float> meanPoint() const;

			/**
			 * \brief Get a copy of a part of the SWCloud
			 * \param [in,out] oCloudPart  	  : result part of the SWCloud
			 * \param [in] ui32BeginIndexPoint : index of the first point to add
			 * \param [in] ui32EndIndexPoint   : index of the last point to add, must be superior than ui32BeginIndexPoint
			 * \return false if bad parameters, else return true
			 */		
			bool retrieveCloudPart(SWCloud &oCloudPart, cuint32 ui32BeginIndexPoint, cuint32 ui32EndIndexPoint);


            /**
             * @brief Delete all the points of the cloud which are outside the input BBox.
             * @param [in] oCloudBBox : bbox
             */
            void keepOnlyPointInsideBBox(const SWCloudBBox &oCloudBBox);

			/**
			 * \brief Get a copy of all the points coordinates in an array [x0,y0,z0,x1,y1,z1,...,xn,yn,zn] (used for opengl)
             * \return the float array or a NULL pointer
			 */
			float *vertexBuffer() const;
			
			/**
			 * \brief Return an unsigned integer array containing the order of the index of the cloud vertex [1,2,3, ... ,n] (used for opengl)
             * \return the uint32 array or a NULL pointer
			 */		
			uint32 *indexBuffer() const;
						
			/**
			 * \brief Get a copy of all points colors in an array [R0,G0,B0,R1,G1,B1,...,Rn,Gn,Bn] (used for opengl)
             * \return the float array or a NULL pointer
			 */			
			float *colorBuffer() const;
			
            /**
             * \brief Compute the id of the nearest cloud point from the input point.
             * \param [in] oPt : point to compare
             * \return the id of the nearset cloud point.
             */
            int idNearestPoint(const std::vector<float> &oPt, cfloat fDistMin = 0.f) const;
			
            static int m_i32NumberOfCreatedClouds;	/**< DEBUG : number of cloud created since the launch of the program */
			
            static int m_i32NumberOfDestroyedClouds;/**< DEBUG : number of cloud destroyed since the launch of the program */

		protected:
		
            uint m_ui32NumberOfPoints;  /**< number of points in the cloud */
		
            uint m_ui32ArraySize;       /**< size of the arrays  */
		
            float *m_aFCoords;          /**< pointer on the coordinates of the cloud [x1, x2, ..., xn, y1, ..., yn, z1, ...,zn] */
		
            uint8 *m_aUi8Colors;        /**< pointer on the color of the points
                                        [R1, R2, ..., Rn, G1, ..., Gn, B1, ..., Bn] */

//            bool m_bBuffersComputed;
//            int *m_aI32IntedxBuffer;
//            float *m_aFVertexBuffer;
//            float *m_aFColorBuffer;


					           
			/**
			 * \brief increase the size of the cloud point by the input value
             * \param [in] ui32SizeToAdd : size to add, if not defined, the size is doubled
			 */				
            void upSize(cuint ui32SizeToAdd = 0);
	};
}

#endif


/**
 * \brief Compute the id of the nearest target cloud point from the input source cloud point.
 * \param [in] i32IdSourcePoint : id of the source cloud point to test
 * \param [in] oTarget          : target cloud point
 * \param [in] fDistMax         : if a point is closeer than this distance, it is not considered.
 * \return the id of the nearset target cloud point.
 */
//            int idNearestPoint(cint i32IdSourcePoint, const SWCloud &oTarget, float fDistMin = 0.f) const;

/**
 * \brief ...
 * \param [in|out] oMeanPosition    : ...
 * \param [in] i32IdSourcePoint     : ...
 * \param [in] oTarget              : ...
 * \param [in] i32NearPointsNumber  : ...
 * \param [in] fDistMax             : ...
 ** \return ...
 */
//            int meanNearestPosition(std::vector<float> &oMeanPosition, cint i32IdSourcePoint, const SWCloud &oTarget, int i32NearPointsNumber, cfloat fDistMax) const;

/**
 * \brief ...
 * \param [in] oTarget : ...
 * \param [in] i32NearPointsNb : ...
 * \param [in] fDeviation : ...
 ** \return ...
 */
//            void SWCloud::adaptCloud(const SWCloud &oTarget, cint i32NearPointsNb, cfloat fDeviation);

/**
 * \brief Checks if the cloud contains a point which is closer to the input point in the 2D plan than the input offsets.
 * \param [in]  v3FPoint	: input point to compare
 * \param [in]  fOffsetX	: x coordinate offset
 * \param [in]  fOffsetY	: y coordinate offset
 * \return true if a point exists else return false
 */
//            bool isNearbyPointsIn2DPlan(std::vector<float> &v3FPoint, cfloat fOffsetX, float fOffsetY);

/**
 * \brief ... TODO : doc
 * \param [in]  v3FPoint	: input point to compare
 * \param [in]  fOffsetX	: x coordinate offset
 * \param [in]  fOffsetY	: y coordinate offset
 * \return the z mean position of the nearby points
 */
//            float computeMeanZPositionOfNearbyPointsIn2DPlan(std::vector<float> &v3FPoint, cfloat fOffsetX, float fOffsetY);
