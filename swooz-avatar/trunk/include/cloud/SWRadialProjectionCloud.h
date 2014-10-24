

/**
 * \file SWRadialProjectionCloud.h
 * \brief Contains utility functions for radial projections
 * \author Florian Lance
 * \date 29/01/13
 */

#ifndef _SWRADIALPROJECTIONCLOUD_
#define _SWRADIALPROJECTIONCLOUD_

// std
#include <math.h>
#include <stack>

// swooz
#include "commonTypes.h"
#include "cloud/SWCloud.h"
#include "mesh/SWMesh.h"

namespace swCloud
{
    static int aa = 0;

    /**
     * \brief  Project a point on an axe
     * \param  [in] oPointToProj : point to project on the axe
     * \param  [in] oAxePoint    : a point on the axe
     * \param  [in] oAxeVector   : the axe direction
     * \return  the projected point
     */
    static cv::Vec3f projectOnAxe(const cv::Vec3f &oPointToProj, const cv::Vec3f &oAxePoint, const cv::Vec3f &oAxeVector)
    {
        cv::Vec3f l_oAxeUVector;
        cv::normalize(oAxeVector, l_oAxeUVector);
        cv::Vec3f l_oAD = 100.f*l_oAxeUVector;

        return (oAxePoint + (l_oAD.dot(oPointToProj - oAxePoint) / check0Div(cv::norm(l_oAD))) * l_oAxeUVector);
    }

    /**
     * \brief Add the textures coordinates in the flow of the obj file
     * \param  [in,out] oFlow  	: flow of the obj
     * \param  [in] oFX	 	: x coordinates array of the obj vertex
     * \param  [in] oFY		: y coordinates array of the obj vertex
     * \param  [in] sBBoxFaceCloud  : ...
     */
    static void addTextureCoordRadialProj(std::ofstream &oFlow, const std::vector<float> &vFX, const std::vector<float> &vFY, const SWCloudBBox &sBBoxFaceCloud)
    {
        for(uint ii = 0; ii < vFX.size(); ++ii)
        {
            // compute relative texture coordinate
            float l_fXCoordinate = (vFX[ii] - sBBoxFaceCloud.m_fMinX )/check0Div(sBBoxFaceCloud.m_fMaxX - sBBoxFaceCloud.m_fMinX);
            float l_fYCoordinate = (vFY[ii] - sBBoxFaceCloud.m_fMinY )/check0Div(sBBoxFaceCloud.m_fMaxY - sBBoxFaceCloud.m_fMinY);

            std::ostringstream l_osCX, l_osCY;
            l_osCX << l_fXCoordinate;
            l_osCY << l_fYCoordinate;

            // add current computed coordinate in the flow
            oFlow << "vt " + l_osCX.str() + " " +  l_osCY.str() << endl;
        }
    }


    /**
     * \brief Add normals in the flow of the obj file
     * \param  [in,out] oFlow  	: flow of the obj
     * \param  [in] oIndexMask	: mask of the valid vertex
     * \param  [in] oFX	 	: x coordinates array of the obj vertex
     * \param  [in] oFY		: y coordinates array of the obj vertex
     * \param  [in] oFZ		: z coordinates array of the obj vertex
     */
    static void addNormalsRadialProj(std::ofstream &oFlow, cv::Mat &oIndexMask, const std::vector<float> &vFX, const std::vector<float> &vFY, const std::vector<float> &vFZ)
    {
        // 1 2 3
        // 4 0 5
        // 6 7 8
        // 0 -> current point

        // grid mask of the valid vertex
        std::vector<bool>  l_vBValidGrid(9);
        // grid index of the vertex
        std::vector<int>   l_vI32IndexGrid(9);
        // grid points
        std::vector<cv::Vec3f> l_vP3fGrid(9);

        for(int ii = 1; ii < oIndexMask.rows-1; ++ii)
        {
            for(int jj = 1; jj < oIndexMask.cols-1; ++jj)
            {
                // if the current vertex is valid
                if(oIndexMask.at<int>(ii,jj) != 0)
                {
                    l_vI32IndexGrid[0] = oIndexMask.at<int>(ii,jj);
                    l_vI32IndexGrid[1] = oIndexMask.at<int>(ii-1,jj-1);
                    l_vI32IndexGrid[2] = oIndexMask.at<int>(ii-1,jj);
                    l_vI32IndexGrid[3] = oIndexMask.at<int>(ii-1,jj+1);
                    l_vI32IndexGrid[4] = oIndexMask.at<int>(ii,jj-1);
                    l_vI32IndexGrid[5] = oIndexMask.at<int>(ii,jj+1);
                    l_vI32IndexGrid[6] = oIndexMask.at<int>(ii+1,jj-1);
                    l_vI32IndexGrid[7] = oIndexMask.at<int>(ii+1,jj);
                    l_vI32IndexGrid[8] = oIndexMask.at<int>(ii+1,jj+1);

                    // init grid mask and grid point
                    for(int kk = 0; kk < 9; ++kk)
                    {
                        l_vBValidGrid[kk] = l_vI32IndexGrid[kk] != 0;
                        if(l_vBValidGrid[kk])
                        {
                            l_vP3fGrid[kk]	  = cv::Vec3f(vFX[l_vI32IndexGrid[kk]], vFY[l_vI32IndexGrid[kk]], vFZ[l_vI32IndexGrid[kk]]);
                        }
                    }

                    cv::Vec3f l_oTotalNormal(0,0,0);
                    cv::Vec3f l_oCurrNormal(0,0,0);

                    if(l_vBValidGrid[4] && l_vBValidGrid[2])
                    {
                        cv::normalize((l_vP3fGrid[4] - l_vP3fGrid[0]).cross(l_vP3fGrid[2] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }
                    if(l_vBValidGrid[1] && l_vBValidGrid[3])
                    {
                        cv::normalize((l_vP3fGrid[1] - l_vP3fGrid[0]).cross(l_vP3fGrid[3] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }
                    if(l_vBValidGrid[2] && l_vBValidGrid[5])
                    {
                        cv::normalize((l_vP3fGrid[2] - l_vP3fGrid[0]).cross(l_vP3fGrid[5] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }
                    if(l_vBValidGrid[3] && l_vBValidGrid[8])
                    {
                        cv::normalize((l_vP3fGrid[3] - l_vP3fGrid[0]).cross(l_vP3fGrid[8] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }
                    if(l_vBValidGrid[5] && l_vBValidGrid[7])
                    {
                        cv::normalize((l_vP3fGrid[5] - l_vP3fGrid[0]).cross(l_vP3fGrid[7] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }
                    if(l_vBValidGrid[8] && l_vBValidGrid[6])
                    {
                        cv::normalize((l_vP3fGrid[8] - l_vP3fGrid[0]).cross(l_vP3fGrid[6] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }
                    if(l_vBValidGrid[7] && l_vBValidGrid[4])
                    {
                        cv::normalize((l_vP3fGrid[7] - l_vP3fGrid[0]).cross(l_vP3fGrid[4] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }
                    if(l_vBValidGrid[6] && l_vBValidGrid[1])
                    {
                        cv::normalize((l_vP3fGrid[6] - l_vP3fGrid[0]).cross(l_vP3fGrid[1] - l_vP3fGrid[0]), l_oCurrNormal);
                        l_oTotalNormal -= l_oCurrNormal;
                    }


                    if(l_oTotalNormal == cv::Vec3f(0,0,0))
                    {
                        l_oTotalNormal = cv::Vec3f(0,0,-1);
                    }
                    else // normalize
                    {
                        cv::normalize(l_oTotalNormal, l_oTotalNormal);
                    }

                    std::ostringstream l_os1, l_os2, l_os3;
                    l_os1 << l_oTotalNormal[0];
                    l_os2 << l_oTotalNormal[1];
                    l_os3 << l_oTotalNormal[2];
                    oFlow << "vn " + l_os1.str() + " " + l_os2.str() + " " + l_os3.str() << endl;
                }
            }
        }
    }


    /**
     * \brief Add the faces corresponding to the input index mat in the flow of the obj file
     * \param  [in,out] oFlow   : flow of the obj
     * \param  [in] oIndexMask  : input vertex index mat
     */
    static void addFacesRadialProj(std::ofstream &oFlow, cv::Mat &oIndexMask)
    {
        for(int ii = 1; ii < oIndexMask.rows; ++ii)
        {
            for(int jj = 1; jj < oIndexMask.cols; ++jj)
            {
                if(oIndexMask.at<int>(ii,jj) != 0)
                {
                    // .     . - .
                    // | \     \ |
                    // . - .     .
                    if(oIndexMask.at<int>(ii-1,jj-1) != 0)
                    {
                        if(oIndexMask.at<int>(ii,jj-1) != 0)
                        {
                            std::ostringstream l_osF1, l_osF2, l_osF3;
                            l_osF1 << oIndexMask.at<int>(ii,jj);
                            l_osF2 << oIndexMask.at<int>(ii-1,jj-1);
                            l_osF3 << oIndexMask.at<int>(ii,jj-1);
                            oFlow << "f " + l_osF1.str() + "/" + l_osF1.str() + "/" + l_osF1.str() + " " +
                                    l_osF2.str() + "/" + l_osF2.str() + "/" + l_osF2.str() + " " +
                                    l_osF3.str() + "/" + l_osF3.str() + "/" + l_osF3.str() << endl;
                        }
                        if(oIndexMask.at<float>(ii-1,jj) != 0)
                        {
                            std::ostringstream l_osF1, l_osF2, l_osF3;
                            l_osF1 << oIndexMask.at<int>(ii,jj);
                            l_osF2 << oIndexMask.at<int>(ii-1,jj);
                            l_osF3 << oIndexMask.at<int>(ii-1,jj-1);
                            oFlow << "f " + l_osF1.str() + "/" + l_osF1.str() + "/" + l_osF1.str() + " " +
                                    l_osF2.str() + "/" + l_osF2.str() + "/" + l_osF2.str() + " " +
                                    l_osF3.str() + "/" + l_osF3.str() + "/" + l_osF3.str() << endl;
                        }
                    }
                    //     .
                    //   / |
                    // . - .
                    else
                    {
                        if(oIndexMask.at<int>(ii,jj-1) != 0 && oIndexMask.at<int>(ii-1,jj) != 0)
                        {
                            std::ostringstream l_osF1, l_osF2, l_osF3;
                            l_osF1 << oIndexMask.at<int>(ii,jj);
                            l_osF2 << oIndexMask.at<int>(ii-1,jj);
                            l_osF3 << oIndexMask.at<int>(ii,jj-1);
                            oFlow << "f " + l_osF1.str() + "/" + l_osF1.str() + "/" + l_osF1.str() + " " +
                                    l_osF2.str() + "/" + l_osF2.str() + "/" + l_osF2.str() + " " +
                                    l_osF3.str() + "/" + l_osF3.str() + "/" + l_osF3.str() << endl;
                        }
                    }
                }
            }
        }
    }

    /**
     * \brief  Project the points of a SWCloud on a cylinder and put the result in a mat gray image
     * \param  [in] oCloud            : input SWCloud to project
     * \param  [in] oGrayImageResult  : result mat gray image
     * \param  [in] oCloudTemp        : cloud containing projected points on the cylinder and on the axe
     * \param  [in] sBBox             : ...
     * \param  [in] ui32WidthImage    : width of the result image
     * \param  [in] ui32HeightImage   : height of the result image
     * \param  [in] fCylinderRadius   : radius of the cylinder
     */
    static void radialProjCloudOnMat(const SWCloud &oCloud, cv::Mat &oGrayImageResult, SWCloud &oCloudTemp, const SWCloudBBox &sBBox,
                     cuint32 ui32WidthImage = 800, cuint32 ui32HeightImage = 400, cfloat fCylinderRadius = 0.3f) // , cv::Mat &oProjectedResult
    {
        float l_fDistToUse, l_fAngleToAdd;

        // defines distances bewteen current point and reference points
            float l_fDistCRef1, l_fDistCRef2, l_fDistCRef3, l_fDistCRef4;
            float l_fMinDist = FLT_MAX, l_fMaxDist = 0.f, l_fCurrDist;

        // defines vectors
            cv::Vec3f l_oAxeVector(0.f, 1.f, 0.f), l_oCurrPoint, l_oCurrVector, l_oNormCurrVector;
            cv::Vec3f l_oAxePoint, l_oProjectedOnAxePoint, l_oProjectedOnCylinderPoint;
            cv::Vec3f l_oCurrRefProjectedOnCylinderPoint1, l_oCurrRefProjectedOnCylinderPoint2;
            cv::Vec3f l_oCurrRefProjectedOnCylinderPoint3, l_oCurrRefProjectedOnCylinderPoint4;

        // defines display cloud vectors (temp)
//             std::vector<float> l_vFX,  l_vFY,  l_vFZ;
//             std::vector<float> l_vFX2, l_vFY2, l_vFZ2;
//             std::vector<float> l_vFX3, l_vFY3, l_vFZ3;
//             std::vector<float> l_vFX4, l_vFY4, l_vFZ4;
//             std::vector<float> l_vFX5, l_vFY5, l_vFZ5;
//             std::vector<float> l_vFX6, l_vFY6, l_vFZ6;


        // init cylinder axe point
            l_oAxePoint[0] = 0.5f * (sBBox.m_fMinX + sBBox.m_fMaxX);
            l_oAxePoint[1] = sBBox.m_fMinY;
            l_oAxePoint[2] = sBBox.m_fMinZ + 0.1f;//0.5f * (sBBox.m_fMinZ + sBBox.m_fMaxZ);
            l_fMinDist = 0.f;
            l_fMaxDist = fCylinderRadius;

        // init result mat
            float l_fMaxHeight = (sBBox.m_fMaxY - sBBox.m_fMinY);

        // cv::Mat l_oProjectedMat(ui32HeightImage, ui32WidthImage, CV_8UC1);
            cv::Mat l_oProjectedMat(ui32HeightImage, ui32WidthImage , CV_32FC1);

        // set the mat to 0
            cv::Scalar l_oScalInit;
            l_oScalInit[0] = 0.f;
            l_oProjectedMat = l_oScalInit;

        for(uint ii = 0; ii < oCloud.size(); ++ii)
        {
            // retrieve current point
                l_oCurrPoint[0] = oCloud.coord(0)[ii];
                l_oCurrPoint[1] = oCloud.coord(1)[ii];
                l_oCurrPoint[2] = oCloud.coord(2)[ii];

            // project the current point on the cylinder axe
                l_oProjectedOnAxePoint = projectOnAxe(l_oCurrPoint, l_oAxePoint, l_oAxeVector);

            // init of the current vector
                l_oCurrVector = l_oCurrPoint - l_oProjectedOnAxePoint;
                cv::normalize(l_oCurrVector, l_oNormCurrVector);
                l_oProjectedOnCylinderPoint = l_oProjectedOnAxePoint + fCylinderRadius * l_oNormCurrVector;

                l_fCurrDist = (float)cv::norm(l_oCurrVector);

            // defines ref points
                l_oCurrRefProjectedOnCylinderPoint1 = l_oProjectedOnAxePoint;
                l_oCurrRefProjectedOnCylinderPoint1[2] += fCylinderRadius;

                l_oCurrRefProjectedOnCylinderPoint2 = l_oProjectedOnAxePoint;
                l_oCurrRefProjectedOnCylinderPoint2[0] += fCylinderRadius;

                l_oCurrRefProjectedOnCylinderPoint3 = l_oProjectedOnAxePoint;
                l_oCurrRefProjectedOnCylinderPoint3[2] -= fCylinderRadius;

                l_oCurrRefProjectedOnCylinderPoint4 = l_oProjectedOnAxePoint;
                l_oCurrRefProjectedOnCylinderPoint4[0] -= fCylinderRadius;

            // add points to display cloud vectors
//                l_vFX.push_back(l_oProjectedOnCylinderPoint[0]);
//                l_vFY.push_back(l_oProjectedOnCylinderPoint[1]);
//                l_vFZ.push_back(l_oProjectedOnCylinderPoint[2]);

//                l_vFX2.push_back(l_oProjectedOnAxePoint[0]);
//                l_vFY2.push_back(l_oProjectedOnAxePoint[1]);
//                l_vFZ2.push_back(l_oProjectedOnAxePoint[2]);


            // set the current distances
                l_fDistCRef1 = (float)cv::norm(l_oCurrRefProjectedOnCylinderPoint1 - l_oProjectedOnCylinderPoint);
                l_fDistCRef2 = (float)cv::norm(l_oCurrRefProjectedOnCylinderPoint2 - l_oProjectedOnCylinderPoint);
                l_fDistCRef3 = (float)cv::norm(l_oCurrRefProjectedOnCylinderPoint3 - l_oProjectedOnCylinderPoint);
                l_fDistCRef4 = (float)cv::norm(l_oCurrRefProjectedOnCylinderPoint4 - l_oProjectedOnCylinderPoint);

                if(l_fDistCRef1  < l_fDistCRef3)
                {
                    if(l_fDistCRef2 < l_fDistCRef4)
                    {
                        l_fDistToUse = l_fDistCRef1;
                        l_fAngleToAdd = 0.f;
                    }
                    else
                    {
                        l_fDistToUse = l_fDistCRef4;
                        l_fAngleToAdd = 270.f;
                    }
                }
                else
                {
                    if(l_fDistCRef2 < l_fDistCRef4)
                    {
                        l_fDistToUse = l_fDistCRef2;
                        l_fAngleToAdd = 90.f;
                    }
                    else
                    {
                        l_fDistToUse = l_fDistCRef3;
                        l_fAngleToAdd = 180.f;
                    }
                }

            // compute the current angle
                float l_fAlpha = asin( (l_fDistToUse/2.f)/check0Div(fCylinderRadius)); // compute alpha rad value
                l_fAlpha *= 360.f / (float)M_PI; ; // conv rad to deg and add the good angle
                l_fAlpha += l_fAngleToAdd;

                float l_fGrayValue =  (l_fCurrDist - l_fMinDist)/check0Div(l_fMaxDist - l_fMinDist);

                uint l_ii = ui32HeightImage - (int)((l_oCurrPoint[1] - sBBox.m_fMinY) * ((ui32HeightImage)/check0Div(l_fMaxHeight)));
                if(l_ii == ui32HeightImage) // TODO : recompute values to write
                    l_ii--;

            // projection image
                uint l_jj = (int)(((ui32WidthImage)/360.f)*l_fAlpha);
                l_oProjectedMat.at<float>(l_ii, l_jj) = l_fGrayValue*255.f;
        }

        // set clouds to display (temp)
//             SWCloud newCloud(l_vFX, l_vFY, l_vFZ);
//             SWCloud newCloud1(l_vFX2, l_vFY2, l_vFZ2);
//             newCloud.setUnicolor(255,215,0);
//             newCloud1.setUnicolor(0,32,144);
//             newCloud += newCloud1;
//             oCloudTemp.copy(newCloud);

//             swCloud::SWVisualizeCloud l_oVisualizeCloud2(oCloudTemp,true);

        // set the projected mat
            oGrayImageResult = l_oProjectedMat;
    }

    /**
     * \brief  Save the radial projection to a 3D obj file
     * \param  [in] oRadialProj          : input radial mat image
     * \param  [in] sPath                : path of the obj to save
     * \param  [in] sBBox                : bbox of the total cloud
     * \param  [in] ui32WidthImage       : width of the result image
     * \param  [in] ui32HeightImage      : height of the result image
     * \param  [in] fCylinderRadius      : radius of the cylinder
     * \param  [in] oBBoxFaceCloud       : ...
     * \param  [in] vP3FTotalStasmPoints : ...
     */
    static bool saveRadialProjTo3DOBJ(const cv::Mat &oRadialProj, const std::string &sPath, const SWCloudBBox &sBBox,
                      cuint32 ui32WidthImage, cuint32 ui32HeightImage, cfloat fCylinderRadius, const swCloud::SWCloudBBox &oBBoxFaceCloud,
                      std::vector<cv::Point3f> &vP3FTotalStasmPoints)
    {
        // init the flow
        std::ofstream l_oFlow(sPath.c_str());
        std::string l_sFaces = "";

        // initialization of the axe of the cylinder
        cv::Vec3f l_oAxePoint;
        l_oAxePoint[0] = 0.5f * (sBBox.m_fMinX + sBBox.m_fMaxX);
        l_oAxePoint[1] = sBBox.m_fMinY;
        l_oAxePoint[2] = sBBox.m_fMinZ + 0.1f;

        float l_fMaxHeight = (sBBox.m_fMaxY - sBBox.m_fMinY);

        // vector containing the vertex coordinates
        std::vector<float> l_vFX, l_vFY, l_vFZ;

        uint l_ui32CurrVertex = 0;

        // mat containing index of the vertex
        cv::Mat l_oIdVertex 	 = oRadialProj.clone();
        // mat containing the depth of the vertex
        cv::Mat l_oIdVertexDepth = oRadialProj.clone();


        if(l_oFlow)
        {
            l_oFlow << "# Swooz : face " << endl;
            for(int ii = 0; ii < oRadialProj.rows; ++ii)
            {
                for(int jj = 0; jj < oRadialProj.cols; ++jj)
                {
                    if(oRadialProj.at<float>(ii,jj) > 0.f)
                    {
                        std::ostringstream l_osV1, l_osV2, l_osV3;
                        float l_fAlpha = 360.f * jj/(ui32WidthImage*1.f);
                        float l_fAngle,l_fDist, l_fX, l_fY, l_fZ;

                        int l_i32SensX, l_i32SensZ;

                        // localisation of the projection on the cylinder for computing the angle and the distance
                        if(l_fAlpha < 90.f)
                        {
                            l_fAngle = l_fAlpha;
                            l_i32SensX = 1;
                            l_i32SensZ = 1;
                        }
                        else if(l_fAlpha < 180.f)
                        {
                            l_fAngle = 180-l_fAlpha;
                            l_i32SensX = 1;
                            l_i32SensZ = -1;
                        }
                        else if(l_fAlpha < 270.f)
                        {
                            l_fAngle = l_fAlpha - 180;
                            l_i32SensX = -1;
                            l_i32SensZ = -1;
                        }
                        else
                        {
                            l_fAngle = 360 - l_fAlpha;
                            l_i32SensX = -1;
                            l_i32SensZ = 1;
                        }

                        l_fAngle /= (180.f/(float)M_PI);
                        l_fDist = (oRadialProj.at<float>(ii,jj)/255.f) * fCylinderRadius;

                        // update current vertex coordinates
                        l_fX = l_oAxePoint[0] + sin(l_fAngle) * l_fDist * l_i32SensX;
                        l_fY = (-ii + 1.f*ui32HeightImage)/(ui32HeightImage*1.f/l_fMaxHeight) + sBBox.m_fMinY;
                        l_fZ = l_oAxePoint[2] + cos(l_fAngle) * l_fDist * l_i32SensZ;


                        bool l_bInsideFaceBBox = oBBoxFaceCloud.isInside(l_fX, l_fY);

                        if(l_bInsideFaceBBox)
                        {

                            // add vertex coordinates in the arrays
                            l_vFX.push_back(l_fX);
                            l_vFY.push_back(l_fY);
                            l_vFZ.push_back(l_fZ);

                            // add vertex coordinates in the flow
                            l_osV1 << l_fX;
                            l_osV2 << l_fY;
                            l_osV3 << l_fZ;
                            l_oFlow << "v " + l_osV1.str() + " " +  l_osV2.str() + " " +  l_osV3.str() << endl;

                            // update index of the current vertex
                            l_ui32CurrVertex++;

                            // update mat containing the index and the depth
                            l_oIdVertex.at<int>(ii,jj) 	  = l_ui32CurrVertex;
                            l_oIdVertexDepth.at<float>(ii,jj) = l_fZ;
                        }
                        else
                        {
                            l_oIdVertex.at<int>(ii,jj) 	 = 0;
                            l_oIdVertexDepth.at<float>(ii,jj)= 0;
                        }
                    }
                    else
                    {
                        // if there is no vertex on the current location
                        l_oIdVertex.at<int>(ii,jj) 	 = 0;
                        l_oIdVertexDepth.at<float>(ii,jj)= 0;
                    }
                }
            }
        }
        else
        {
            std::cerr << "Error writing obj file : saveRadialProjTo3DOBJ " << std::endl;
            return false;
        }

        // add texture coordinates
        addTextureCoordRadialProj(l_oFlow, l_vFX, l_vFY, oBBoxFaceCloud);

        // add normals in the obj files
        addNormalsRadialProj(l_oFlow, l_oIdVertex, l_vFX, l_vFY, l_vFZ);

        // add faces in the obj files
        addFacesRadialProj(l_oFlow, l_oIdVertex);


        // temp -> display cloud
//        swCloud::SWCloud l_oCloudToDisplay(l_vFX, l_vFY, l_vFZ);
//        l_oCloudToDisplay.setUnicolor(255,0,0);

//        l_oCloudToDisplay.bBox2DFilter(oBBoxFaceCloud);

//        SWCloud bboxOfTheCloud1, bboxOfTheCloud2;
//        swCloud::SWCloud::cloudBBox(sBBox,	   bboxOfTheCloud1,1000);
//        swCloud::SWCloud::cloudBBox(oBBoxFaceCloud,bboxOfTheCloud2,1000);

//        bboxOfTheCloud1.setUnicolor(0,255,0);
//        bboxOfTheCloud2.setUnicolor(0,0,255);
//        l_oCloudToDisplay += bboxOfTheCloud1;
//        l_oCloudToDisplay += bboxOfTheCloud2;

//        swCloud::SWVisualizeCloud l_oVisualizeCloud2(l_oCloudToDisplay,true);




        // stasm
        // cv::Mat l_oStasmCloudMat(vP3FTotalStasmPoints.size(), 1, CV_32FC3);

        // for(uint ii = 0; ii < vP3FTotalStasmPoints.size(); ++ii)
        // {
            // l_oStasmCloudMat.at<cv::Point3f>(ii) = vP3FTotalStasmPoints[ii];
        // }

        // SWCloud l_oStasmsPointCloud;
        // swCloud::swUtil::convCloudMat2SWCloud(l_oStasmCloudMat, l_oStasmsPointCloud, 0.f, 10.f, 0, 255, 0);
        // l_oCloudToDisplay += l_oStasmsPointCloud;

        return true;
    }



//    static void erodeRadialProjc(cv::Mat &oRadialProj, cint i32Iteration, cv::Mat oKernel = cv::Mat(), )
//    {

//    }


    /**
     * \brief  Apply a secure dilatation and erosion on the radial proj (management of the edge)
     * \param  [in,out] oRadialProj   : the radial projection to dilate/erode
     * \param  [in] i32Dilatation     : number of dilatations
     * \param  [in] i32Erosion        : number of erosions
     */
    static void dilateErodeRadialProj(cv::Mat &oRadialProj, cint i32Dilatation, cint i32Erosion)
    {
        if(i32Dilatation < 3 || i32Erosion < 3)
        {
            return;
        }

        // init expanded mat
        cv::Mat l_oExpandedMat(oRadialProj.rows + 2*i32Dilatation, oRadialProj.cols + 2*i32Dilatation, CV_32FC1);

        // set the mat to 0
        cv::Scalar l_oScalInit;
        l_oScalInit[0] = 0.f;
        l_oExpandedMat = l_oScalInit;

        uint l_ui32Offset = i32Dilatation + 2;

        for(int ii = 0; ii < oRadialProj.rows; ++ii)
        {
            for(int jj = 0; jj < oRadialProj.cols; ++jj)
            {
                l_oExpandedMat.at<float>(ii+l_ui32Offset,jj+l_ui32Offset) = oRadialProj.at<float>(ii,jj);
            }
        }

        if(i32Dilatation > 0)
        {
            cv::dilate(l_oExpandedMat, l_oExpandedMat, cv::Mat(), cv::Point(-1, -1), i32Dilatation);
        }
        if(i32Erosion > 0)
        {
            cv::erode(l_oExpandedMat,  l_oExpandedMat, cv::Mat(), cv::Point(-1, -1), i32Erosion);
        }

        for(int ii = 0; ii < oRadialProj.rows; ++ii)
        {
            for(int jj = 0; jj < oRadialProj.cols; ++jj)
            {
                 oRadialProj.at<float>(ii,jj) = l_oExpandedMat.at<float>(ii+l_ui32Offset,jj+l_ui32Offset);
            }
        }
    }

    /**
     * \brief  Erase the contours of the radial projection image
     * \param  [in,out] oRadialProj    : radial projection which contours will be erased
     * \param  [in]     i32Erase       : number of iterations
     * \param  [in]     i32MinConnex   : minimum connexe value for deletion
     * \param  [in]     bConnex8       : activate 8-neighbours connexity check
     */
    static void eraseContoursRadialProj(cv::Mat &oRadialProj, cint i32Erase, cint i32MinConnex = 0, cbool bConnex8 = false)
    {
        for(int kk = 0; kk < i32Erase; ++kk)
        {
            cv::Mat l_oMask = oRadialProj.clone();
            for(int ii = 1; ii < oRadialProj.rows-1; ++ii)
            {
                for(int jj = 1; jj < oRadialProj.cols-1; ++jj)
                {
                    int l_i32Number = 0;

                    if(l_oMask.at<float>(ii,jj-1) == 0.f)
                    {
                        ++l_i32Number;
                    }
                    if(l_oMask.at<float>(ii-1,jj) == 0.f)
                    {
                        ++l_i32Number;
                    }

                    if(l_oMask.at<float>(ii+1,jj) == 0.f)
                    {
                        ++l_i32Number;
                    }
                    if(l_oMask.at<float>(ii,jj+1) == 0.f)
                    {
                        ++l_i32Number;
                    }

                    if(bConnex8)
                    {
                        if(l_oMask.at<float>(ii+1,jj-1) == 0.f)
                        {
                            ++l_i32Number;
                        }
                        if(l_oMask.at<float>(ii-1,jj+1) == 0.f)
                        {
                            ++l_i32Number;
                        }
                        if(l_oMask.at<float>(ii+1,jj+1) == 0.f)
                        {
                            ++l_i32Number;
                        }
                        if(l_oMask.at<float>(ii-1,jj-1) == 0.f)
                        {
                            ++l_i32Number;
                        }
                    }

                    if(l_i32Number >= i32MinConnex)
                    {
                        oRadialProj.at<float>(ii,jj) = 0;
                    }
                }
            }
        }
    }

    /**
     * @brief Expand the contours of the radial projection image
     * @param [in,out] oRadialProj : radial projection which contours will be expanded
     * @param [in] i32Expansion    : number of iterations
     * @param [in] i32MinConnex    : minimum connexe value for expansion
     * @param [in] bConnex8        : activate 8-neighbours connexity check
     */
    static void expandContoursRadialProj(cv::Mat &oRadialProj, cint i32Expansion, cint i32MinConnex = 0, cbool bConnex8 = false)
    {
        for(int kk = 0; kk < i32Expansion; ++kk)
        {
            cv::Mat l_oMask = oRadialProj.clone();
            for(int ii = 1; ii < l_oMask.rows-1; ++ii)
            {
                for(int jj = 1; jj < l_oMask.cols-1; ++jj)
                {
                    if(l_oMask.at<float>(ii,jj) == 0.f)
                    {
                        int l_i32ConnexNb = 0;
                        float l_fTotalValue = 0.f;

                        if(l_oMask.at<float>(ii,jj-1) > 0.f)
                        {
                            l_fTotalValue += l_oMask.at<float>(ii,jj-1);
                            ++l_i32ConnexNb;
                        }
                        if(l_oMask.at<float>(ii-1,jj) > 0.f)
                        {
                            l_fTotalValue += l_oMask.at<float>(ii-1,jj);
                            ++l_i32ConnexNb;
                        }

                        if(l_oMask.at<float>(ii+1,jj) > 0.f)
                        {
                            l_fTotalValue += l_oMask.at<float>(ii+1,jj);
                            ++l_i32ConnexNb;
                        }
                        if(l_oMask.at<float>(ii,jj+1) > 0.f)
                        {
                            l_fTotalValue += l_oMask.at<float>(ii,jj+1);
                            ++l_i32ConnexNb;
                        }

                        if(bConnex8)
                        {
                            if(l_oMask.at<float>(ii+1,jj+1) > 0.f)
                            {
                                l_fTotalValue += l_oMask.at<float>(ii+1,jj+1);
                                ++l_i32ConnexNb;
                            }
                            if(l_oMask.at<float>(ii+1,jj-1) > 0.f)
                            {
                                l_fTotalValue += l_oMask.at<float>(ii+1,jj-1);
                                ++l_i32ConnexNb;
                            }
                            if(l_oMask.at<float>(ii-1,jj+1) > 0.f)
                            {
                                l_fTotalValue += l_oMask.at<float>(ii-1,jj+1);
                                ++l_i32ConnexNb;
                            }
                            if(l_oMask.at<float>(ii-1,jj-1) > 0.f)
                            {
                                l_fTotalValue += l_oMask.at<float>(ii-1,jj-1);
                                ++l_i32ConnexNb;
                            }
                        }

                        if(l_i32ConnexNb > i32MinConnex)
                        {
                            oRadialProj.at<float>(ii,jj) = l_fTotalValue/l_i32ConnexNb;
                        }
                    }
                }
            }
        }
    }


    /**
     * \brief ...
     * \param  [in] oFX	 	: x coordinates array of the obj vertex
     * \param  [in] oFY		: y coordinates array of the obj vertex
     * \param  [in] sBBoxFaceCloud  : ...
     */
    static void retrieveTextureCoordFromRadialProj(const std::vector<float> &vFX, const std::vector<float> &vFY, const SWCloudBBox &sBBoxFaceCloud,
                                                   std::vector<std::vector<float> > &v2FTextureCoord)
    {
        v2FTextureCoord.clear();

        for(uint ii = 0; ii < vFX.size(); ++ii)
        {
            // compute relative texture coordinate
            std::vector<float> l_vCoord2D(2);
            l_vCoord2D[0] = (vFX[ii] - sBBoxFaceCloud.m_fMinX )/check0Div(sBBoxFaceCloud.m_fMaxX - sBBoxFaceCloud.m_fMinX);
            l_vCoord2D[1] = (vFY[ii] - sBBoxFaceCloud.m_fMinY )/check0Div(sBBoxFaceCloud.m_fMaxY - sBBoxFaceCloud.m_fMinY);
            v2FTextureCoord.push_back(l_vCoord2D);
        }
    }

    /**
     * \brief ...
     * \param  [in]  oIndexMask  : input vertex index mat
     * \param  [out] v3FFacesId : ...
     */
//    static void retrieveFacesFromRadialProj(const cv::Mat &oIndexMask, std::vector<std::vector<uint> > &v3UIFacesId)
//    {
//        v3UIFacesId.clear();

//        for(int ii = 1; ii < oIndexMask.rows; ++ii)
//        {
//            for(int jj = 1; jj < oIndexMask.cols; ++jj)
//            {
//                if(oIndexMask.at<int>(ii,jj) != 0)
//                {
//                    // .     . - .
//                    // | \     \ |
//                    // . - .     .
//                    if(oIndexMask.at<int>(ii-1,jj-1) != 0)
//                    {
//                        if(oIndexMask.at<int>(ii,jj-1) != 0)
//                        {
//                            std::vector<uint> l_vFace(3);
//                            l_vFace[0] = (uint)oIndexMask.at<int>(ii,jj);
//                            l_vFace[1] = (uint)oIndexMask.at<int>(ii-1,jj-1);
//                            l_vFace[2] = (uint)oIndexMask.at<int>(ii,jj-1);
//                            v3UIFacesId.push_back(l_vFace);
//                        }
//                        if(oIndexMask.at<float>(ii-1,jj) != 0)
//                        {
//                            std::vector<uint> l_vFace(3);
//                            l_vFace[0] = (uint)oIndexMask.at<int>(ii,jj);
//                            l_vFace[1] = (uint)oIndexMask.at<int>(ii-1,jj);
//                            l_vFace[2] = (uint)oIndexMask.at<int>(ii-1,jj-1);
//                            v3UIFacesId.push_back(l_vFace);
//                        }
//                    }
//                    //     .
//                    //   / |
//                    // . - .
//                    else
//                    {
//                        if(oIndexMask.at<int>(ii,jj-1) != 0 && oIndexMask.at<int>(ii-1,jj) != 0)
//                        {
//                            std::vector<uint> l_vFace(3);
//                            l_vFace[0] = (uint)oIndexMask.at<int>(ii,jj);
//                            l_vFace[1] = (uint)oIndexMask.at<int>(ii-1,jj);
//                            l_vFace[2] = (uint)oIndexMask.at<int>(ii,jj-1);
//                            v3UIFacesId.push_back(l_vFace);
//                        }
//                    }
//                }
//            }
//        }
//    }


    /**
     * @brief retrieveFacesFromRadialProj2
     * @param oIndexMask
     * @param oDepthVertex
     * @param v3UIFacesId
     */
    static void retrieveFacesFromRadialProj2(const cv::Mat &oIndexMask, const cv::Mat &oDepthVertex, std::vector<std::vector<uint> > &v3UIFacesId)
    {
        v3UIFacesId.clear();

        for(int ii = 0; ii < oIndexMask.rows-1; ++ii)
        {
            for(int jj = 0; jj < oIndexMask.cols-1; ++jj)
            {
                std::vector<uint> l_vFace1(3), l_vFace2(3);

                // 1 - 2
                // |   |
                // 3 - 4
                bool l_b1 = oIndexMask.at<int>(ii,jj)    != 0;
                bool l_b2 = oIndexMask.at<int>(ii,jj+1)  != 0;
                bool l_b3 = oIndexMask.at<int>(ii+1,jj)  != 0;
                bool l_b4 = oIndexMask.at<int>(ii+1,jj+1)!= 0;

                // . - .
                // |   |
                // . - .
                if(l_b1 && l_b2 && l_b3 && l_b4)
                {
                    float l_fDiff14 = oDepthVertex.at<float>(ii,jj) - oDepthVertex.at<float>(ii+1,jj+1);
                    l_fDiff14 *= l_fDiff14;

                    float l_fDiff23 = oDepthVertex.at<float>(ii,jj+1) - oDepthVertex.at<float>(ii+1,jj);
                    l_fDiff23 *= l_fDiff23;

                    if(l_fDiff14 > l_fDiff23)
                    {
                        // 1 - 2
                        //   \ |
                        //     4
                        l_vFace1[0] = (uint)oIndexMask.at<int>(ii,jj);
                        l_vFace1[1] = (uint)oIndexMask.at<int>(ii+1,jj+1);
                        l_vFace1[2] = (uint)oIndexMask.at<int>(ii,jj+1);
                        v3UIFacesId.push_back(l_vFace1);

                        // 1
                        // | \
                        // 3 - 4
                        l_vFace2[0] = (uint)oIndexMask.at<int>(ii,jj);
                        l_vFace2[1] = (uint)oIndexMask.at<int>(ii+1,jj);
                        l_vFace2[2] = (uint)oIndexMask.at<int>(ii+1,jj+1);
                        v3UIFacesId.push_back(l_vFace2);
                    }
                    else
                    {
                        // 1 - 2
                        // | /
                        // 3
                        l_vFace1[0] = (uint)oIndexMask.at<int>(ii,jj);
                        l_vFace1[1] = (uint)oIndexMask.at<int>(ii+1,jj);
                        l_vFace1[2] = (uint)oIndexMask.at<int>(ii,jj+1);
                        v3UIFacesId.push_back(l_vFace1);

                        //     2
                        //   / |
                        // 3 - 4
                        l_vFace2[0] = (uint)oIndexMask.at<int>(ii,jj+1);
                        l_vFace2[1] = (uint)oIndexMask.at<int>(ii+1,jj);
                        l_vFace2[2] = (uint)oIndexMask.at<int>(ii+1,jj+1);
                        v3UIFacesId.push_back(l_vFace2);
                    }
                }
                // . - .
                // |   |
                // x - .
                else if(l_b1 && l_b2 && l_b4)
                {
                    l_vFace1[0] = (uint)oIndexMask.at<int>(ii,jj);
                    l_vFace1[1] = (uint)oIndexMask.at<int>(ii+1,jj+1);
                    l_vFace1[2] = (uint)oIndexMask.at<int>(ii,jj+1);
                    v3UIFacesId.push_back(l_vFace1);
                }
                // . - .
                // |   |
                // . - x
                else if(l_b1 && l_b2 && l_b3)
                {
                    l_vFace1[0] = (uint)oIndexMask.at<int>(ii,jj);
                    l_vFace1[1] = (uint)oIndexMask.at<int>(ii+1,jj);
                    l_vFace1[2] = (uint)oIndexMask.at<int>(ii,jj+1);

                    v3UIFacesId.push_back(l_vFace1);
                }
                // . - x
                // |   |
                // . - .
                else if(l_b1 && l_b3 && l_b4)
                {
                    l_vFace1[0] = (uint)oIndexMask.at<int>(ii,jj);
                    l_vFace1[1] = (uint)oIndexMask.at<int>(ii+1,jj);
                    l_vFace1[2] = (uint)oIndexMask.at<int>(ii+1,jj+1);
                    v3UIFacesId.push_back(l_vFace1);
                }
                // x - .
                // |   |
                // . - .
                else if(l_b2 && l_b3 && l_b4)
                {
                    l_vFace1[0] = (uint)oIndexMask.at<int>(ii,jj+1);
                    l_vFace1[1] = (uint)oIndexMask.at<int>(ii+1,jj);
                    l_vFace1[2] = (uint)oIndexMask.at<int>(ii+1,jj+1);
                    v3UIFacesId.push_back(l_vFace1);
                }
            }
        }
    }

    /**
     * \brief  ...
     * \param  [in] oRadialProj          : input radial mat image
     * \param  [out] oResultMesh         : result mesh
     * \param  [in] sTotalCloudBBox      : bbox of the total cloud
     * \param  [in] oBBoxFaceCloud       : bbox corresponding to the projection of the texture on the cloud
     * \param  [in] ui32WidthImage       : width of the result image
     * \param  [in] ui32HeightImage      : height of the result image
     * \param  [in] fCylinderRadius      : radius of the cylinder
     */
    static bool transformRadialProjToMesh(
                    const cv::Mat &oRadialProj, swMesh::SWMesh &oResultMesh,
                    const SWCloudBBox &oTotalCloudBBox, const swCloud::SWCloudBBox &oBBoxFaceCloud,
                    cuint32 ui32WidthImage, cuint32 ui32HeightImage, cfloat fCylinderRadius)
    {
        // initialization of the axe of the cylinder
            cv::Vec3f l_oAxePoint;
            l_oAxePoint[0] = 0.5f * (oTotalCloudBBox.m_fMinX + oTotalCloudBBox.m_fMaxX);
            l_oAxePoint[1] = oTotalCloudBBox.m_fMinY;
            l_oAxePoint[2] = oTotalCloudBBox.m_fMinZ + 0.1f;

            float l_fMaxHeight = (oTotalCloudBBox.m_fMaxY - oTotalCloudBBox.m_fMinY);

        // vector containing the vertex coordinates
            std::vector<float> l_vFX, l_vFY, l_vFZ;

        // mat containing index of the vertex
            cv::Mat l_oIdVertex 	 = oRadialProj.clone();
        // mat containing the depth of the vertex
            cv::Mat l_oIdVertexDepth = oRadialProj.clone();

        uint l_ui32CurrVertex = 0;
        for(int ii = 0; ii < oRadialProj.rows; ++ii)
        {
            for(int jj = 0; jj < oRadialProj.cols; ++jj)
            {
                if(oRadialProj.at<float>(ii,jj) > 0.f)
                {
                    float l_fAlpha = 360.f * jj/(ui32WidthImage*1.f);
                    float l_fAngle,l_fDist, l_fX, l_fY, l_fZ;

                    int l_i32SensX, l_i32SensZ;

                    // localisation of the projection on the cylinder for computing the angle and the distance
                        if(l_fAlpha < 90.f)
                        {
                            l_fAngle = l_fAlpha;
                            l_i32SensX = 1;
                            l_i32SensZ = 1;
                        }
                        else if(l_fAlpha < 180.f)
                        {
                            l_fAngle = 180-l_fAlpha;
                            l_i32SensX = 1;
                            l_i32SensZ = -1;
                        }
                        else if(l_fAlpha < 270.f)
                        {
                            l_fAngle = l_fAlpha - 180;
                            l_i32SensX = -1;
                            l_i32SensZ = -1;
                        }
                        else
                        {
                            l_fAngle = 360 - l_fAlpha;
                            l_i32SensX = -1;
                            l_i32SensZ = 1;
                        }

                        l_fAngle /= (180.f/(float)M_PI);
                        l_fDist = (oRadialProj.at<float>(ii,jj)/255.f) * fCylinderRadius;

                    // update current vertex coordinates
                        l_fX = l_oAxePoint[0] + sin(l_fAngle) * l_fDist * l_i32SensX;
                        l_fY = (-ii + 1.f*ui32HeightImage)/(ui32HeightImage*1.f/l_fMaxHeight) + oTotalCloudBBox.m_fMinY;
                        l_fZ = l_oAxePoint[2] + cos(l_fAngle) * l_fDist * l_i32SensZ;


                    bool l_bInsideFaceBBox = oBBoxFaceCloud.isInside(l_fX, l_fY);

                    if(l_bInsideFaceBBox)
                    {
                        // add vertex coordinates in the arrays
                            l_vFX.push_back(l_fX);
                            l_vFY.push_back(l_fY);
                            l_vFZ.push_back(l_fZ);

                        // update index of the current vertex
                            l_ui32CurrVertex++;

                        // update mat containing the index and the depth
                            l_oIdVertex.at<int>(ii,jj) 	  = l_ui32CurrVertex;
                            l_oIdVertexDepth.at<float>(ii,jj) = l_fZ;
                    }
                    else
                    {
                        l_oIdVertex.at<int>(ii,jj) 	 = 0;
                        l_oIdVertexDepth.at<float>(ii,jj)= 0;
                    }
                }
                else
                {
                    // if there is no vertex on the current location
                        l_oIdVertex.at<int>(ii,jj) 	 = 0;
                        l_oIdVertexDepth.at<float>(ii,jj)= 0;
                }
            }
        }

        // retrive vertex coordinates
            std::vector<std::vector<float> > l_vVertexCoords;
            for(uint ii = 0; ii < l_vFX.size(); ++ii)
            {
                std::vector<float> l_vP(3);
                l_vP[0] = l_vFX[ii];
                l_vP[1] = l_vFY[ii];
                l_vP[2] = l_vFZ[ii];
                l_vVertexCoords.push_back(l_vP);
            }

        // retrieve faces index
            std::vector<std::vector<uint> > l_vIndexTri;
            retrieveFacesFromRadialProj2(l_oIdVertex, l_oIdVertexDepth, l_vIndexTri);

        // retrieve texture coordinates
            std::vector<std::vector<float> > l_vTextureCoords;
            retrieveTextureCoordFromRadialProj(l_vFX, l_vFY, oBBoxFaceCloud, l_vTextureCoords);

        // set the new mesh
            oResultMesh = swMesh::SWMesh(l_vVertexCoords, l_vIndexTri, l_vTextureCoords);


            swCloud::SWRigidMotion rigidMotion(0.f,180.f,0.f);
            oResultMesh.cloud()->transform(rigidMotion.m_aFRotation, rigidMotion.m_aFTranslation);

            std::vector<float> l_offsetToApply(3,0.f);

            float l_zMax = -FLT_MAX;
            int l_idZMax = 0;
            for(uint ii = 0; ii < oResultMesh.pointsNumber(); ++ii)
            {
                if(oResultMesh.cloud()->coord(2)[ii] > l_zMax)
                {
                    l_zMax = oResultMesh.cloud()->coord(2)[ii];
                    l_idZMax = ii;
                }
            }

            l_offsetToApply[0] = -oResultMesh.cloud()->coord(0)[l_idZMax];
            l_offsetToApply[1] = -oResultMesh.cloud()->coord(1)[l_idZMax];
            l_offsetToApply[2] = -oResultMesh.cloud()->coord(2)[l_idZMax];
            (*oResultMesh.cloud()) += l_offsetToApply;

            oResultMesh.updateNonOrientedTrianglesNormals();
            oResultMesh.updateNonOrientedVerticesNormals();


        return true;
    }


    /**
     * @brief checkGerm4Connex
     * @param i32I
     * @param i32J
     * @param oRadialProj
     * @param oMask
     */
    static void checkGerm4Connex(cint i32I, cint i32J, cv::Mat &oRadialProj, cv::Mat &oMask, cfloat fMaxDiffValue)
    {
        //    1
        //    |
        //2 - . - 3
        //    |
        //    4
        if( oRadialProj.at<float>(i32I, i32J) <= 0.f)
        {
            std::cerr << "Error : checkGerm4Connex -> bad coordinate value for the germ. " << std::endl;
            return;
        }

        std::stack<std::vector<int> > mystack;

        std::vector<int> l_vPosInit(2);
        l_vPosInit[0] = i32I;
        l_vPosInit[1] = i32J;
        mystack.push(l_vPosInit);

        while(mystack.size() > 0)
        {
            std::vector<int> l_vCurrPos = mystack.top();
            mystack.pop();

            int l_i32PI = l_vCurrPos[0];
            int l_i32PJ = l_vCurrPos[1];

            oMask.at<float>(l_i32PI, l_i32PJ) = 1.f;

            bool l_b1 = (oMask.at<float>(l_i32PI-1, l_i32PJ)   < 1.f) && (l_i32PI-1 > 0);// && oRadialProj.at<float>(oPt.x+1, oPt.y) > 0.f
            bool l_b2 = (oMask.at<float>(l_i32PI,   l_i32PJ-1) < 1.f) && (l_i32PJ-1 > 0);
            bool l_b3 = (oMask.at<float>(l_i32PI,   l_i32PJ+1) < 1.f) && (l_i32PJ+1 < oMask.cols);
            bool l_b4 = (oMask.at<float>(l_i32PI+1, l_i32PJ)   < 1.f) && (l_i32PI+1 < oMask.rows);

            bool l_bM1 = (oRadialProj.at<float>(l_i32PI-1, l_i32PJ)   > 0.f);
            bool l_bM2 = (oRadialProj.at<float>(l_i32PI,   l_i32PJ-1) > 0.f);
            bool l_bM3 = (oRadialProj.at<float>(l_i32PI,   l_i32PJ+1) > 0.f);
            bool l_bM4 = (oRadialProj.at<float>(l_i32PI+1, l_i32PJ)   > 0.f);

            float l_fV1 = (oRadialProj.at<float>(l_i32PI-1, l_i32PJ)  - oRadialProj.at<float>(l_i32PI, l_i32PJ));
            float l_fV2 = (oRadialProj.at<float>(l_i32PI, l_i32PJ-1)  - oRadialProj.at<float>(l_i32PI, l_i32PJ));
            float l_fV3 = (oRadialProj.at<float>(l_i32PI, l_i32PJ+1)  - oRadialProj.at<float>(l_i32PI, l_i32PJ));
            float l_fV4 = (oRadialProj.at<float>(l_i32PI+1, l_i32PJ)  - oRadialProj.at<float>(l_i32PI, l_i32PJ));
            if(l_fV1 < 0)
                l_fV1 = -l_fV1;
            if(l_fV2 < 0)
                l_fV2 = -l_fV2;
            if(l_fV3 < 0)
                l_fV3 = -l_fV3;
            if(l_fV4 < 0)
                l_fV4 = -l_fV4;

            bool l_Diff1 = l_fV1 < fMaxDiffValue;
            bool l_Diff2 = l_fV2 < fMaxDiffValue;
            bool l_Diff3 = l_fV3 < fMaxDiffValue;
            bool l_Diff4 = l_fV4 < fMaxDiffValue;


            std::vector<int> l_vNewPos(2);
            if(l_b1 && l_bM1 && l_Diff1)
            {
                l_vNewPos[0] = l_i32PI-1;
                l_vNewPos[1] = l_i32PJ;
                mystack.push(l_vNewPos);
            }
            if(l_b2 && l_bM2 && l_Diff2)
            {
                l_vNewPos[0] = l_i32PI;
                l_vNewPos[1] = l_i32PJ-1;
                mystack.push(l_vNewPos);
            }
            if(l_b3 && l_bM3 && l_Diff3)
            {
                l_vNewPos[0] = l_i32PI;
                l_vNewPos[1] = l_i32PJ+1;
                mystack.push(l_vNewPos);
            }
            if(l_b4 && l_bM4 && l_Diff4)
            {
                l_vNewPos[0] = l_i32PI+1;
                l_vNewPos[1] = l_i32PJ;
                mystack.push(l_vNewPos);
            }
        }
    }

    /**
     * @brief keepBiggestConnexAggregate
     * @param oRadialProj
     * @return
     */
    static bool keepBiggestConnexAggregate(cv::Mat &oRadialProj, cfloat fMaxDiffValue = 1000.f)
    {
        cv::Mat l_oMask = oRadialProj.clone();
        l_oMask.setTo(0.f);
        checkGerm4Connex(oRadialProj.rows/2, oRadialProj.cols/2, oRadialProj, l_oMask, fMaxDiffValue);

        for(int ii = 0; ii < oRadialProj.rows * oRadialProj.cols; ++ii)
        {
            if(l_oMask.at<float>(ii) < 1.f)
            {
                oRadialProj.at<float>(ii) = 0.f;
            }
        }

        return true;
    }


}

#endif
