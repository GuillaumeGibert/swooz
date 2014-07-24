
/**
 * \file SWOptimalStepNonRigidICP.cpp
 * \brief ...
 * \author Florian Lance
 * \date 19/09/13
 */



#include <iostream>
#include <fstream>

// UTILITY
#include <time.h>

// SWOOZ
#include "mesh/SWOptimalStepNonRigidICP.h"
#include "cloud/SWAlignClouds.h"
#include "gpuMat/gpuMatUtility.h"
#include "geometryUtility.h"

// OPENCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"



using namespace swMesh;
using namespace std;

SWOptimalStepNonRigidICP::SWOptimalStepNonRigidICP(const SWMesh &oSource, const SWMesh &oTarget,
                                                   const string &sPathSourceStasmCorr, const string &sPathTargetStasmCorr):
                                                   m_oSourceMesh(oSource), m_oTargetMesh(oTarget), m_oOriginalTargetMesh(oTarget),
                                                   m_pSourceMeshC(NULL), m_pTargetMeshC(NULL)
{        
    vector<float> l_A3FTargetMeanPoint = m_oTargetMesh.cloud()->meanPoint();
    l_A3FTargetMeanPoint[2] -= 10.f;

    int l_i32IdCloseSource = m_oSourceMesh.cloud()->idNearestPoint(l_A3FTargetMeanPoint);
    int l_i32IdCloseTarget = m_oTargetMesh.cloud()->idNearestPoint(l_A3FTargetMeanPoint);

    vector<float> l_v3FOffset(3,0);
    l_v3FOffset[0] = - m_oSourceMesh.cloud()->coord(0)[l_i32IdCloseSource];
    l_v3FOffset[1] = - m_oSourceMesh.cloud()->coord(1)[l_i32IdCloseSource];
    l_v3FOffset[2] = - m_oSourceMesh.cloud()->coord(2)[l_i32IdCloseSource];

    (*m_oSourceMesh.cloud()) += l_v3FOffset;

    l_v3FOffset[0] = - m_oTargetMesh.cloud()->coord(0)[l_i32IdCloseTarget];
    l_v3FOffset[1] = - m_oTargetMesh.cloud()->coord(1)[l_i32IdCloseTarget];
    l_v3FOffset[2] = - m_oTargetMesh.cloud()->coord(2)[l_i32IdCloseTarget];

    (*m_oTargetMesh.cloud())         += l_v3FOffset;
    (*m_oOriginalTargetMesh.cloud()) += l_v3FOffset;

    // init deformation arrays
        m_X  = new cv::Mat(4 * oSource.pointsNumber(), 3, CV_32FC1, cv::Scalar(0.f));
        m_pX = new cv::Mat(4 * oSource.pointsNumber(), 3, CV_32FC1, cv::Scalar(0.f));

    // init distance weights array
        m_w.assign(oSource.pointsNumber(), 1.f);
        m_w1.assign(oSource.pointsNumber(), 1.f);
        m_w2.assign(oSource.pointsNumber(), 1.f);
        m_w3.assign(oSource.pointsNumber(), 1.f);

    // init correspondances array
        m_u.assign(oSource.pointsNumber(), 0);

    // set initial parameters
        m_fAngleMax = 50.f;
        m_fLastComputedCost = -1.f;
        m_fWeightVectorDistMax = 0.08f;

    // read stasm correspondance files
        // source
            vector<int> l_vI32CorrStasmSource;
            readStasmCorrFile(sPathSourceStasmCorr, l_vI32CorrStasmSource);
        //target
            vector<int> l_vI32CorrStasmTarget;
            readStasmCorrFile(sPathTargetStasmCorr, l_vI32CorrStasmTarget);

    // init landmarks
        for(uint ii = 0; ii < l_vI32CorrStasmSource.size(); ++ii)
        {
            if(l_vI32CorrStasmSource[ii] != -1 && l_vI32CorrStasmTarget[ii] != -1)
            {
                m_l[l_vI32CorrStasmSource[ii]] = l_vI32CorrStasmTarget[ii];
            }
        }

        // DEBUG
//        for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it)
//        {
//            uint l_ui32TemplateId = it->first;
//            uint l_ui32TargetId   = it->second;
//            cout << "m_l : " << l_ui32TemplateId << " " << l_ui32TargetId << endl;
//        }

//    clock_t l_oProgramTime = clock();


    //  update triangles normals
        m_oTargetMesh.updateNonOrientedTrianglesNormals();
        // DEBUG
//        cout << "Target normals1 : " << (float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC  << std::endl;
        m_oSourceMesh.updateNonOrientedTrianglesNormals();
        // DEBUG
//        cout << "Source normals1 : " << (float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC  << std::endl;

    // update vertices normals
        m_oTargetMesh.updateNonOrientedVerticesNormals();
        // DEBUG
//        cout << "Target normals2 : " << (float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC  << std::endl;
        m_oSourceMesh.updateNonOrientedVerticesNormals();
        // DEBUG
//        cout << "Source normals2 : " << (float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC  << std::endl;
}


SWOptimalStepNonRigidICP::~SWOptimalStepNonRigidICP()
{
    deleteAndNullify(m_X);
    deleteAndNullify(m_pX);

    deleteAndNullify(m_pSourceMeshC);
    deleteAndNullify(m_pTargetMeshC);
}


//void SWOptimalStepNonRigidICP::initResolve()
//{
//    clock_t m_oProgramTime;

//    // update triangles normals
//        m_oProgramTime = clock(); m_oSourceMesh.updateNonOrientedTrianglesNormals(); cout << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " ";
//    // update vertices normals
//        m_oProgramTime = clock(); m_oSourceMesh.updateNonOrientedVerticesNormals();


//    // update correspondances between source and target meshes
//        m_oProgramTime = clock(); computeCorrespondences(); cout << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " ";

//    // update weights
//        m_oProgramTime = clock(); computeDistanceWeights(); cout << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  <<  " -> ";
//}


void SWOptimalStepNonRigidICP::updateSourceMeshNormals()
{
    m_oSourceMesh.updateNonOrientedTrianglesNormals();
    m_oSourceMesh.updateNonOrientedVerticesNormals();
}

void SWOptimalStepNonRigidICP::computeCorrespondences()
{
    // DEBUG
//    clock_t l_oProgramTime = clock();

    m_fMaxTemplateTargetDistance = 0.f;

    for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
    {
        m_u[ii] = m_oSourceMesh.idNearestPoint(ii, m_oTargetMesh);

        std::vector<float> l_vPtTemplate, l_vPtTarget;
        m_oSourceMesh.point(l_vPtTemplate, ii);
        m_oTargetMesh.point(l_vPtTarget, m_u[ii]);

        float l_fDist = swUtil::norm(swUtil::vec(l_vPtTemplate, l_vPtTarget));

        if(l_fDist > m_fMaxTemplateTargetDistance)
        {
            m_fMaxTemplateTargetDistance = l_fDist;
        }
    }

    // DEBUG
//    cout << " end  computeCorrespondences " << (float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC << std::endl;
}



void SWOptimalStepNonRigidICP::computeDistanceWeights()
{
    // DEBUG
    clock_t m_oProgramTime = clock();

    // set to 1 all the weights
        m_w.assign(m_oSourceMesh.pointsNumber(), 1.f);
        m_w1.assign(m_oSourceMesh.pointsNumber(), 1.f);
        m_w2.assign(m_oSourceMesh.pointsNumber(), 1.f);
        m_w3.assign(m_oSourceMesh.pointsNumber(), 1.f);

    // 1) ui lies on a border of the target mesh
        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
            if(m_oTargetMesh.vertexOnBorder(m_u[ii]))
            {
                m_w[ii] = 0.f;
                m_w1[ii] = 0.f;
            }
        }

        // DEBUG
//        cout << " [1 " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << " ] ";
        m_oProgramTime = clock();

    // 2) the angle between the normals of the meshes at Xivi and ui is larger than a fixed threshold
        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
            if(m_w[ii] == 0.f)
            {
                continue;
            }

            vector<float> l_vXiViNormal;
            m_oSourceMesh.vertexNormal(l_vXiViNormal , ii);

            vector<float> l_vUiNormal;
            m_oTargetMesh.vertexNormal(l_vUiNormal, m_u[ii]);

            float l_fAngle = static_cast<float>(swUtil::vectorAngle(l_vXiViNormal, l_vUiNormal));

            if(l_fAngle > m_fAngleMax)
            {
                m_w[ii] = 0.f;
                m_w2[ii] = 0.f;
            }
        }

        // DEBUG
//        cout << " [2 " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << " ] ";
        m_oProgramTime = clock();

    // 3) the line segment Xivi to ui intersects the deformed template
        std::vector<float> l_vV1, l_vV2, l_vV3;
        std::vector<float> l_vP, l_vD;
        std::vector<float> l_vTriMiddle(3,0.f);
        std::vector<float> l_vVec1, l_vVec2;
        float l_fSquareWeightVectorDistMax = m_fWeightVectorDistMax*m_fWeightVectorDistMax;

        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
            if(m_w[ii] == 0.f)
            {
                continue;
            }

            m_oSourceMesh.point(l_vP, ii);
            m_oTargetMesh.point(l_vD, m_u[ii]);

            bool l_bIntersect = false;

            for(uint jj = 0; jj < m_oSourceMesh.trianglesNumber(); ++jj)
            {
                m_oSourceMesh.trianglePoints(l_vV1, l_vV2, l_vV3, jj);


                l_vTriMiddle[0] = (l_vV1[0] + l_vV2[0] + l_vV3[0])/3.f;
                l_vTriMiddle[1] = (l_vV1[1] + l_vV2[1] + l_vV3[1])/3.f;
                l_vTriMiddle[2] = (l_vV1[2] + l_vV2[2] + l_vV3[2])/3.f;

                swUtil::vec(l_vP, l_vTriMiddle, l_vVec1);
                swUtil::vec(l_vTriMiddle, l_vD, l_vVec2);

                if(swUtil::squareLength(l_vVec1) > l_fSquareWeightVectorDistMax && swUtil::squareLength(l_vVec2) > l_fSquareWeightVectorDistMax)
//                if(swUtil::norm(l_vVec1) > m_fWeightVectorDistMax && swUtil::norm(l_vVec2) > m_fWeightVectorDistMax)
//                if(swUtil::norm(swUtil::vec(l_vP,l_vTriMiddle)) > m_fWeightVectorDistMax && swUtil::norm(swUtil::vec(l_vTriMiddle,l_vD)) > m_fWeightVectorDistMax)
                {
                    continue;
                }

                if(swUtil::segmentTriangleIntersect(l_vP, l_vD, l_vV1, l_vV2, l_vV3) == 1)
                {
                    l_bIntersect = true;
                    break;
                }
            }

            if(l_bIntersect)
            {
                cout << "a";
                m_w[ii]  = 0.f;
                m_w3[ii] = 0.f;
            }

        }

        // DEBUG
        cout << " [3 " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << " ] ";
}

float SWOptimalStepNonRigidICP::totalEnergy() const
{
    return m_fLastComputedCost;
}

void SWOptimalStepNonRigidICP::displayDenseMatrix(cv::Mat &oMat)
{
    for(int ii = 0; ii < oMat.rows; ++ii)
    {
        for(int jj = 0; jj < oMat.cols; ++jj)
        {
            if(oMat.at<float>(ii,jj) != 0.0)
            {
                cout << oMat.at<float>(ii,jj) << " ";
            }
        }
    }
    cout << endl;
}


float SWOptimalStepNonRigidICP::computeDiff(cv::Mat &newX)
{
    cv::Mat l_oDiff(newX.rows, newX.cols, CV_32FC1);

    // update m_X and m_pX and compute the normed difference
        for(int ii = 0; ii < newX.rows; ++ii)
        {
            for(int jj = 0; jj < newX.cols; ++jj)
            {
                m_pX->at<float>(ii,jj) = m_X->at<float>(ii,jj);
                m_X->at<float>(ii,jj) = newX.at<float>(ii,jj);
                l_oDiff.at<float>(ii,jj) = m_X->at<float>(ii,jj) - m_pX->at<float>(ii,jj);
            }
        }

    return static_cast<float>(cv::norm(l_oDiff));
}

void SWOptimalStepNonRigidICP::copyDataForResolving()
{
    // copy meshes
        deleteAndNullify(m_pSourceMeshC);
        deleteAndNullify(m_pTargetMeshC);
        m_pSourceMeshC = new SWMesh(m_oSourceMesh);
        m_pTargetMeshC = new SWMesh(m_oTargetMesh);

    // copy correspondances arrays
        m_wC = m_w;
        m_uC = m_u;
}

float SWOptimalStepNonRigidICP::resolve(cfloat fAlpha, cfloat fBeta, cfloat fGama, cbool bUseLandMarks)
{
    clock_t m_oProgramTime;

    cv::Mat MG_A, WD, B, TAA, TAB, TAAInv, newX;

    // #### MG
    m_oProgramTime = clock();
    buildMG(MG_A, fAlpha, fGama);
//    cout << " MG " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;

    // #### WD
    m_oProgramTime = clock();
    buildWD(WD);
//    cout << " WD " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;

    // #### A
    m_oProgramTime = clock();
    int l_i32MGRow = MG_A.rows;
    buildA(MG_A,WD);//,A);
    WD.release();
//    cout << " A " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;

    // #### B
    m_oProgramTime = clock();
    buildB(l_i32MGRow,B);
//    cout << " B " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;

    // #### landmarks
    if(bUseLandMarks)
    {
        m_oProgramTime = clock();
        addLandMarks(fBeta, l_i32MGRow, MG_A, B);
    }

    // #### TAB
    m_oProgramTime = clock();
    buildTAB(MG_A, B, TAB);
//    cout << " TAB " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;
    B.release();

    // #### TAA
    m_oProgramTime = clock();
    buildTAA(MG_A, TAA);
//    cout << " TAA " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;
    MG_A.release();

    // #### TAAInv
    m_oProgramTime = clock();
    swUtil::swCuda::matrixInversion(TAA, TAAInv);
//    cout << " TAAInv " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;
    TAA.release();

//    // #### newX
    m_oProgramTime = clock();
    swUtil::swCuda::matrixMultiplication(TAAInv, TAB, newX);
//    cout << " newX " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << std::endl;
    TAAInv.release();
    TAB.release();

    m_oProgramTime = clock();
    float l_fDiff = computeDiff(newX);

    // apply deformation to the source cloud)
        swCloud::SWCloud *l_oSourceCloud = m_pSourceMeshC->cloud();
        swCloud::SWCloud *l_oTargetCloud = m_pTargetMeshC->cloud();

        for(uint ii = 0; ii < l_oSourceCloud->size(); ++ii)
        {
            std::vector<float> l_vPt(3,0.f);
            l_vPt[0] = l_oSourceCloud->coord(0)[ii];
            l_vPt[1] = l_oSourceCloud->coord(1)[ii];
            l_vPt[2] = l_oSourceCloud->coord(2)[ii];


            float l_fCoeffReduc = 1.f;

            // test in order to limit back head deformation
            {
                std::vector<float> l_vNearestPoint(3,0.f);
                l_vNearestPoint[0] = l_oTargetCloud->coord(0)[m_u[ii]];
                l_vNearestPoint[1] = l_oTargetCloud->coord(1)[m_u[ii]];
                l_vNearestPoint[2] = l_oTargetCloud->coord(2)[m_u[ii]];

                float l_fDistNearestPoint = swUtil::norm(swUtil::vec(l_vPt,l_vNearestPoint));

                l_fCoeffReduc = 1.f - (l_fDistNearestPoint*l_fDistNearestPoint)/(m_fMaxTemplateTargetDistance*m_fMaxTemplateTargetDistance);
            }

            cv::Mat l_oTr(X(ii));

            float l_fNewX =     l_oTr.at<float>(0,0) * l_vPt[0] +
                                l_oTr.at<float>(1,0) * l_vPt[1] +
                                l_oTr.at<float>(2,0) * l_vPt[2] +
                                l_oTr.at<float>(3,0);

            float l_fNewY =     l_oTr.at<float>(0,1) * l_vPt[0] +
                                l_oTr.at<float>(1,1) * l_vPt[1] +
                                l_oTr.at<float>(2,1) * l_vPt[2] +
                                l_oTr.at<float>(3,1);

            float l_fNewZ =     l_oTr.at<float>(0,2) * l_vPt[0] +
                                l_oTr.at<float>(1,2) * l_vPt[1] +
                                l_oTr.at<float>(2,2) * l_vPt[2] +
                                l_oTr.at<float>(3,2);

            l_oSourceCloud->coord(0)[ii] = (1.f - l_fCoeffReduc) *l_oSourceCloud->coord(0)[ii] + l_fCoeffReduc * l_fNewX;
            l_oSourceCloud->coord(1)[ii] = (1.f - l_fCoeffReduc) *l_oSourceCloud->coord(1)[ii] + l_fCoeffReduc * l_fNewY;
            l_oSourceCloud->coord(2)[ii] = (1.f - l_fCoeffReduc) *l_oSourceCloud->coord(2)[ii] + l_fCoeffReduc * l_fNewZ;

        }

//        std::cout << "MG  : " << MG_A.rows << " " << MG_A.cols << std::endl;
//        std::cout << "WD  : " << WD.rows << " " << WD.cols << std::endl;
//        std::cout << "B   : " << B.rows << " " << B.cols << std::endl;
//        std::cout << "TAA : " << TAA.rows << " " << TAA.cols << std::endl;
//        std::cout << "TAB : " << TAB.rows << " " << TAB.cols << std::endl;
//        std::cout << "TAB : " << TAB.rows << " " << TAB.cols << std::endl;
//        std::cout << "TAAInv : " << TAAInv.rows << " " << TAAInv.cols << std::endl;
//        std::cout << "newX : " << newX.rows << " " << newX.cols << std::endl;

    return l_fDiff;
}


void SWOptimalStepNonRigidICP::updateSourceMeshWithMorphModification()
{
    m_oSourceMesh.cloud()->copy(*m_pSourceMeshC->cloud());
}

cv::Mat SWOptimalStepNonRigidICP::X(cuint ii) const
{
    cv::Mat l_oMat(4, 3, CV_32FC1);

    m_X->row(4*ii)  .copyTo(l_oMat.row(0));
    m_X->row(4*ii+1).copyTo(l_oMat.row(1));
    m_X->row(4*ii+2).copyTo(l_oMat.row(2));
    m_X->row(4*ii+3).copyTo(l_oMat.row(3));

    return l_oMat;
}

cv::Mat SWOptimalStepNonRigidICP::pX(cuint ii) const
{
    cv::Mat l_oMat(4, 3, CV_32FC1);

    m_pX->row(4*ii)  .copyTo(l_oMat.row(0));
    m_pX->row(4*ii+1).copyTo(l_oMat.row(1));
    m_pX->row(4*ii+2).copyTo(l_oMat.row(2));
    m_pX->row(4*ii+3).copyTo(l_oMat.row(3));

    return l_oMat;
}




void SWOptimalStepNonRigidICP::readStasmCorrFile(const std::string &oPathCorrStasmFile, std::vector<int> &vI32CorrStasmTarget)
{
    ifstream  l_fsCorrFile(oPathCorrStasmFile);

    if(!l_fsCorrFile.is_open())
    {
        cerr << "readStasmCorrFile : file bad path " << endl;
        return;
    }

    bool l_bRead = true;

    while(l_bRead)
    {
        if(l_fsCorrFile.good())
        {
            string l_sType;
            int l_i32CorrLine;
            l_fsCorrFile >> l_sType;
            l_sType = "";
            l_fsCorrFile >> l_i32CorrLine;
            vI32CorrStasmTarget.push_back(l_i32CorrLine);
            getline(l_fsCorrFile, l_sType);
        }
        else
        {
            if(!l_fsCorrFile.eof())
            {
                cerr << "readStasmCorrFile : file read error" << endl;
            }

            l_bRead = false;
        }
    }
}



void SWOptimalStepNonRigidICP::buildW(cv::Mat &oW)
{
    // compute W matrix,  W = diag(w1, ... , wn)
        oW = cv::Mat(m_oSourceMesh.pointsNumber(), m_oSourceMesh.pointsNumber(), CV_32FC1, cv::Scalar(0.f));
        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
//            oW.at<float>(ii,ii) = static_cast<float>(m_w[ii]);
            oW.at<float>(ii,ii) = m_w[ii];
        }
}


void SWOptimalStepNonRigidICP::buildD(cv::Mat &oD)
{
    // compute D matrix
    // D = |v1^t             |  vi = [xi, yi, zi, 1]^T
    //     |    v2^t         |
    //     |         ...     |
    //     |             vn^t|
        oD = cv::Mat(m_oSourceMesh.pointsNumber(), 4 * m_oSourceMesh.pointsNumber(), CV_32FC1, cv::Scalar(0.f));

        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
            vector<float> l_oPt;
            m_oSourceMesh.point(l_oPt, ii);

            oD.at<float>(ii, ii * 4)        = l_oPt[0];
            oD.at<float>(ii, ii * 4 + 1)    = l_oPt[1];
            oD.at<float>(ii, ii * 4 + 2)    = l_oPt[2];
            oD.at<float>(ii, ii * 4 + 3)    = 1.0;
        }
}


void SWOptimalStepNonRigidICP::buildU(cv::Mat &oU)
{
    // compute U matrix, U = [u1, ..., un]^T
        oU = cv::Mat(m_oSourceMesh.pointsNumber(), 3, CV_32FC1, cv::Scalar(0.f));
        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
            vector<float> l_oPt;
            m_oTargetMesh.point(l_oPt, m_u[ii]);

            oU.at<float>(ii, 0) = l_oPt[0];
            oU.at<float>(ii, 1) = l_oPt[1];
            oU.at<float>(ii, 2) = l_oPt[2];
        }
}

//void SWOptimalStepNonRigidICP::buildMG(cv::Mat &oMG, cdouble dAlpha, cdouble dGama)
void SWOptimalStepNonRigidICP::buildMG(cv::Mat &oMG, cfloat fAlpha, cfloat fGama)
{
    // build alpha * Kronecker product(M, G)) with G = diag(1,1,1, gama) and M node-arc incidence matrix (edge, vertex)
        int l_aMGSize[] = {4 * m_pSourceMeshC->edgesNumber(), 4 * m_pSourceMeshC->pointsNumber()};
        oMG = cv::Mat(l_aMGSize[0], l_aMGSize[1], CV_32FC1, cv::Scalar(0.f));

        for(uint ii = 0, l_EdgeId = 0; ii < m_pSourceMeshC->pointsNumber(); ++ii)
        {
            vector<uint> l_aVertexLinks = m_pSourceMeshC->vertexLinks(ii);

            for(uint jj = 0; jj < l_aVertexLinks.size(); ++jj, ++l_EdgeId)
            {
                oMG.at<float>(4 * l_EdgeId,     4 * ii)     = -1.f     * fAlpha;
                oMG.at<float>(4 * l_EdgeId + 1, 4 * ii + 1) = -1.f     * fAlpha;
                oMG.at<float>(4 * l_EdgeId + 2, 4 * ii + 2) = -1.f     * fAlpha;
                oMG.at<float>(4 * l_EdgeId + 3, 4 * ii + 3) = -fGama   * fAlpha;

                oMG.at<float>(4 * l_EdgeId,     4 * l_aVertexLinks[jj])     = 1.f     * fAlpha;
                oMG.at<float>(4 * l_EdgeId + 1, 4 * l_aVertexLinks[jj] + 1) = 1.f     * fAlpha;
                oMG.at<float>(4 * l_EdgeId + 2, 4 * l_aVertexLinks[jj] + 2) = 1.f     * fAlpha;
                oMG.at<float>(4 * l_EdgeId + 3, 4 * l_aVertexLinks[jj] + 3) = fGama   * fAlpha;
            }
        }
}

void SWOptimalStepNonRigidICP::buildWD(cv::Mat &oWD)
{
    // build W * D
    int l_aWDSize[] = {m_pSourceMeshC->pointsNumber(), 4*m_pSourceMeshC->pointsNumber()};
    oWD = cv::Mat (l_aWDSize[0], l_aWDSize[1], CV_32FC1, cv::Scalar(0.f));

    for(int ii = 0; ii < oWD.rows; ++ii)
    {
        float l_aFXYZ[3];
        m_pSourceMeshC->point(l_aFXYZ, ii);

        oWD.at<float>(ii, ii*4)   = m_wC[ii] * l_aFXYZ[0];
        oWD.at<float>(ii, ii*4+1) = m_wC[ii] * l_aFXYZ[1];
        oWD.at<float>(ii, ii*4+2) = m_wC[ii] * l_aFXYZ[2];
        oWD.at<float>(ii, ii*4+3) = m_wC[ii];
    }
}

void SWOptimalStepNonRigidICP::buildWU(cv::Mat &oWU)
{
    // build W * U
    int l_aWUSize[] = {m_oSourceMesh.pointsNumber(), 3};
    oWU = cv::Mat (l_aWUSize[0], l_aWUSize[1], CV_32FC1, cv::Scalar(0.f));

    for(int ii = 0; ii < oWU.rows; ++ii)
    {
        float l_aFXYZ[3];
        m_oTargetMesh.point(l_aFXYZ, m_u[ii]);

        oWU.at<float>(ii, 0) = m_w[ii] * l_aFXYZ[0];
        oWU.at<float>(ii, 1) = m_w[ii] * l_aFXYZ[1];
        oWU.at<float>(ii, 2) = m_w[ii] * l_aFXYZ[2];
    }
}

void SWOptimalStepNonRigidICP::buildA(cv::Mat &oMG_A, cv::Mat &oWD)//, cv::Mat &oA)
{
    // build A sparse matrix
    //      | MG1 ... MGn | with MGi i-col of MG, and WDi i-col of WD
    // A =  | WD1 ... WDn |
    oMG_A.resize(oMG_A.rows + oWD.rows);

    for(int ii = oMG_A.rows - oWD.rows; ii < oMG_A.rows; ++ii)
    {
        for(int jj = 0; jj < oMG_A.cols; ++jj)
        {
            oMG_A.at<float>(ii,jj) = oWD.at<float>(ii - oMG_A.rows + oWD.rows, jj);
        }
    }
}

void SWOptimalStepNonRigidICP::buildB(cint i32MGRows, cv::Mat &oB) // TODO : possible optimization
{
    // build W * U
    int l_aWUSize[] = {m_oSourceMesh.pointsNumber(), 3};
    cv::Mat oWU(l_aWUSize[0], l_aWUSize[1], CV_32FC1, cv::Scalar(0.f));

    for(int ii = 0; ii < oWU.rows; ++ii)
    {
        float l_aFXYZ[3];
        m_oTargetMesh.point(l_aFXYZ, m_u[ii]);

        oWU.at<float>(ii, 0) = m_w[ii] * l_aFXYZ[0];
        oWU.at<float>(ii, 1) = m_w[ii] * l_aFXYZ[1];
        oWU.at<float>(ii, 2) = m_w[ii] * l_aFXYZ[2];
    }

    // build B sparse matrix
    //      | c01 ... c03 | with c0i i-col of 0 (same number of rows than MG), and WUi i-col of WU
    // B =  | WU1 ... WU3 |
    int l_aBSize[] = { i32MGRows + oWU.rows, oWU.cols};
    oB = cv::Mat(l_aBSize[0], l_aBSize[1], CV_32FC1, cv::Scalar(0.f));

    for(int ii = 0; ii < oWU.rows; ++ii)
    {
        for(int jj = 0; jj < l_aBSize[1]; ++jj)
        {
            oB.at<float>(ii + i32MGRows, jj) = oWU.at<float>(ii,jj);
        }
    }
}

bool SWOptimalStepNonRigidICP::checkDiff(cv::Mat &oMatrix1, cv::Mat &oMatrix2)
{
    if(oMatrix1.rows != oMatrix2.rows && oMatrix1.cols != oMatrix2.cols)
    {
        std::cerr << "Wrong sizes. " << std::endl;
        return false;
    }

    for(int ii = 0; ii < oMatrix1.rows; ++ii)
    {
        for(int jj = 0; jj < oMatrix1.cols; ++jj)
        {
            float l_fV1 = oMatrix1.at<float>(ii,jj);
            float l_fV2 = oMatrix2.at<float>(ii,jj);
            if(sqrt(l_fV1*l_fV1) - sqrt(l_fV2*l_fV2) > 0.001f)
            {
                std::cerr << "Differents Values.  ["  << ii << " " << jj <<"] -> " << l_fV1 << " " << l_fV2 << std::endl;
                return false;
            }
//            if(rand()%1000 == 0)
//                std::cout << ii << " "  << jj << " " << sqrt(l_fV1*l_fV1) << " " << sqrt(l_fV2*l_fV2) <<   " | ";
        }
    }

    return true;
}


void SWOptimalStepNonRigidICP::buildTAA(cv::Mat &oA, cv::Mat &oTAA)
{
    // build TA * A
    int l_aTAASize[] = {oA.cols, oA.cols};
    oTAA = cv::Mat(l_aTAASize[0], l_aTAASize[1], CV_32FC1, cv::Scalar(0.f));

    for(int ii = 0; ii < oA.cols; ++ii)
    {
        for(int jj = 0; jj < oA.rows; ++jj)
        {
            if(oA.at<float>(jj,ii) != 0.f)
            {
                for(int kk = 0; kk < oA.cols; ++kk)
                {
                    if(oA.at<float>(jj,kk) != 0.f)
                    {
                        oTAA.at<float>(ii,kk) += oA.at<float>(jj,ii) * oA.at<float>(jj,kk);
                    }
                }
            }
        }
    }
}

void SWOptimalStepNonRigidICP::buildTAB(cv::Mat &oA, cv::Mat &oB, cv::Mat &oTAB)
{
    // build TA * B
    oTAB = cv::Mat(oA.cols, oB.cols, CV_32FC1, cv::Scalar(0.f));

    for(int ii = 0; ii < oA.cols; ++ii)
    {
        for(int jj = 0; jj < oA.rows; ++jj)
        {
            if(oA.at<float>(jj,ii) != 0.f)
            {
                for(int kk = 0; kk < oB.cols; ++kk)
                {
                    if(oB.at<float>(jj,kk) != 0.f)
                    {
                        oTAB.at<float>(ii, kk) += oA.at<float>(jj,ii) * oB.at<float>(jj,kk);
                    }
                }
            }
        }
    }
}

void SWOptimalStepNonRigidICP::addLandMarks(cfloat fBeta, cint i32MGRows, cv::Mat &oA, cv::Mat &oB)
{
    std::vector<float> l_a3DSourcePt, l_a3DTargetPt;

    for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it)
    {
        uint l_ui32SourceId = it->first;
        uint l_ui32TargetId = it->second;
        m_oSourceMesh.point(l_a3DSourcePt, l_ui32SourceId);
        m_oTargetMesh.point(l_a3DTargetPt, l_ui32TargetId);

        oA.at<float>(i32MGRows+l_ui32SourceId, l_ui32SourceId * 4)      = l_a3DSourcePt[0] * fBeta;
        oA.at<float>(i32MGRows+l_ui32SourceId, l_ui32SourceId * 4 + 1)  = l_a3DSourcePt[1] * fBeta;
        oA.at<float>(i32MGRows+l_ui32SourceId, l_ui32SourceId * 4 + 2)  = l_a3DSourcePt[2] * fBeta;
        oA.at<float>(i32MGRows+l_ui32SourceId, l_ui32SourceId * 4 + 3)  = fBeta;

        oB.at<float>(i32MGRows+l_ui32SourceId, 0) = l_a3DTargetPt[0];
        oB.at<float>(i32MGRows+l_ui32SourceId, 1) = l_a3DTargetPt[1];
        oB.at<float>(i32MGRows+l_ui32SourceId, 2) = l_a3DTargetPt[2];
    }
}




//void SWOptimalStepNonRigidICP::displaySparseMatrix(cv::SparseMat_<float> &oSparseMat)
//{
//    const int *l_aSize = oSparseMat.size();

//    for(int ii = 0; ii < l_aSize[0]; ++ii)
//    {
//        for(int jj = 0; jj < l_aSize[1]; ++jj)
//        {
//            if(oSparseMat.ptr(ii,jj, false))
//            {
//                cout << oSparseMat.ref(ii, jj) << " ";
//            }
//        }
//    }
//    cout << endl;
//}


//void SWOptimalStepNonRigidICP::buildWD(cv::SparseMat_<float> &oWD)
//{
//    // build W * D
//    int l_aWDSize[] = {m_pSourceMeshC->pointsNumber(), 4*m_pSourceMeshC->pointsNumber()};
//    oWD = cv::SparseMat_<float> (2, l_aWDSize);

//    for(int ii = 0; ii < oWD.size(0); ++ii)
//    {
//        float l_aFXYZ[3];
//        m_pSourceMeshC->point(l_aFXYZ, ii);

//        oWD.ref(ii, ii*4)   = m_wC[ii] * l_aFXYZ[0];
//        oWD.ref(ii, ii*4+1) = m_wC[ii] * l_aFXYZ[1];
//        oWD.ref(ii, ii*4+2) = m_wC[ii] * l_aFXYZ[2];
//        oWD.ref(ii, ii*4+3) = m_wC[ii];
//    }
//}

//void SWOptimalStepNonRigidICP::buildMG(cv::SparseMat_<float> &oMG, cfloat fAlpha, cfloat fGama)
//{
//    // build alpha * Kronecker product(M, G)) with G = diag(1,1,1, gama) and M node-arc incidence matrix (edge, vertex)
//        int l_aMGSize[] = {4 * m_pSourceMeshC->edgesNumber(), 4 * m_pSourceMeshC->pointsNumber()};
//        oMG = cv::SparseMat_<float> (2, l_aMGSize);

//        for(uint ii = 0, l_EdgeId = 0; ii < m_pSourceMeshC->pointsNumber(); ++ii)
//        {
//            vector<uint> l_aVertexLinks = m_pSourceMeshC->vertexLinks(ii);

//            for(uint jj = 0; jj < l_aVertexLinks.size(); ++jj, ++l_EdgeId)
//            {
//                oMG.ref(4 * l_EdgeId,     4 * ii)     = -1.f     * fAlpha;
//                oMG.ref(4 * l_EdgeId + 1, 4 * ii + 1) = -1.f     * fAlpha;
//                oMG.ref(4 * l_EdgeId + 2, 4 * ii + 2) = -1.f     * fAlpha;
//                oMG.ref(4 * l_EdgeId + 3, 4 * ii + 3) = -fGama   * fAlpha;

//                oMG.ref(4 * l_EdgeId,     4 * l_aVertexLinks[jj])     = 1.f     * fAlpha;
//                oMG.ref(4 * l_EdgeId + 1, 4 * l_aVertexLinks[jj] + 1) = 1.f     * fAlpha;
//                oMG.ref(4 * l_EdgeId + 2, 4 * l_aVertexLinks[jj] + 2) = 1.f     * fAlpha;
//                oMG.ref(4 * l_EdgeId + 3, 4 * l_aVertexLinks[jj] + 3) = fGama   * fAlpha;
//            }
//        }
//}

////void SWOptimalStepNonRigidICP::buildWU(cv::SparseMat_<double> &oWU)
//void SWOptimalStepNonRigidICP::buildWU(cv::SparseMat_<float> &oWU)
//{
//    // build W * U
//    int l_aWUSize[] = {m_oSourceMesh.pointsNumber(), 3};
//    oWU = cv::SparseMat_<float> (2, l_aWUSize);

//    for(int ii = 0; ii < oWU.size(0); ++ii)
//    {
//        float l_aFXYZ[3];
//        m_oTargetMesh.point(l_aFXYZ, m_u[ii]);
//        oWU.ref(ii, 0) = m_w[ii] * l_aFXYZ[0];
//        oWU.ref(ii, 1) = m_w[ii] * l_aFXYZ[1];
//        oWU.ref(ii, 2) = m_w[ii] * l_aFXYZ[2];
//    }
//}

//void SWOptimalStepNonRigidICP::addLandMarks(cfloat fBeta, cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oA, cv::SparseMat_<float> &oB)
//{
//    std::vector<float> l_a3DSourcePt, l_a3DTargetPt;

//    for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it)
//    {
//        uint l_ui32SourceId = it->first;
//        uint l_ui32TargetId = it->second;
//        m_oSourceMesh.point(l_a3DSourcePt, l_ui32SourceId);
//        m_oTargetMesh.point(l_a3DTargetPt, l_ui32TargetId);

//        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4)      = l_a3DSourcePt[0] * fBeta;
//        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4 + 1)  = l_a3DSourcePt[1] * fBeta;
//        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4 + 2)  = l_a3DSourcePt[2] * fBeta;
//        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4 + 3)  = fBeta;

////        oB.ref(oMG.size(0)+l_ui32TargetId, 0) = l_a3DTargetPt[0];
////        oB.ref(oMG.size(0)+l_ui32TargetId, 1) = l_a3DTargetPt[1];
////        oB.ref(oMG.size(0)+l_ui32TargetId, 2) = l_a3DTargetPt[2];

//        oB.ref(oMG.size(0)+l_ui32SourceId, 0) = l_a3DTargetPt[0];
//        oB.ref(oMG.size(0)+l_ui32SourceId, 1) = l_a3DTargetPt[1];
//        oB.ref(oMG.size(0)+l_ui32SourceId, 2) = l_a3DTargetPt[2];
//    }
//}

//void SWOptimalStepNonRigidICP::buildA(cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oWD, cv::SparseMat_<float> &oA)
//{
//    // build A sparse matrix
//    //      | MG1 ... MGn | with MGi i-col of MG, and WDi i-col of WD
//    // A =  | WD1 ... WDn |
//    int l_aASize[] = { oMG.size(0) + oWD.size(0), oMG.size(1)};

//    oA = cv::SparseMat_<float>(2, l_aASize);

//    for(int jj = 0; jj < l_aASize[1]; ++jj)
//    {
//        for(int ii = 0; ii < l_aASize[0]; ++ii)
//        {
//            if(ii <  oMG.size(0))
//            {
//                if(oMG.ptr(ii,jj, false))
//                {
//                    oA.ref(ii,jj) = oMG.ref(ii,jj);
//                }
//            }
//            else
//            {
//                if(oWD.ptr(ii - oMG.size(0),jj, false))
//                {
//                    oA.ref(ii,jj) = oWD.ref(ii - oMG.size(0),jj);
//                }
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::buildB(cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oWU, cv::SparseMat_<float> &oB)
//{
//    // build B sparse matrix
//    //      | c01 ... c03 | with c0i i-col of 0 (same number of rows than MG), and WUi i-col of WU
//    // B =  | WU1 ... WU3 |
//    int l_aBSize[] = { oMG.size(0) + oWU.size(0), oWU.size(1)};
//    oB = cv::SparseMat_<float>(2, l_aBSize);

//    for(int ii = 0; ii < oWU.size(0); ++ii)
//    {
//        for(int jj = 0; jj < l_aBSize[1]; ++jj)
//        {
//            if(oWU.ptr(ii,jj, false))
//            {
//                oB.ref(ii + oMG.size(0), jj) = oWU.ref(ii,jj);
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::buildTAA(cv::SparseMat_<float> &oA, cv::SparseMat_<float> &oTAA)
//{
//    // build TA * A
//    int l_aTAASize[] = {oA.size(1), oA.size(1)};
//    oTAA = cv::SparseMat_<float>(2, l_aTAASize);

//    for(int ii = 0; ii < oA.size(1); ++ii)
//    {
//        for(int jj = 0; jj < oA.size(0); ++jj)
//        {
//            if(oA.ptr(jj,ii, false))
//            {
//                for(int kk = 0; kk < oA.size(1); ++kk)
//                {
//                    if(oA.ptr(jj,kk, false))
//                    {
//                        oTAA.ref(ii,kk) += oA.ref(jj,ii) * oA.ref(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::buildTAB(cv::SparseMat_<float> &oA, cv::SparseMat_<float> &oB, cv::Mat &oTAB)
//{
//    // build TA * B
//    oTAB = cv::Mat(oA.size(1), oB.size(1), CV_32FC1, cv::Scalar(0.f));

//    for(int ii = 0; ii < oA.size(1); ++ii)
//    {
//        for(int jj = 0; jj < oA.size(0); ++jj)
//        {
//            if(oA.ptr(jj,ii, false))
//            {
//                for(int kk = 0; kk < oB.size(1); ++kk)
//                {
//                    if(oB.ptr(jj,kk, false))
//                    {
//                        oTAB.at<float>(ii, kk) += oA.ref(jj,ii) * oB.ref(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::buildAX(cv::SparseMat_<float> &oA, cv::Mat &oX, cv::Mat &oAX)
//{
//    // build A*X
//    oAX = cv::Mat(oA.size(0), oX.cols, CV_32FC1, cv::Scalar(0.f));

//    for(int ii = 0; ii < oA.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oA.size(1); ++jj)
//        {
//            if(oA.ptr(ii,jj, false))
//            {
//                for(int kk = 0; kk < oX.cols; ++kk)
//                {
//                    if(oA.ptr(ii,jj, false))
//                    {
//                        oAX.at<float>(ii,kk) += oA.ref(ii,jj) * oX.at<float>(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}

//bool SWOptimalStepNonRigidICP::checkDiff(cv::Mat &oMatrix, cv::SparseMat_<float> &oSparseMatrix)
//{
//    if(oMatrix.rows != oSparseMatrix.size(0) && oMatrix.cols != oSparseMatrix.size(1))
//    {
//        std::cerr << "Wrong sizes. " << std::endl;
//        return false;
//    }

//    for(int ii = 0; ii < oMatrix.rows; ++ii)
//    {
//        for(int jj = 0; jj < oMatrix.cols; ++jj)
//        {
//            if(oSparseMatrix.ptr(ii,jj, false))
//            {
//                float l_fV1 = oMatrix.at<float>(ii,jj);
//                float l_fV2 = oSparseMatrix.ref(ii,jj);
//                if(sqrt(l_fV1*l_fV1) - sqrt(l_fV2*l_fV2) > 0.01f)
//                {
//                    std::cerr << "Differents Values.  ["  << ii << " " << jj <<"] -> " << l_fV1 << " " << l_fV2 << std::endl;
//                    return false;
//                }

////                if(rand()%1000 == 0)
////                    std::cout << ii << " "  << jj << " " << sqrt(l_fV1*l_fV1) << " " << sqrt(l_fV2*l_fV2) <<   " | ";
//            }
//        }
//    }

//    return true;
//}
