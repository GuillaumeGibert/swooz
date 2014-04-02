
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

            double m_dAngleMax; /**< ... */

            std::vector<uint>   m_u; /**< array of target index vertices corresponding to the source vertices */
            std::map<uint,uint> m_l; /**< landmark correspondance */
            std::vector<double> m_w; /**< distance weights array */
            std::vector<double> m_w1; /**< debug 1 distance weights array */
            std::vector<double> m_w2; /**< debug 2 distance weights array */
            std::vector<double> m_w3; /**< debug 3 distance weights array */

            SWMesh m_oSourceMesh;   /**< source mesh */
            SWMesh m_oTargetMesh;   /**< target mesh */
            SWMesh m_oOriginalTargetMesh;

            // working copy
            std::vector<uint>   m_uC;   /**< working copy of m_u */
            std::vector<double> m_wC;   /**< working copy of m_w*/
            SWMesh *m_pSourceMeshC;     /**< working copy of m_oSourceMesh */
            SWMesh *m_pTargetMeshC;     /**< working copy of m_oTargetMesh */

            void updateSourceMeshNormals();
            void computeDistanceWeights();
            void computeCorrespondences();
            void copyDataForResolving();

//            void initResolve();

            double resolve(cdouble dAlpha, cdouble dBeta, cdouble dGama, cbool bUseLandMarks);

            void updateSourceMeshWithMorphModification();

            double totalEnergy() const;

     private :

            cv::Mat *m_X;  /**< current X */
            cv::Mat *m_pX; /**< previous X */

            float m_fWeightVectorDistMax;   /**< ... */
            double m_dLastComputedCost;     /**< last computed result of cost function */

            cv::Mat X(cuint ii) const;
            cv::Mat pX(cuint ii) const;

            void displaySparseMatrix(cv::SparseMat_<double> &oSparseMat);
            void displayDenseMatrix(cv::Mat &oMat);
            void readStasmCorrFile(const std::string &oPathCorrStasmFile, std::vector<int> &vI32CorrStasmTarget);

            void buildMG(cv::SparseMat_<double> &oMG, cdouble dAlpha, cdouble dGama);
            void buildWD(cv::SparseMat_<double> &oWD);
            void buildWU(cv::SparseMat_<double> &oWU);

            void addLandMarks(cdouble dBeta, cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oB);
            void buildDL_UL(cv::SparseMat_<double> &oDL, cv::SparseMat_<double> &oUL, cdouble dBeta);

            void buildA(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWD, cv::SparseMat_<double> &oA);
            void buildAWithLandMarks(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWD, cv::SparseMat_<double> &oDL, cv::SparseMat_<double> &oA);

            void buildB(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWU, cv::SparseMat_<double> &oB);
            void buildBWithLandMarks(cv::SparseMat_<double> &oMG, cv::SparseMat_<double> &oWU, cv::SparseMat_<double> &oUL, cv::SparseMat_<double> &oB);

            void buildAX(cv::SparseMat_<double> &oA, cv::Mat &oX, cv::Mat &oAX);

            void buildTAA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTAA);
            void buildTAB(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oB, cv::Mat &oTAB);

            double computeDiff(cv::Mat &newX);

    };
}

#endif



//            void buildTA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA);
//            void buildTAA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA, cv::SparseMat_<double> &oTAA);
//            void setX(cuint ii, cv::Mat &oX);
//            void setPX(cuint ii, cv::Mat &oPX);
//
//            bool m_bUseLandMarks;   /**< landmarks points used ? */
//            void buildTAA(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA, cv::Mat &oTAA);
//            void mult(cv::SparseMat_<double> &oM, cv::SparseMat_<double> &oN, cv::SparseMat_<double> &oMN);
//            void trans(cv::SparseMat_<double> &oA, cv::SparseMat_<double> &oTA);
//            swCloud::SWAlignClouds m_oAlignClouds;

//            int m_i32IdCloseSource;
//            int m_i32IdCloseTarget;

//            double m_dDiff;
//            double m_dBeta;     /**< landmark weight */

//            double m_dGama;     /**< ... */
//            double squareMinDistTargetFromMesh(const cv::Vec3d &oPt, cint i32Rand = 30) const;
//            double distanceEnergy() const;
//            double stiffnessEnergy() const;
//            double landmarksEnergy() const;
//            double totalEnergy(cdouble dAlpha, cdouble dBeta, cbool bUseLandMarks) const;

//            void buildW(cv::Mat &oW);
//            void buildW(cv::SparseMat_<double> &oW);

//            void buildD(cv::Mat &oD);
//            void buildD(cv::SparseMat_<double> &oD);

//            void buildU(cv::Mat &oU);



//            void buildWD(cv::SparseMat_<double> &oW, cv::SparseMat_<double> &oD, cv::SparseMat_<double> &oWD);


//            void buildWU(cv::SparseMat_<double> &oW, cv::Mat &oU, cv::SparseMat_<double> &oWU);

//            void buildDL_UL(cv::SparseMat_<double> &oWD, cv::SparseMat_<double> &oDL, cv::SparseMat_<double> &oUL, cdouble dBeta);

//            void addLandmarkData(cv::SparseMat_<double> &oWD, cv::SparseMat_<double> &oWU);

//            void buildA(cv::SparseMat_<double> &oMG, cv::Mat &oWD, cv::SparseMat_<double> &oA);



