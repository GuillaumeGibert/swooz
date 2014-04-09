
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
        m_X  = new cv::Mat(4 * oSource.pointsNumber(), 3, CV_64FC1, cv::Scalar(0.0));
        m_pX = new cv::Mat(4 * oSource.pointsNumber(), 3, CV_64FC1, cv::Scalar(0.0));

    // init distance weights array
        m_w.assign(oSource.pointsNumber(), 1.0);
        m_w1.assign(oSource.pointsNumber(), 1.0);
        m_w2.assign(oSource.pointsNumber(), 1.0);
        m_w3.assign(oSource.pointsNumber(), 1.0);

    // init correspondances array
        m_u.assign(oSource.pointsNumber(), 0);

    // set initial parameters
        m_dAngleMax = 50.0;
        m_dLastComputedCost = -1.0;
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
        for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it)
        {
            uint l_ui32TemplateId = it->first;
            uint l_ui32TargetId   = it->second;

//            cout << "m_l : " << l_ui32TemplateId << " " << l_ui32TargetId << endl;
        }


    //  update triangles normals
        m_oTargetMesh.updateNonOrientedTrianglesNormals();
        m_oSourceMesh.updateNonOrientedTrianglesNormals();

    // update vertices normals
        m_oTargetMesh.updateNonOrientedVerticesNormals();
        m_oSourceMesh.updateNonOrientedVerticesNormals();
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
    for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
    {
        m_u[ii] = m_oSourceMesh.idNearestPoint(ii, m_oTargetMesh);
    }
}


void SWOptimalStepNonRigidICP::computeDistanceWeights()
{
    clock_t m_oProgramTime = clock();

    // set to 1 all the weights
        m_w.assign(m_oSourceMesh.pointsNumber(), 1.0);
        m_w1.assign(m_oSourceMesh.pointsNumber(), 1.0);
        m_w2.assign(m_oSourceMesh.pointsNumber(), 1.0);
        m_w3.assign(m_oSourceMesh.pointsNumber(), 1.0);

    // 1) ui lies on a border of the target mesh
        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
            if(m_oTargetMesh.vertexOnBorder(m_u[ii]))
            {
                m_w[ii] = 0.0;
                m_w1[ii] = 0.0;
            }
        }

        cout << " [1 " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << " ] ";
        m_oProgramTime = clock();

    // 2) the angle between the normals of the meshes at Xivi and ui is larger than a fixed threshold
        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
//            if(m_w[ii] == 1.0)
//            {
                vector<double> l_vXiViNormal;
                m_oSourceMesh.vertexNormal(l_vXiViNormal , ii);

                vector<double> l_vUiNormal;
                m_oTargetMesh.vertexNormal(l_vUiNormal, m_u[ii]);

                double l_dAngle = swUtil::vectorAngle(l_vXiViNormal, l_vUiNormal);

                if(l_dAngle > m_dAngleMax)
                {
                    m_w[ii] = 0.0;
                    m_w2[ii] = 0.0;
                }
//            }
        }

        cout << " [2 " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << " ] ";
        m_oProgramTime = clock();

    // 3) the line segment Xivi to ui intersects the deformed template
        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
        {
//            if(m_w[ii] == 1.0)
//            {
                std::vector<double> l_vP, l_vD;
                m_oSourceMesh.point(l_vP, ii);
                m_oTargetMesh.point(l_vD, m_u[ii]);

                bool l_bIntersect = false;

                for(uint jj = 0; jj < m_oSourceMesh.trianglesNumber(); ++jj)
                {
                    std::vector<double> l_vV1, l_vV2, l_vV3;
                    m_oSourceMesh.trianglePoints(l_vV1, l_vV2, l_vV3, jj);

                    std::vector<double> m_vTriMiddle(3,0.0);
                    m_vTriMiddle[0] = (l_vV1[0] + l_vV2[0] + l_vV3[0])/3.0;
                    m_vTriMiddle[1] = (l_vV1[1] + l_vV2[1] + l_vV3[1])/3.0;
                    m_vTriMiddle[2] = (l_vV1[2] + l_vV2[2] + l_vV3[2])/3.0;

                    if(swUtil::norm(swUtil::vec(l_vP,m_vTriMiddle)) > m_fWeightVectorDistMax && swUtil::norm(swUtil::vec(m_vTriMiddle,l_vD)) > m_fWeightVectorDistMax)
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
                    m_w[ii]  = 0.0;
                    m_w3[ii] = 0.0;
                }
//            }
        }

        cout << " [3 " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << " ] ";
}

double SWOptimalStepNonRigidICP::totalEnergy() const
{
    return m_dLastComputedCost;
}


void SWOptimalStepNonRigidICP::displaySparseMatrix(cv::SparseMat_<double> &oSparseMat)
{
    const int *l_aSize = oSparseMat.size();

    for(int ii = 0; ii < l_aSize[0]; ++ii)
    {
        for(int jj = 0; jj < l_aSize[1]; ++jj)
        {
            if(oSparseMat.ptr(ii,jj, false))
            {
                cout << oSparseMat.ref(ii, jj) << " ";
            }
        }
    }
    cout << endl;
}

void SWOptimalStepNonRigidICP::displayDenseMatrix(cv::Mat &oMat)
{
    for(int ii = 0; ii < oMat.rows; ++ii)
    {
        for(int jj = 0; jj < oMat.cols; ++jj)
        {
            if(oMat.at<double>(ii,jj) != 0.0)
            {
                cout << oMat.at<double>(ii,jj) << " ";
            }
        }
    }
    cout << endl;
}

void SWOptimalStepNonRigidICP::buildWD(cv::SparseMat_<double> &oWD)
{
    // build W * D
    int l_aWDSize[] = {m_pSourceMeshC->pointsNumber(), 4*m_pSourceMeshC->pointsNumber()};
    oWD = cv::SparseMat_<double> (2, l_aWDSize);

    for(int ii = 0; ii < oWD.size(0); ++ii)
    {
        float l_aFXYZ[3];
        m_pSourceMeshC->point(l_aFXYZ, ii);

        oWD.ref(ii, ii*4)   = m_wC[ii] * l_aFXYZ[0];
        oWD.ref(ii, ii*4+1) = m_wC[ii] * l_aFXYZ[1];
        oWD.ref(ii, ii*4+2) = m_wC[ii] * l_aFXYZ[2];
        oWD.ref(ii, ii*4+3) = m_wC[ii];
    }
}

void SWOptimalStepNonRigidICP::buildMG(cv::SparseMat_<double> &oMG, cdouble dAlpha, cdouble dGama)
{
    // build alpha * Kronecker product(M, G)) with G = diag(1,1,1, gama) and M node-arc incidence matrix (edge, vertex)
        int l_aMGSize[] = {4 * m_pSourceMeshC->edgesNumber(), 4 * m_pSourceMeshC->pointsNumber()};
        oMG = cv::SparseMat_<double> (2, l_aMGSize);

        for(uint ii = 0, l_EdgeId = 0; ii < m_pSourceMeshC->pointsNumber(); ++ii)
        {
            vector<uint> l_aVertexLinks = m_pSourceMeshC->vertexLinks(ii);

            for(uint jj = 0; jj < l_aVertexLinks.size(); ++jj, ++l_EdgeId)
            {
                oMG.ref(4 * l_EdgeId,     4 * ii)     = -1.0     * dAlpha;
                oMG.ref(4 * l_EdgeId + 1, 4 * ii + 1) = -1.0     * dAlpha;
                oMG.ref(4 * l_EdgeId + 2, 4 * ii + 2) = -1.0     * dAlpha;
                oMG.ref(4 * l_EdgeId + 3, 4 * ii + 3) = -dGama   * dAlpha;

                oMG.ref(4 * l_EdgeId,     4 * l_aVertexLinks[jj])     = 1.0     * dAlpha;
                oMG.ref(4 * l_EdgeId + 1, 4 * l_aVertexLinks[jj] + 1) = 1.0     * dAlpha;
                oMG.ref(4 * l_EdgeId + 2, 4 * l_aVertexLinks[jj] + 2) = 1.0     * dAlpha;
                oMG.ref(4 * l_EdgeId + 3, 4 * l_aVertexLinks[jj] + 3) = dGama   * dAlpha;
            }
        }
}

void SWOptimalStepNonRigidICP::buildWU(cv::SparseMat_<double> &oWU)
{
    // build W * U
    int l_aWUSize[] = {m_oSourceMesh.pointsNumber(), 3};
    oWU = cv::SparseMat_<double> (2, l_aWUSize);

    for(int ii = 0; ii < oWU.size(0); ++ii)
    {
        float l_aFXYZ[3];
        m_oTargetMesh.point(l_aFXYZ, m_u[ii]);

        oWU.ref(ii, 0) = m_w[ii] * l_aFXYZ[0];
        oWU.ref(ii, 1) = m_w[ii] * l_aFXYZ[1];
        oWU.ref(ii, 2) = m_w[ii] * l_aFXYZ[2];
    }
}



void SWOptimalStepNonRigidICP::addLandMarks(cdouble dBeta, cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oB)
{
    vector<double> l_a3DSourcePt, l_a3DTargetPt;

    for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it)
    {
        uint l_ui32SourceId = it->first;
        uint l_ui32TargetId = it->second;
        m_oSourceMesh.point(l_a3DSourcePt, l_ui32SourceId);
        m_oTargetMesh.point(l_a3DTargetPt, l_ui32TargetId);
//        cout << swUtil::norm(swUtil::vec(l_a3DSourcePt, l_a3DTargetPt)) << " ";

        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4)      = l_a3DSourcePt[0] * dBeta;
        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4 + 1)  = l_a3DSourcePt[1] * dBeta;
        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4 + 2)  = l_a3DSourcePt[2] * dBeta;
        oA.ref(oMG.size(0)+l_ui32SourceId, l_ui32SourceId * 4 + 3)  = dBeta;

//        oB.ref(oMG.size(0)+l_ui32TargetId, 0) = l_a3DTargetPt[0];
//        oB.ref(oMG.size(0)+l_ui32TargetId, 1) = l_a3DTargetPt[1];
//        oB.ref(oMG.size(0)+l_ui32TargetId, 2) = l_a3DTargetPt[2];

        oB.ref(oMG.size(0)+l_ui32SourceId, 0) = l_a3DTargetPt[0];
        oB.ref(oMG.size(0)+l_ui32SourceId, 1) = l_a3DTargetPt[1];
        oB.ref(oMG.size(0)+l_ui32SourceId, 2) = l_a3DTargetPt[2];
    }
}

void SWOptimalStepNonRigidICP::buildA(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWD, cv::SparseMat_<double> &oA)
{
    // build A sparse matrix
    //      | MG1 ... MGn | with MGi i-col of MG, and WDi i-col of WD
    // A =  | WD1 ... WDn |
    int l_aASize[] = { oMG.size(0) + oWD.size(0), oMG.size(1)};

    oA = cv::SparseMat_<double>(2, l_aASize);

    for(int jj = 0; jj < l_aASize[1]; ++jj)
    {
        for(int ii = 0; ii < l_aASize[0]; ++ii)
        {
            if(ii <  oMG.size(0))
            {
                if(oMG.ptr(ii,jj, false))
                {
                    oA.ref(ii,jj) = oMG.ref(ii,jj);
                }
            }
            else
            {
                if(oWD.ptr(ii - oMG.size(0),jj, false))
                {
                    oA.ref(ii,jj) = oWD.ref(ii - oMG.size(0),jj);
                }
            }
        }
    }
}

void SWOptimalStepNonRigidICP::buildB(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWU, cv::SparseMat_<double> &oB)
{
    // build B sparse matrix
    //      | c01 ... c03 | with c0i i-col of 0 (same number of rows than MG), and WUi i-col of WU
    // B =  | WU1 ... WU3 |
    int l_aBSize[] = { oMG.size(0) + oWU.size(0), oWU.size(1)};
    oB = cv::SparseMat_<double>(2, l_aBSize);

    for(int ii = 0; ii < oWU.size(0); ++ii)
    {
        for(int jj = 0; jj < l_aBSize[1]; ++jj)
        {
            if(oWU.ptr(ii,jj, false))
            {
                oB.ref(ii + oMG.size(0), jj) = oWU.ref(ii,jj);
            }
        }
    }
}

void SWOptimalStepNonRigidICP::buildTAA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTAA)
{
    // build TA * A
    int l_aTAASize[] = {oA.size(1), oA.size(1)};
    oTAA = cv::SparseMat_<double>(2, l_aTAASize);

    for(int ii = 0; ii < oA.size(1); ++ii)
    {
        for(int jj = 0; jj < oA.size(0); ++jj)
        {
            if(oA.ptr(jj,ii, false))
            {
                for(int kk = 0; kk < oA.size(1); ++kk)
                {
                    if(oA.ptr(jj,kk, false))
                    {
                        oTAA.ref(ii,kk) += oA.ref(jj,ii) * oA.ref(jj,kk);
                    }
                }
            }
        }
    }
}

void SWOptimalStepNonRigidICP::buildTAB(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oB, cv::Mat &oTAB)
{
    // build TA * B
    oTAB = cv::Mat(oA.size(1), oB.size(1), CV_64FC1, cv::Scalar(0.0));

    for(int ii = 0; ii < oA.size(1); ++ii)
    {
        for(int jj = 0; jj < oA.size(0); ++jj)
        {
            if(oA.ptr(jj,ii, false))
            {
                for(int kk = 0; kk < oB.size(1); ++kk)
                {
                    if(oB.ptr(jj,kk, false))
                    {
                        oTAB.at<double>(ii, kk) += oA.ref(jj,ii) * oB.ref(jj,kk);
                    }
                }
            }
        }
    }
}

void SWOptimalStepNonRigidICP::buildAX(cv::SparseMat_<double> &oA, cv::Mat &oX, cv::Mat &oAX)
{
    // build A*X
    oAX = cv::Mat(oA.size(0), oX.cols, CV_64FC1, cv::Scalar(0.0));

    for(int ii = 0; ii < oA.size(0); ++ii)
    {
        for(int jj = 0; jj < oA.size(1); ++jj)
        {
            if(oA.ptr(ii,jj, false))
            {
                for(int kk = 0; kk < oX.cols; ++kk)
                {
                    if(oA.ptr(ii,jj, false))
                    {
                        oAX.at<double>(ii,kk) += oA.ref(ii,jj) * oX.at<double>(jj,kk);
                    }
                }
            }
        }
    }
}

double SWOptimalStepNonRigidICP::computeDiff(cv::Mat &newX)
{
    cv::Mat l_oDiff(newX.rows, newX.cols, CV_64FC1);

    // update m_X and m_pX and compute the normed difference
        for(int ii = 0; ii < newX.rows; ++ii)
        {
            for(int jj = 0; jj < newX.cols; ++jj)
            {
                m_pX->at<double>(ii,jj) = m_X->at<double>(ii,jj);
                m_X->at<double>(ii,jj) = newX.at<double>(ii,jj);

                l_oDiff.at<double>(ii,jj) = m_X->at<double>(ii,jj) - m_pX->at<double>(ii,jj);
            }
        }

    return cv::norm(l_oDiff);
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


double SWOptimalStepNonRigidICP::resolve(cdouble dAlpha, cdouble dBeta, cdouble dGama, cbool bUseLandMarks)
{
    clock_t m_oProgramTime;
//    m_oProgramTime = clock(); initResolve(); cout << " initStep " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    cv::SparseMat_<double> sMG, sWD, sWU, sA, sB, sTA, sTAA;
    cv::Mat TAB, TAAInv, newX, AX;
    m_oProgramTime = clock(); buildMG(sMG, dAlpha, dGama);  cout << " sMG " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    m_oProgramTime = clock(); buildWD(sWD);                 cout << " sWD " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    m_oProgramTime = clock(); buildWU(sWU);                 cout << " sWU " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    m_oProgramTime = clock(); buildA(sMG, sWD, sA);         cout << " A   " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    m_oProgramTime = clock(); buildB(sMG, sWU, sB);         cout << " B   " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";

    if(bUseLandMarks)
    {
         m_oProgramTime = clock(); addLandMarks(dBeta,sMG,sA,sB); cout << " addL " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    }

    m_oProgramTime = clock(); buildTAA(sA, sTAA);           cout << " TAA " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    m_oProgramTime = clock(); buildTAB(sA, sB, TAB);        cout << " TAB " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";

    m_oProgramTime = clock(); swUtil::swCuda::matrixInversion(sTAA, TAAInv); cout << " TAAInv " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";
    m_oProgramTime = clock(); newX = (TAAInv * TAB);                         cout << " newX   " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";

    // DEBUG
//    buildAX(sA, newX, AX);

//    // build AX - B
//    cv::Mat AX_B(AX.rows, AX.cols, CV_64FC1);
//    for(int ii = 0; ii < AX.rows; ++ii)
//    {
//        for(int jj = 0; jj < AX.cols; ++jj)
//        {
//            AX_B.at<double>(ii,jj) = AX.at<double>(ii,jj) - sB.ref(ii,jj);
//        }
//    }

//    m_dLastComputedCost = cv::norm(AX_B);
//    m_dLastComputedCost *= m_dLastComputedCost;
//    cout << "Result cost function :  " << m_dLastComputedCost << endl;

    m_oProgramTime = clock(); double l_dDiff = computeDiff(newX); cout << " diffX   " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";


    m_oProgramTime = clock();
    // apply deformation to the source cloud)
        swCloud::SWCloud *l_oSourceCloud = m_pSourceMeshC->cloud();
        swCloud::SWCloud *l_oTargetCloud = m_pTargetMeshC->cloud();

        for(uint ii = 0; ii < l_oSourceCloud->size(); ++ii)
        {
//            cv::Vec3f l_oPt(l_oSourceCloud->coord(0)[ii], l_oSourceCloud->coord(1)[ii], l_oSourceCloud->coord(2)[ii]);

            std::vector<float> l_vPt(3,0);
            l_vPt[0] = l_oSourceCloud->coord(0)[ii];
            l_vPt[1] = l_oSourceCloud->coord(1)[ii];
            l_vPt[2] = l_oSourceCloud->coord(2)[ii];

//            std::vector<float> l_vMeanPoint(3,0);
//            l_vMeanPoint.push_back(l_oTargetCloud->coord(0)[m_i32IdCloseTarget]);
//            l_vMeanPoint.push_back(l_oTargetCloud->coord(1)[m_i32IdCloseTarget]);
//            l_vMeanPoint.push_back(l_oTargetCloud->coord(2)[m_i32IdCloseTarget]);


//            int l_i32IdNearestPoint = l_oTargetCloud->idNearestPoint(l_vPt);
            std::vector<float> l_vNearestPoint(3,0);
            l_vNearestPoint[0] = l_oTargetCloud->coord(0)[m_u[ii]];
            l_vNearestPoint[1] = l_oTargetCloud->coord(1)[m_u[ii]];
            l_vNearestPoint[2] = l_oTargetCloud->coord(2)[m_u[ii]];
//            l_vNearestPoint[0] = l_oTargetCloud->coord(0)[l_i32IdNearestPoint];
//            l_vNearestPoint[1] = l_oTargetCloud->coord(1)[l_i32IdNearestPoint];
//            l_vNearestPoint[2] = l_oTargetCloud->coord(2)[l_i32IdNearestPoint];

//            float l_fDistNearestPoint = swUtil::norm(swUtil::vec(l_vPt,l_vNearestPoint));


//            float l_fDistMeanPoint = swUtil::norm(swUtil::vec(l_vPt, l_vMeanPoint));


            float l_fCoeffReduc = 1.f;//1.f;


//            cout << l_i32IdNearestPoint << " - " << l_fDistNearestPoint << " | ";
//            if(l_fDistNearestPoint > 0.05f )
//            {
//                if(m_w3[ii] == 1.0)
//                {
//                    l_fCoeffReduc = 0.f;
//                }
////                cout << m_w1[ii] << " " << m_w2[ii] << " " << m_w3[ii] << " | ";

//            }

            cv::Mat l_oTr(X(ii));

//            float l_fNewX = (float)(l_oTr.at<double>(0,0) * l_oPt(0) +  l_oTr.at<double>(0,1) * l_oPt(1) + l_oTr.at<double>(0,2) * l_oPt(2) + l_oTr.at<double>(0,3));
//            float l_fNewY = (float)(l_oTr.at<double>(1,0) * l_oPt(0) +  l_oTr.at<double>(1,1) * l_oPt(1) + l_oTr.at<double>(1,2) * l_oPt(2) + l_oTr.at<double>(1,3));
//            float l_fNewZ = (float)(l_oTr.at<double>(2,0) * l_oPt(0) +  l_oTr.at<double>(2,1) * l_oPt(1) + l_oTr.at<double>(2,2) * l_oPt(2) + l_oTr.at<double>(2,3));

//            cout << l_oTr << endl;

            float l_fNewX = (float)(l_oTr.at<double>(0,0) * l_vPt[0] +
                                    l_oTr.at<double>(1,0) * l_vPt[1] +
                                    l_oTr.at<double>(2,0) * l_vPt[2] +
                                    l_oTr.at<double>(3,0));

            float l_fNewY = (float)(l_oTr.at<double>(0,1) * l_vPt[0] +
                                    l_oTr.at<double>(1,1) * l_vPt[1] +
                                    l_oTr.at<double>(2,1) * l_vPt[2] +
                                    l_oTr.at<double>(3,1));

            float l_fNewZ = (float)(l_oTr.at<double>(0,2) * l_vPt[0] +
                                    l_oTr.at<double>(1,2) * l_vPt[1] +
                                    l_oTr.at<double>(2,2) * l_vPt[2] +
                                    l_oTr.at<double>(3,2));

            l_oSourceCloud->coord(0)[ii] = (1 - l_fCoeffReduc) *l_oSourceCloud->coord(0)[ii] + l_fCoeffReduc * l_fNewX;
            l_oSourceCloud->coord(1)[ii] = (1 - l_fCoeffReduc) *l_oSourceCloud->coord(1)[ii] + l_fCoeffReduc * l_fNewY;
            l_oSourceCloud->coord(2)[ii] = (1 - l_fCoeffReduc) *l_oSourceCloud->coord(2)[ii] + l_fCoeffReduc * l_fNewZ;

//            l_oSourceCloud->coord(0)[ii] = l_fNewX;
//            l_oSourceCloud->coord(1)[ii] = l_fNewY;
//            l_oSourceCloud->coord(2)[ii] = l_fNewZ;
        }

    // DEBUG
        cout << "MG : " << sMG.size(0) << " " << sMG.size(1) << endl;
        cout << "A  : " << sA.size(0) << " " << sA.size(1) << endl;
        cout << "B  : " << sB.size(0) << " " << sB.size(1) << endl;
        cout << "TA : " << sTA.size(0) << " " << sTA.size(1) << endl;
        cout << "TAAInv: " << TAAInv.rows << " " << TAAInv.cols << endl;
        cout << "TAB: " << TAB.rows << " " << TAB.cols << endl;
        cout << "X  : " << newX.rows << " " << newX.cols << endl;
        cout << "AX : " << AX.rows << " " << AX.cols << endl << endl;
        cout << " applyDef   " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC  << " |";

//    sMG.clear(), sWD.clear(), sWU.clear(), sA.clear(), sB.clear(), sTA.clear(), sTAA.clear();


    return l_dDiff;
}


void SWOptimalStepNonRigidICP::updateSourceMeshWithMorphModification()
{
    m_oSourceMesh.cloud()->copy(*m_pSourceMeshC->cloud());
}

cv::Mat SWOptimalStepNonRigidICP::X(cuint ii) const
{
    cv::Mat l_oMat(4, 3, CV_64FC1);

    m_X->row(4*ii)  .copyTo(l_oMat.row(0));
    m_X->row(4*ii+1).copyTo(l_oMat.row(1));
    m_X->row(4*ii+2).copyTo(l_oMat.row(2));
    m_X->row(4*ii+3).copyTo(l_oMat.row(3));

    return l_oMat;
}

cv::Mat SWOptimalStepNonRigidICP::pX(cuint ii) const
{
    cv::Mat l_oMat(4, 3, CV_64FC1);

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
            std::cout <<  vI32CorrStasmTarget.back() << " | ";
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



//    swCloud::SWCloud *l_oSourceCloud = m_oSourceMesh.cloud();
//    swCloud::SWCloud *l_oTargetCloud = m_oTargetMesh.cloud();

//    std::vector<float> l_vFNearPointsX, l_vFNearPointsY, l_vFNearPointsZ;
//    for(uint ii = 0; ii < l_oSourceCloud->size(); ++ii)
//    {
//        cv::Vec3f l_oPt(l_oSourceCloud->coord(0)[ii], l_oSourceCloud->coord(1)[ii], l_oSourceCloud->coord(2)[ii]);

//        std::vector<float> l_vPt(3,0);
//        l_vPt.push_back(l_oPt[0]);
//        l_vPt.push_back(l_oPt[1]);
//        l_vPt.push_back(l_oPt[2]);

//        int l_i32IdNearestPoint = l_oTargetCloud->idNearestPoint(l_vPt);
//        std::vector<float> l_vNearestPoint(3,0);
//        l_vNearestPoint.push_back(l_oTargetCloud->coord(0)[l_i32IdNearestPoint]);
//        l_vNearestPoint.push_back(l_oTargetCloud->coord(1)[l_i32IdNearestPoint]);
//        l_vNearestPoint.push_back(l_oTargetCloud->coord(2)[l_i32IdNearestPoint]);

//        if(swUtil::norm(swUtil::vec(l_vPt,l_vNearestPoint)) < 0.12f)
//        {
//            l_vFNearPointsX.push_back(l_oPt[0]);
//            l_vFNearPointsY.push_back(l_oPt[1]);
//            l_vFNearPointsZ.push_back(l_oPt[2]);
//        }
//    }


//    swCloud::SWCloud test(l_vFNearPointsX, l_vFNearPointsY, l_vFNearPointsZ);
//    cout << "test " << test.size() << " " << l_oSourceCloud->size() << endl;

//    // set clouds alignment params
////        m_oAlignClouds.setClouds(*m_oTargetMesh.cloud() ,*m_oSourceMesh.cloud());
//        m_oAlignClouds.setClouds(*m_oTargetMesh.cloud() ,test);
//        m_oAlignClouds.setReduction(5, 100);

//    // align the source cloud with the target cloud for the first iteration
//        m_oAlignClouds.alignClouds();

//    // retrieve the rigid motion
//        swCloud::SWRigidMotion l_oR = m_oAlignClouds.rigidMotion();

//    // init X^0 with the rigid motion
//        cv::Mat l_oX(3, 4, CV_64FC1, cv::Scalar(0.0));
//        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//        {
//            l_oX.at<double>(0,0) = l_oR.m_aFRotation[0]; l_oX.at<double>(0,1) = l_oR.m_aFRotation[1]; l_oX.at<double>(0,2) = l_oR.m_aFRotation[2]; l_oX.at<double>(0,3) = l_oR.m_aFTranslation[0];
//            l_oX.at<double>(1,0) = l_oR.m_aFRotation[3]; l_oX.at<double>(1,1) = l_oR.m_aFRotation[4]; l_oX.at<double>(1,2) = l_oR.m_aFRotation[5]; l_oX.at<double>(1,3) = l_oR.m_aFTranslation[1];
//            l_oX.at<double>(2,0) = l_oR.m_aFRotation[6]; l_oX.at<double>(2,1) = l_oR.m_aFRotation[7]; l_oX.at<double>(2,2) = l_oR.m_aFRotation[8]; l_oX.at<double>(2,3) = l_oR.m_aFTranslation[2];

//            setX(ii, l_oX);
//        }
// apply the first transformation to the source cloud
//        m_oAlignClouds.transformedCloud(*m_oSourceMesh.cloud());



//void SWOptimalStepNonRigidICP::setX(cuint ii, cv::Mat &oX)
//{
//    oX.row(0).copyTo(m_X->row(4*ii));
//    oX.row(1).copyTo(m_X->row(4*ii+1));
//    oX.row(2).copyTo(m_X->row(4*ii+2));
//    oX.row(3).copyTo(m_X->row(4*ii+3));
//}

//void SWOptimalStepNonRigidICP::setPX(cuint ii, cv::Mat &oPX)
//{
//    oPX.row(0).copyTo(m_pX->row(4*ii));
//    oPX.row(1).copyTo(m_pX->row(4*ii+1));
//    oPX.row(2).copyTo(m_pX->row(4*ii+2));
//    oPX.row(3).copyTo(m_pX->row(4*ii+3));
//}

//void SWOptimalStepNonRigidICP::buildDL_UL(cv::SparseMat_<double> &oDL, cv::SparseMat_<double> &oUL, cdouble dBeta)
//{
//    // build beta * DL
//    int l_aDLSize[] = {m_l.size(), m_oSourceMesh.pointsNumber()};
//    oDL = cv::SparseMat_<double> (2, l_aDLSize);

//    int l_aULSize[] = {m_l.size(), 3};
//    oUL = cv::SparseMat_<double> (2, l_aULSize);

//    vector<double> l_a3DSourcePt, l_a3DTargetPt;

//    int ii = 0;
//    for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it, ++ii)
//    {
//        uint l_ui32SourceId = it->first;
//        uint l_ui32TargetId = it->second;
//        m_oSourceMesh.point(l_a3DSourcePt, l_ui32SourceId);
//        m_oTargetMesh.point(l_a3DTargetPt, l_ui32TargetId);

//        oDL.ref(ii, l_ui32SourceId*4)   = dBeta * l_a3DSourcePt[0];
//        oDL.ref(ii, l_ui32SourceId*4+1) = dBeta * l_a3DSourcePt[1];
//        oDL.ref(ii, l_ui32SourceId*4+2) = dBeta * l_a3DSourcePt[2];
//        oDL.ref(ii, l_ui32SourceId*4+3) = dBeta;

//        oUL.ref(ii, 0) = l_a3DTargetPt[0];
//        oUL.ref(ii, 1) = l_a3DTargetPt[1];
//        oUL.ref(ii, 2) = l_a3DTargetPt[2];
//    }
//}

//void SWOptimalStepNonRigidICP::buildAWithLandMarks(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWD, cv::SparseMat_<double> &oDL, cv::SparseMat_<double> &oA)
//{
//    // build A sparse matrix
//    //      | MG1 ... MGn | with MGi i-col of MG, and WDi i-col of WD
//    // A =  | WD1 ... WDn |
//    //      | beta*DL1... beta*DLl|
//    int l_aASize[] = { oMG.size(0) + oWD.size(0) + oDL.size(0), oWD.size(1)};
//    oA = cv::SparseMat_<double>(2, l_aASize);

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
//            else if(ii < oMG.size(0) + oWD.size(0))
//            {
//                if(oWD.ptr(ii - oMG.size(0),jj, false))
//                {
//                    oA.ref(ii, jj) = oWD.ref(ii - oMG.size(0),jj);
//                }
//            }
//            else
//            {
//                if(oDL.ptr(ii - oMG.size(0) - oWD.size(0),jj, false))
//                {
//                    oA.ref(ii, jj) = oDL.ref(ii - oMG.size(0) - oWD.size(0),jj);
//                }
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::buildBWithLandMarks(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWU, cv::SparseMat_<double> &oUL, cv::SparseMat_<double> &oB)
//{
//    // build B sparse matrix
//    //      | c01 ... c03 | with c0i i-col of 0 (same number of rows than MG), and WUi i-col of WU
//    // B =  | WU1 ... WU3 |
//    //      | UL1 ... UL3 |
//    int l_aBSize[] = { oMG.size(0) + oWU.size(0) + oUL.size(0), oWU.size(1)};
//    oB = cv::SparseMat_<double>(2, l_aBSize);

//    for(int ii = 0; ii < oWU.size(0) + oUL.size(0); ++ii)
//    {
//        for(int jj = 0; jj < l_aBSize[1]; ++jj)
//        {
//            if(ii < oWU.size(0))
//            {
//                if(oWU.ptr(ii,jj, false))
//                {
//                    oB.ref(ii + oMG.size(0), jj) = oWU.ref(ii,jj);
//                }
//            }
//            else
//            {
//                if(oUL.ptr(ii-oWU.size(0),jj, false))
//                {
//                    oB.ref(ii + oMG.size(0), jj) = oUL.ref(ii-oWU.size(0),jj);
//                }
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::buildTA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA)
//{
//    // build transposate A sparse matrix
//    int l_aTASize[] = {oA.size(1), oA.size(0)};
//    oTA = cv::SparseMat_<double>(2, l_aTASize);

//    for(int ii = 0; ii < oA.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oA.size(1); ++jj)
//        {
//            if(oA.ptr(ii,jj, false))
//            {
//                oTA.ref(jj,ii) = oA.ref(ii,jj);
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::buildTAA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA, cv::SparseMat_<double> &oTAA)
//{
//    // build TA * A
//    int l_aTAASize[] = {oTA.size(0), oTA.size(0)};
//    oTAA = cv::SparseMat_<double>(2, l_aTAASize);

//    for(int ii = 0; ii < oTA.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oTA.size(1); ++jj)
//        {
//            if(oTA.ptr(ii,jj, false))
//            {
//                for(int kk = 0; kk < oA.size(1); ++kk)
//                {
//                    if(oA.ptr(jj,kk, false))
//                    {
//                        oTAA.ref(ii,kk) += oTA.ref(ii,jj) * oA.ref(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}



//double SWOptimalStepNonRigidICP::distanceEnergy() const
//{
//    double l_dEnergy = 0.0;

////    vector<double> l_a3DPoint;

////    for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
////    {
////        m_oSourceMesh.point(l_a3DPoint, ii);
////        l_dEnergy += m_w[ii] * squareMinDistTargetFromMesh(cv::Mat(l_a3DPoint), 30);
////    }

////    cout << "l_dEnergy first method : " << l_dEnergy << endl;
////    l_dEnergy = 0;

//    vector<double> l_a3DPointSource;
//    vector<double> l_a3DPointTarget;

//    for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//    {
//        m_oSourceMesh.point(l_a3DPointSource, ii);
//        m_oTargetMesh.point(l_a3DPointTarget, m_u[ii]);
//        double l_dDistance = swUtil::norm(swUtil::vec(l_a3DPointSource, l_a3DPointTarget));
//        l_dEnergy += m_w[ii] * l_dDistance * l_dDistance;
//    }
//    cout << "l_dEnergy second method : " << l_dEnergy << endl;

//    return l_dEnergy;
//}

//double SWOptimalStepNonRigidICP::stiffnessEnergy() const
//{
//    double l_dEnergy = 0.0;

//    cv::Mat l_oDiag = cv::Mat::eye(4,4, CV_64FC1);
//    l_oDiag.at<double>(15) = m_dGama;

//    for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//    {
//        double l_dValue = cv::norm(l_oDiag * (X(ii) - pX(ii)));
//        l_dEnergy += l_dValue *l_dValue;
//    }

//    return l_dEnergy;
//}


//double SWOptimalStepNonRigidICP::landmarksEnergy() const // TODO : test
//{
//    double l_dEnergy = 0.0;

//    vector<double> l_a3DTemplatePt, l_a3DTargetPt;

//    for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it)
//    {
//        uint l_ui32TemplateId = it->first;
//        uint l_ui32TargetId   = it->second;

//        m_oSourceMesh.point(l_a3DTemplatePt, l_ui32TemplateId);
//        m_oTargetMesh.point(l_a3DTargetPt, l_ui32TargetId);

//        l_dEnergy += swUtil::norm(swUtil::vec(l_a3DTemplatePt, l_a3DTargetPt));
//    }

//    return l_dEnergy;
//}



//double SWOptimalStepNonRigidICP::totalEnergy(cdouble dAlpha, cdouble dBeta, cbool bUseLandMarks) const
//{
//    double l_dDistanceEnergy    =          distanceEnergy(); cout << " stiffnessEnergy " << endl;
//    double l_dStiffnessEnergy   = dAlpha * stiffnessEnergy(); cout << " end stiffnessEnergy " << endl;

//    double l_dLandMarkEnergy = 0;

//    if(bUseLandMarks)
//    {
//        l_dLandMarkEnergy = dBeta * landmarksEnergy();
//    }

//    double l_dTotal = l_dDistanceEnergy + l_dStiffnessEnergy + l_dLandMarkEnergy;

//    cout << "Energies -> DE : " << l_dDistanceEnergy << " SE : " << l_dStiffnessEnergy << " LE : " << l_dLandMarkEnergy <<  " Total : " << l_dTotal << endl;

//    return l_dTotal;
//}



//double SWOptimalStepNonRigidICP::squareMinDistTargetFromMesh(const cv::Vec3d &oPt, cint i32Rand) const
//{
//    double l_dMin = DBL_MAX;

//    for(uint ii = 0; ii < m_oTargetMesh.pointsNumber(); ++ii)
//    {
//        if(rand()%i32Rand == 0)
//        {
//            float l_a9FCoords[9];
//            m_oTargetMesh.trianglePoints(l_a9FCoords,ii);
//            cv::Vec3f l_vV1(l_a9FCoords[0],l_a9FCoords[1],l_a9FCoords[2]);
//            cv::Vec3f l_vV2(l_a9FCoords[3],l_a9FCoords[4],l_a9FCoords[5]);
//            cv::Vec3f l_vV3(l_a9FCoords[6],l_a9FCoords[7],l_a9FCoords[8]);

//            double l_dCurrDist = swCloud::squareDistancePoint2Triangle(l_vV1, l_vV2, l_vV3, oPt);

//            if(l_dCurrDist < l_dMin)
//            {
//                l_dMin = l_dCurrDist;
//            }
//        }
//    }

//    return l_dMin;
//}






//void SWOptimalStepNonRigidICP::buildTAA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA, cv::Mat &oTAA)
//{
//    // build TA * A
//    int l_aTAASize = oTA.size(0);
//    oTAA = cv::Mat(l_aTAASize, l_aTAASize, CV_64FC1, cv::Scalar(0.0));

//    for(int ii = 0; ii < oTA.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oTA.size(1); ++jj)
//        {
//            if(oTA.ptr(ii,jj, false))
//            {
//                for(int kk = 0; kk < oA.size(1); ++kk)
//                {
//                    if(oA.ptr(jj,kk, false))
//                    {
//                        oTAA.at<double>(ii, kk) += oTA.ref(ii,jj) * oA.ref(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}


//void SWOptimalStepNonRigidICP::buildDL_UL(cv::SparseMat_<double> &oWD, cv::SparseMat_<double> &oDL, cv::SparseMat_<double> &oUL, cdouble dBeta)
//{
//    // build beta * DL
//    int l_aDLSize[] = {m_l.size(), oWD.size(1)};
//    oDL = cv::SparseMat_<double> (2, l_aDLSize);

//    int l_aULSize[] = {m_l.size(), 4};
//    oUL = cv::SparseMat_<double> (2, l_aULSize);

//    vector<double> l_a3DSourcePt, l_a3DTargetPt;

//    int ii = 0;
//    for (std::map<uint,uint>::const_iterator it = m_l.cbegin(); it != m_l.cend(); ++it, ++ii)
//    {
//        uint l_ui32SourceId = it->first;
//        uint l_ui32TargetId = it->second;
//        m_oTargetMesh.point(l_a3DSourcePt, l_ui32SourceId);
//        m_oTargetMesh.point(l_a3DTargetPt, l_ui32TargetId);

//        oDL.ref(ii, l_ui32SourceId*4)   = dBeta * l_a3DSourcePt[0];
//        oDL.ref(ii, l_ui32SourceId*4+1) = dBeta * l_a3DSourcePt[1];
//        oDL.ref(ii, l_ui32SourceId*4+2) = dBeta * l_a3DSourcePt[2];
//        oDL.ref(ii, l_ui32SourceId*4+2) = dBeta;

//        oUL.ref(ii, 0) = l_a3DTargetPt[0];
//        oUL.ref(ii, 1) = l_a3DTargetPt[1];
//        oUL.ref(ii, 2) = l_a3DTargetPt[2];
//        oUL.ref(ii, 3) = 1;
//    }
//}

//void SWOptimalStepNonRigidICP::buildWU(cv::SparseMat_<double> &oW, cv::Mat &oU, cv::SparseMat_<double> &oWU)
//{
//    // build W * U
//    int l_aWUSize[] = {oW.size(0), oU.cols};
//    oWU = cv::SparseMat_<double> (2, l_aWUSize);

//    for(int ii = 0; ii < oW.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oW.size(1); ++jj)
//        {
//            if(oW.ptr(ii,jj, false))
//            {
//                for(int kk = 0; kk < oU.cols; ++kk)
//                {
//                    if(oU.ptr(jj,kk, false))
//                    {
//                        oWU.ref(ii, kk) += oW.ref(ii,jj) * oU.at<double>(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}


//void SWOptimalStepNonRigidICP::buildW(cv::SparseMat_<double> &oW)
//{
//    // compute W matrix,  W = diag(w1, ... , wn)
//        int l_aWSize[] = {m_oSourceMesh.pointsNumber(), m_oSourceMesh.pointsNumber()};
//        oW = cv::SparseMat_<double>(2, l_aWSize);

//        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//        {
//            if(m_w[ii] > 0)
//            {
//                oW.ref(ii,ii) = m_w[ii];
//            }
//        }
//}

//void SWOptimalStepNonRigidICP::buildD(cv::SparseMat_<double> &oD)
//{
//    // compute D matrix
//    // D = |v1^t             |  vi   = [xi, yi, zi, 1]^T
//    //     |    v2^t         |  vi^t = [xi, yi, zi, 1]
//    //     |         ...     |
//    //     |             vn^t|
//    int l_aDSize[] = {m_oSourceMesh.pointsNumber(),4 * m_oSourceMesh.pointsNumber()};
//    oD = cv::SparseMat_<double>(2, l_aDSize);

//    for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//    {
//        float l_aFXYZ[3];
//        m_oSourceMesh.point(l_aFXYZ, ii);

//        oD.ref(ii, ii * 4)      = l_aFXYZ[0];
//        oD.ref(ii, ii * 4 + 1)  = l_aFXYZ[1];
//        oD.ref(ii, ii * 4 + 2)  = l_aFXYZ[2];
//        oD.ref(ii, ii * 4 + 3)  = 1.0;
//    }
//}





//void SWOptimalStepNonRigidICP::buildWD(cv::SparseMat_<double> &oW, cv::SparseMat_<double> &oD, cv::SparseMat_<double> &oWD)
//    {
// build W * D
//    int l_aWDSize[] = {oW.size(0), oD.size(1)};
//    oWD = cv::SparseMat_<double> (2, l_aWDSize);

//    for(int ii = 0; ii < oW.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oW.size(1); ++jj)
//        {
//            if(oW.ptr(ii,jj, false))
//            {
//                for(int kk = 0; kk < oD.size(1); ++kk)
//                {
//                    if(oD.ptr(jj,kk, false))
//                    {
//                        oWD.ref(ii, kk) += oW.ref(ii,jj) * oD.ref(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}


//void SWOptimalStepNonRigidICP::buildW(cv::Mat &oW)
//{
//    // compute W matrix,  W = diag(w1, ... , wn)
//        oW = cv::Mat(m_oSourceMesh.pointsNumber(), m_oSourceMesh.pointsNumber(), CV_64FC1, cv::Scalar(0.0));
//        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//        {
//            oW.at<double>(ii,ii) = m_w[ii];
//        }
//}



//void SWOptimalStepNonRigidICP::buildD(cv::Mat &oD)
//{
//    // compute D matrix
//    // D = |v1^t             |  vi = [xi, yi, zi, 1]^T
//    //     |    v2^t         |
//    //     |         ...     |
//    //     |             vn^t|
//        oD = cv::Mat(m_oSourceMesh.pointsNumber(), 4 * m_oSourceMesh.pointsNumber(), CV_64FC1, cv::Scalar(0.0));

//        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//        {
//            vector<double> l_oPt;
//            m_oSourceMesh.point(l_oPt, ii);

//            oD.at<double>(ii, ii * 4)        = l_oPt[0];
//            oD.at<double>(ii, ii * 4 + 1)    = l_oPt[1];
//            oD.at<double>(ii, ii * 4 + 2)    = l_oPt[2];
//            oD.at<double>(ii, ii * 4 + 3)    = 1.0;
//        }
//}






//void SWOptimalStepNonRigidICP::buildU(cv::Mat &oU)
//{
//    // compute U matrix, U = [u1, ..., un]^T
//        oU = cv::Mat(m_oSourceMesh.pointsNumber(), 3, CV_64FC1);
//        for(uint ii = 0; ii < m_oSourceMesh.pointsNumber(); ++ii)
//        {
//            vector<double> l_oPt;
//            m_oTargetMesh.point(l_oPt, m_u[ii]);

//            oU.at<double>(ii, 0) = l_oPt[0];
//            oU.at<double>(ii, 1) = l_oPt[1];
//            oU.at<double>(ii, 2) = l_oPt[2];
//        }
//}









//void SWOptimalStepNonRigidICP::buildA(cv::SparseMat_<double> &oMG, cv::Mat &oWD, cv::SparseMat_<double> &oA)
//{
//    // build A sparse matrix
//    //      | MG1 ... MGn | with MGi i-col of MG, and WDi i-col of WD
//    // A =  | WD1 ... WDn |
//    int l_aASize[] = { oMG.size(0) + oWD.rows, oWD.cols};
//    oA = cv::SparseMat_<double>(2, l_aASize);

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
//                if(oWD.at<double>(ii - oMG.size(0),jj) != 0.0)
//                {
//                    oA.ref(ii, jj) = oWD.at<double>(ii - oMG.size(0),jj);
//                }
//            }
//        }
//    }
//}









//void SWOptimalStepNonRigidICP::mult(cv::SparseMat_<double> &oM, cv::SparseMat_<double> &oN, cv::SparseMat_<double> &oMN)
//{
//    if(oM.size(1) != oN.size(0)) // TODO : throw
//    {
//        cerr << "Error SWOptimalStepNonRigidICP mult, input matrix have invalid size. " << endl;
//        return;
//    }

//    // build M * N
//    int l_aMNSize[] = {oM.size(0), oN.size(1)};
//    oMN = cv::SparseMat_<double>(2, l_aMNSize);

//    for(int ii = 0; ii < oM.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oM.size(1); ++jj)
//        {
//            if(oM.ptr(ii,jj, false))
//            {
//                for(int kk = 0; kk < oN.size(1); ++kk)
//                {
//                    if(oN.ptr(jj,kk, false))
//                    {
//                        oMN.ref(ii,kk) += oM(ii,jj) * oN(jj,kk);
//                    }
//                }
//            }
//        }
//    }
//}

//void SWOptimalStepNonRigidICP::trans(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA)
//{
//    // build transposate A sparse matrix
//    int l_aTASize[] = {oA.size(1), oA.size(0)};
//    oTA = cv::SparseMat_<double>(2, l_aTASize);

//    for(int ii = 0; ii < oA.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oA.size(1); ++jj)
//        {
//            if(oA.ptr(ii,jj, false))
//            {
//                oTA.ref(jj,ii) = oA(ii,jj);
//            }
//        }
//    }
//}




//vector<int> l_vI32CorrStasmTarget;

//if(sPathSourceStasmCorr.size() > 0 && sPathTargetStasmCorr.size() > 0)
//{
//    ifstream  l_fsSource(sPathSourceStasmCorr);
//    ifstream  l_fsTarget(sPathTargetStasmCorr);

//    bool l_bContinue = true;
//    while(l_bContinue)
//    {
//        string l_sType, l_sLine;
//        l_fsSource >> l_sType;

//        if(l_sType.size() == 0)
//        {
//            l_bContinue = false;
//            break;
//        }

//        if(l_sType.at(0) == '#')
//        {
//            getline(l_fsSource, l_sLine);
//        }
//        else
//        {
//            int l_i32IndexCorr;
//            l_fsSource >> l_i32IndexCorr;
//            l_vI32CorrStasmSource.push_back(l_i32IndexCorr);
//            l_sType = "";
//            l_fsSource >> l_sType;

//             if(l_sType.at(0) == '#')
//             {
//                 getline(l_fsSource, l_sLine);
//             }
//        }

//        l_sType = "";
//        l_fsTarget >> l_sType;

//        if(l_sType.at(0) == '#')
//        {
//            getline(l_fsTarget, l_sLine);
//        }
//        else
//        {
//            int l_i32IndexCorr;
//            l_fsTarget >> l_i32IndexCorr;
//            l_vI32CorrStasmTarget.push_back(l_i32IndexCorr);
//        }
//    }
//}



//    m_oProgramTime = clock();
//        buildW(W);
////        displayDenseMatrix(W);
//    cout << "Build W : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

//        displaySparseMatrix(sW);
//    cout << "Build sW : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();
//        buildD(D);
////        displayDenseMatrix(D);
//    cout << "Build D : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

////        displaySparseMatrix(sD);
//    cout << "Build sD : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

//    cout << "Build U : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

//    cout << "Build sMG : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

//        displaySparseMatrix(sDL);

//    cout << "Build DL_UL : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;
//        displaySparseMatrix(sUL);


//    m_oProgramTime = clock();
//        matrixMultiplication(W, D, WD);
//        // WD = W * D;
////        displayDenseMatrix(WD);
//    cout << "Build WD : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();
//        buildWD(sW, sD, sWD);

//        swUtil::swCuda::blockMatrixMultiplication(sW, sD, sWD, 4);
//        displaySparseMatrix(sWD);
//    cout << "Build sWD : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

////    m_oProgramTime = clock();
////        matrixMultiplication(W, U, WU);
//////        WU = W * U;
//////        displayDenseMatrix(WU);
////    cout << "Build WU : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

////        displaySparseMatrix(sWU);
//    cout << "Build sWU : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

////    m_oProgramTime = clock();
////        addLandmarkData(sWD, sWU);
////    cout << "Add landmark data : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();
////        buildA(sMG, WD, sA);
////        buildA(sMG, sWD, sA);

////        displaySparseMatrix(sA);
//    cout << "Build sA : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();
////        buildB(sMG, sWU, sB);

////        displaySparseMatrix(sB);
//    cout << "Build sB : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

////        displaySparseMatrix(sTA);
//    cout << "Build sTA : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

////     m_oProgramTime = clock();
//        buildTAA(sA, sTA, TAA);
//        displayDenseMatrix(TAA);
//     cout << "Build TAA : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//     m_oProgramTime = clock();
//        swUtil::swCuda::matrixMultiplication(sA, sTA, sTAA);
//     cout << "Build block mult sTAA : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//     m_oProgramTime = clock();

////        displaySparseMatrix(sTAA);
//     cout << "Build sTAA : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;



//        cout << "Build TAB : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

////    m_oProgramTime = clock();
////        matrixInversion(TAA, TAAInv);
////        TAAInv = TAA.inv();
////    cout << "Build TAAInv : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();

//    cout << "Build sTAAInv : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;

//    m_oProgramTime = clock();
////        cv::Mat newX = (TAA.inv() * TAB).t();


//    cout << "Build newX : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << endl;
