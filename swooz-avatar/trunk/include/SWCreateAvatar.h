
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
 * \file SWCreateAvatar.h
 * \brief Defines SWCreateAvatar
 * \author Florian Lance
 * \date 05/11/12
 */

#ifndef _SWCREATEAVATAR_
#define _SWCREATEAVATAR_

#include "devices/rgbd/SWKinectParams.h"
#include "cloud/SWAlignClouds.h"
#include "detect/SWStasm.h"
#include "detect/SWFaceDetection.h"
#include "mesh/SWMesh.h"

typedef boost::shared_ptr<swDetect::SWStasm> SWStasmPtr; /**< boost shared pointer for SWStasm */
typedef boost::shared_ptr<swDetect::SWFaceDetection> SWFaceDetectionPtr; /**< boost shared pointer for SWFaceDetection */

/**
 * \class SWCreateAvatar
 * \brief Use rgbd device data to create a 3D avatar
 * \author Florian Lance
 * \date 05/11/12
 */
class SWCreateAvatar
{
    public:
        // ############################################# CONSTRUCTORS / DESTRUCTORS

        /**
         * \brief constructor of SWCreateAvatar
         * \param [in] bVerbose      : enable verbose display
         */
        SWCreateAvatar(cbool bVerbose = false);

        // ############################################# METHODS

        /**
         * @brief Reset computed data.
         */
        void resetData();

        /**
         * @brief accessor
         * @return return the last computed face rectangle
         */
        cv::Rect lastRectFace() const;

        /**
         * @brief accessor
         * @return return the last computed nose rectangle
         */
        cv::Rect lastRectNose() const;

        /**
         * @brief lastRadialProjection
         * @param [out] oFilteredRadialProj : radial project mat
         */
        void lastRadialProjection(cv::Mat &oFilteredRadialProj) const;

        /**
         * @brief Return the last face mesh computed.
         * @param [out] oResultMesh : face mesh
         */
        void lastResultFaceMesh(swMesh::SWMesh &oResultMesh);

        /**
         * @brief get a copy of the total cloud
         * @param [out] oTotalCloud : total cloud
         */
        void totalCloud(swCloud::SWCloud &oTotalCloud);

        /**
         * @brief build a face cloud with the input data of the kinect and add it to the total face cloud
         * @param [in] oRgb   : input rgb image
         * @param [in] oDepth : input depth image
         * @return false if no cloud has been added to the total cloud, else return true
         */
        bool addCloudToAvatar(const cv::Mat &oRgb, const cv::Mat &oDepth);

        /**
         * @brief constructAvatar
         */
        void constructAvatar();

        /**
         * @brief setHeightRadialProjection
         * @param ui32HeightRadialProj
         */
        void setHeightRadialProjection(cuint ui32HeightRadialProj);

        /**
         * @brief setWidthRadialProjection
         * @param ui32WidthRadialProj
         */
        void setWidthRadialProjection(cuint ui32WidthRadialProj);

        /**
         * @brief setBilateralFilter
         * @param i32BilateralFilterIteration
         */
        void setBilateralFilter(cint i32BilateralFilterIteration);

        /**
         * @brief setColorFilterValue
         * @param ui32ColorFilterValue
         */
        void setColorFilterValue(cuint ui32ColorFilterValue);

        /**
         * @brief setSpaceFilterValue
         * @param ui32SpaceFilterValue
         */
        void setSpaceFilterValue(cuint ui32SpaceFilterValue);

        /**
         * @brief setCylinderRadiusValue
         * @param fRadius
         */
        void setCylinderRadiusValue(cfloat fRadius);

        /**
         * @brief setUseBilateralFilter
         * @param bUseFilter
         */
        void setUseBilateralFilter(cbool bUseFilter);

        /**
         * @brief setUseStasm
         * @param bUseStasm
         */
        void setUseStasm(cbool bUseStasm);

        /**
         * @brief setErodeValue
         * @param ui32Erode
         */
        void setErodeValue(cuint ui32Erode);

        /**
         * @brief setDilateValue
         * @param ui32Dilate
         */
        void setDilateValue(cuint ui32Dilate);

        /**
         * @brief setExpandValue
         */
        void setExpandValue(const int);

        /**
         * @brief setExpandConnex
         */
        void setExpandConnex(const int);

        /**
         * @brief setEraseValue
         */
        void setEraseValue(const int);

        /**
         * @brief setEraseConnex
         */
        void setEraseConnex(const int);

        /**
         * @brief setDepthCloud
         */
        void setDepthCloud(const float);

        /**
         * @brief addPixelToDelete
         */
        void addPixelToDelete(const std::vector<int>);


        /**
         * @brief resetDeletedPointsRadialProj
         */
        void resetDeletedPointsRadialProj();


        /**
         * @brief computeSTASMCoords
         */
        void computeSTASMCoords();


        /**
         * @brief mesh_stasm_points_index
         * @return
         */
        std::vector<int> mesh_stasm_points_index() const;

    private:

        // parameters
        //  miscellaneous
        bool m_bVerbose;                        /**< enable verbose display info mode */
        //  alignment
        float m_fTemplateDownScale;             /**< template cloud reduction scale */
        float m_fTargetDownScale;               /**< target cloud reduction scale*/
        float m_fDistMaxAlignment;              /**< mean distance max between the current cloud and the reference cloud */
        //  stasm
        bool m_bDetectStasmPoints;              /**< use stasm for detecting features points */
        //  radial proj
        int m_i32Dilate;                        /**< dilate value used on the radial projection */
        int m_i32Erode;                         /**< erode value used on the radial projection */
//        int m_i32WidthRadialProj;               /**< width of the radial projection mat image */
//        int m_i32HeightRadialProj;              /**< height of the radial projection mat image */
        float m_fCylinderRadius;                /**< radius of the cylinder used for the radial projection */
        int m_i32ExpandValue;                   /**< ... */
        int m_i32ExpandConnex;                  /**< ... */
        int m_i32EraseValue;                    /**< ... */
        int m_i32EraseConnex;                   /**< ... */
        //  bgr
        float m_fRemoveBackGroundDistance;      /**< distance max for a pixel to be considered as not a part of the background */
        //  cloud
        int m_i32NumCloud;                      /**< number of cloud to sum */
        float m_fDepthCloud;                    /**< distance for creating the cloud */
        //  bilateral filter
        bool m_bUseBilateralFilter;             /**< apply bilateral filter ? */
        int m_i32BilateralFilter;               /**< d bilateral filter*/
        int m_i32ColorFilterValue;              /**< color value bilateral filter*/
        int m_i32SpaceFilterValue;              /**< space value bilateral filter*/        

        // alignment
        swCloud::SWCloudBBox m_oCloudFaceBBox;              /**< face cloud bbox */
        swCloud::SWCloud m_oFaceCloudRef;                   /**< face cloud reference used for alignment */
        swCloud::SWCloud m_oNoseCloudRef;                   /**< nose cloud reference used for alignment */
        swCloud::SWAlignClouds m_oAlignClouds;              /**< swooz clouds alignment */

        // results
        cv::Mat m_oFilteredRadialProjection;                /**< radial projection result */
        std::vector<uint> m_vUi32CloudNumbersOfPoints; 	   	/**< number of points for each cloud of the accumated cloud */
        swCloud::SWCloud m_oAccumulatedFaceClouds;          /**< addition of all the aligned face clouds  */
        swMesh::SWMesh m_oLastResultFaceMesh;               /**< last face mesh computed */

        // detection
        //  haar cascade
        cv::Rect m_oLastRectFace;                           /**< last face rectangle */
        cv::Rect m_oLastRectNose;                           /**< last nose rectangle */
        SWFaceDetectionPtr m_CFaceDetectPtr;                /**< detect face pointer */
        //  stasm
        std::vector<cv::Point3f> m_vP3FStasm3DPoints;       /**< array of stasms 3D points*/
//        std::vector<cv::Point3f> m_vP3FTotalStasm3DPoints;  /**< sum of array of stasms 3D points*/
        std::vector<std::vector<cv::Point3f> > m_vStasm3DPoints;  /**< sum of array of stasms 3D points*/
        std::vector<int> m_vMeshIdSTASMPoints;              /**< ... */


    public:
        SWStasmPtr               m_CStasmDetectPtr;         /**< stasm detection pointer */
        cv::Mat m_oTextureMat;                              /**< face texture */ // TODO : acc

        int m_i32WidthRadialProj;               /**< width of the radial projection mat image */
        int m_i32HeightRadialProj;              /**< height of the radial projection mat image */


        // test
        std::vector<std::vector<int> > m_vPixelToDelete;

};

#endif
