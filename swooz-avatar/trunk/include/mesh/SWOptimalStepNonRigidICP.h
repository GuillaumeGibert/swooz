
/**
 * \file SWOptimalStepNonRigidICP.h
 * \brief ...
 * \author Florian Lance
 * \date 19/09/13
 */


#ifndef _SWOPTIMALSTEPNONRIGIDICP_
#define _SWOPTIMALSTEPNONRIGIDICP_

#include "cloud/SWAlignClouds.h"
#include "mesh/SWMesh.h"

#include "opencv2/imgproc/imgproc.hpp"

namespace swMesh
{
    class SWOptimalStepNonRigidICP
    {
        public :


            SWOptimalStepNonRigidICP(const SWMesh &oSource, const SWMesh &oTarget,
                                     const std::string &sPathSourceStasmCorr = "", const std::string &sPathTargetStasmCorr = "");

            ~SWOptimalStepNonRigidICP();

//            double m_dAngleMax; /**< ... */
            float m_fAngleMax;

            std::vector<uint>   m_u; /**< array of target index vertices corresponding to the source vertices */
            std::map<uint,uint> m_l; /**< landmark correspondance */
            std::vector<float> m_w; /**< distance weights array */
            std::vector<float> m_w1; /**< debug 1 distance weights array */
            std::vector<float> m_w2; /**< debug 2 distance weights array */
            std::vector<float> m_w3; /**< debug 3 distance weights array */


            SWMesh m_oSourceMesh;   /**< source mesh */
            SWMesh m_oTargetMesh;   /**< target mesh */
            SWMesh m_oOriginalTargetMesh;

            void updateSourceMeshNormals();
            void computeDistanceWeights();
            void computeCorrespondences();
            void associateTextureCoordinates();

            float resolve(cfloat fAlpha, cfloat fBeta, cfloat fGama, cbool bUseLandMarks);

            float totalEnergy() const;

            void updateLandmarksWithSTASM();
            void updateLandmarksWithManualSelection(std::vector<int> &idSource, std::vector<int> &idTarget);

     private :

            cv::Mat *m_X;  /**< current X */
            cv::Mat *m_pX; /**< previous X */
            float m_fWeightVectorDistMax;   /**< ... */
            float m_fLastComputedCost;     /**< last computed result of cost function */
            cv::Mat X(cuint ii) const;
            cv::Mat pX(cuint ii) const;
//            void displaySparseMatrix(cv::SparseMat_<float> &oSparseMat);
            void displayDenseMatrix(cv::Mat &oMat);
            void readStasmCorrFile(const std::string &oPathCorrStasmFile, std::vector<int> &vI32CorrStasmTarget);
//            void buildMG(cv::SparseMat_<float> &oMG, cfloat fAlpha, cfloat fGama);
//            void buildWD(cv::SparseMat_<float> &oWD);
//            void buildWU(cv::SparseMat_<float> &oWU);
//            void addLandMarks(cfloat fBeta, cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oA, cv::SparseMat_<float> &oB);
//            void buildDL_UL(cv::SparseMat_<float> &oDL, cv::SparseMat_<float> &oUL, cfloat fBeta);
//            void buildA(cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oWD, cv::SparseMat_<float> &oA);
//            void buildAWithLandMarks(cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oWD, cv::SparseMat_<float> &oDL, cv::SparseMat_<float> &oA);
//            void buildB(cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oWU, cv::SparseMat_<float> &oB);
//            void buildBWithLandMarks(cv::SparseMat_<float> &oMG, cv::SparseMat_<float> &oWU, cv::SparseMat_<float> &oUL, cv::SparseMat_<float> &oB);
//            void buildAX(cv::SparseMat_<float> &oA, cv::Mat &oX, cv::Mat &oAX);
//            void buildTAA(cv::SparseMat_<float> &oA, cv::SparseMat_<float> &oTAA);
//            void buildTAB(cv::SparseMat_<float> &oA, cv::SparseMat_<float> &oB, cv::Mat &oTAB);
            float computeDiff(cv::Mat &newX);

            float m_fMaxTemplateTargetDistance;
            void buildU(cv::Mat &oU);
            void buildD(cv::Mat &oD);
            void buildW(cv::Mat &oW);
            void buildMG(cv::Mat &oMG, cfloat fAlpha, cfloat fGama);
            void buildWD(cv::Mat &oWD);
            void buildWU(cv::Mat &oWU);
//            void buildA(cv::Mat &oMG, cv::Mat &oWD, cv::Mat &oA);
            void buildA(cv::Mat &oMG_A, cv::Mat &oWD);
//            void buildB(cv::Mat &oMG, cv::Mat &oWU, cv::Mat &oB);
            void buildB(cint i32MGRows, cv::Mat &oB);
            void buildTAA(cv::Mat &oA, cv::Mat &oTAA);
            void buildTAB(cv::Mat &oA, cv::Mat &oB, cv::Mat &oTAB);
//            void addLandMarks(cfloat fBeta, cv::Mat &oMG, cv::Mat &oA, cv::Mat &oB);
            void addLandMarks(cfloat fBeta, cint i32MGRows, cv::Mat &oA, cv::Mat &oB);

            bool checkDiff(cv::Mat &oMatrix1, cv::Mat &oMatrix2);
//            bool checkDiff(cv::Mat &oMatrix, cv::SparseMat_<float> &oSparseMatrix);

            std::string m_sPathSourceStasmCorr;
            std::string m_sPathTargetStasmCorr;
    };
}

#endif



