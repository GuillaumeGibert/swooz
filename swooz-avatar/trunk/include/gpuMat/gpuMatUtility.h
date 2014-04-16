

#ifndef _GPUMATUTILITY_
#define _GPUMATUTILITY_

#include "commonTypes.h"

// CUDA
#include "gpuMat/configCuda.h"

// OPENCV
#include "opencv2/imgproc/imgproc.hpp"


int doCulaSgesv(float *aFInputMat, float *aFOutputInvMat, int i32SizeSquareMat);
//int doCulaSgesv(float *aFInputMat, float *aFOutputInvMat, int i32N, int i32NRHS);

void transpose(float *idata, float *odata, int size_x, int size_y);

void matMult(const Matrix A, const Matrix B, Matrix C, const int blockSize = 16);

void cudaDummyCall();

int LUDecomposition(float *aFMat, int i32SizeSquareMat);

namespace swUtil
{
    namespace swCuda
    {
        /**
         * \brief GPU matrix inversion.
         * \param [in]  oInput   : float input square matrix
         * \param [out] oResult  : output inverse matrix
         */
        static void matrixInversion(const cv::Mat &oInput, cv::Mat &oResult)
        {
//            cudaDummyCall();

            float *l_aFDataIn  = new float[oInput.rows * oInput.cols];
            float *l_aFDataOut = new float[oInput.rows * oInput.cols];

            int l_i32CurrentIndexDiag = 0;

            // input input/output array datas
            for(int ii = 0; ii < oInput.rows * oInput.cols; ++ii)
            {
                l_aFDataIn[ii] = (oInput.at<float>(ii));

                if(ii == l_i32CurrentIndexDiag) // init output array with id matrix
                {
                    l_aFDataOut[ii] = 1;
                    l_i32CurrentIndexDiag += oInput.rows + 1;
                }
                else
                {
                    l_aFDataOut[ii] = 0;
                }
            }

            // compute inverse mat            
            doCulaSgesv(l_aFDataIn, l_aFDataOut, oInput.rows);
            delete[] l_aFDataIn;

            // fill result mat
            oResult = cv::Mat(oInput.rows,oInput.rows, CV_32FC1);
            for(int ii = 0; ii < oInput.rows*oInput.rows; ++ii)
            {
                oResult.at<float>(ii) = (l_aFDataOut[ii]);
            }

            delete[] l_aFDataOut;
        }



        /**
         * \brief GPU matrix multiplication. Res(l,n) = A(l,m)*B(m,n)
         * \param [in]  oMatA    : input A matrix
         * \param [in]  oMatB    : input B matrix
         * \param [out] oMatRes  : res C matrix
         */
        static void matrixMultiplication(const cv::Mat &oMatA, const cv::Mat &oMatB, cv::Mat &oMatRes)
        {
            int l_i32BlockSize = 16;

//            printf("start matrix multiplication\n");
            // Padd matrix offset
            int l_i32PaddOffsetRowsA = (l_i32BlockSize - (oMatA.rows % l_i32BlockSize))% l_i32BlockSize;
            int l_i32PaddOffsetColsA = (l_i32BlockSize - (oMatA.cols % l_i32BlockSize))% l_i32BlockSize;
            int l_i32PaddOffsetRowsB = (l_i32BlockSize - (oMatB.rows % l_i32BlockSize))% l_i32BlockSize;
            int l_i32PaddOffsetColsB = (l_i32BlockSize - (oMatB.cols % l_i32BlockSize))% l_i32BlockSize;

            Matrix A, B, C;
            A.height = oMatA.rows + l_i32PaddOffsetRowsA;
            A.width  = oMatA.cols + l_i32PaddOffsetColsA;
            A.elements = new float[A.width * A.height];

//            printf("allocation A\n");

            B.height = oMatB.rows + l_i32PaddOffsetRowsB;
            B.width  = oMatB.cols + l_i32PaddOffsetColsB;
            B.elements = new float[B.width * B.height];

//            printf("allocation B\n");

            C.height = A.height;
            C.width  = B.width;
            C.elements = new float[C.width * C.height];

//            printf("allocation C\n");

            bool l_bA32 = false, l_bA64 = false;
            if(oMatA.depth() == CV_32F)
            {
                l_bA32 = true;
            }
            else if(oMatA.depth() == CV_64F)
            {
                l_bA64 = true;
            }

            for(int ii = 0; ii < A.height; ++ii)
            {
                for(int jj = 0; jj < A.width; ++jj)
                {
                    if(ii < oMatA.rows && jj < oMatA.cols)
                    {
                        if(l_bA32)
                        {
                            A.elements[ii*A.width + jj] = oMatA.at<float>(ii,jj);
                        }
                        else if(l_bA64)
                        {
                            A.elements[ii*A.width + jj] = static_cast<float>(oMatA.at<double>(ii,jj));
                        }
                        else
                        {
                            A.elements[ii*A.width + jj] = static_cast<float>(oMatA.at<int>(ii,jj));
                        }
                    }
                    else
                    {
                        A.elements[ii*A.width + jj] = 0.f;
                    }
                }
            }

            bool l_bB32 = false, l_bB64 = false;

            if(oMatB.depth() == CV_32F)
            {
                l_bB32 = true;
            }
            else if(oMatB.depth() == CV_64F)
            {
                l_bB64 = true;
            }

            for(int ii = 0; ii < B.height; ++ii)
            {
                for(int jj = 0; jj < B.width; ++jj)
                {
                    if(ii < oMatB.rows && jj < oMatB.cols)
                    {
                        if(l_bB32)
                        {
                            B.elements[ii*B.width + jj] = oMatB.at<float>(ii,jj);
                        }
                        else if(l_bB64)
                        {
                            B.elements[ii*B.width + jj] = static_cast<float>(oMatB.at<double>(ii,jj));
                        }
                        else
                        {
                            B.elements[ii*B.width + jj] = static_cast<float>(oMatB.at<int>(ii,jj));
                        }
                    }
                    else
                    {
                        B.elements[ii*B.width + jj] = 0.f;
                    }
                }
            }

            matMult(A, B, C, l_i32BlockSize);

            delete[] A.elements;
            delete[] B.elements;

            int l_i32ResHeight = C.height- l_i32PaddOffsetRowsA;
            int l_i32ResWidth  = C.width - l_i32PaddOffsetColsB;

            bool l_bC32 = false, l_bC64 = false;

            if(l_bA64 || l_bB64)
            {
                oMatRes = cv::Mat(l_i32ResHeight, l_i32ResWidth, CV_64FC1);
                l_bC64 = true;
            }
            else if(l_bA32 || l_bB32)
            {
                oMatRes = cv::Mat(l_i32ResHeight, l_i32ResWidth, CV_32FC1);
                l_bC32 = true;
            }
            else
            {
                oMatRes = cv::Mat(l_i32ResHeight, l_i32ResWidth, CV_32SC1);
            }

            for(int ii = 0; ii < oMatRes.rows; ++ii)
            {
                for(int jj = 0; jj < oMatRes.cols; ++jj)
                {
                    if(l_bC32)
                    {
                        oMatRes.at<float>(ii,jj) = C.elements[ii*C.width + jj];
                    }
                    else if(l_bC64)
                    {
                        oMatRes.at<double>(ii,jj) = static_cast<double>(C.elements[ii*C.width + jj]);
                    }
                    else
                    {
                        oMatRes.at<int>(ii,jj) = static_cast<int>(C.elements[ii*C.width + jj]);
                    }
                }
            }

            delete[] C.elements;
        }




        static void block(cv::Mat &oMat, float *aFBlock, cuint ui32X, cuint ui32Y, cuint ui32HeightBlock, cuint ui32WidthBlock)
        {
            for(uint ii = 0; ii < ui32HeightBlock; ++ii)
            {
                for(uint jj = 0; jj < ui32WidthBlock; ++jj)
                {
                    int l_i32MatII = ui32X * ui32HeightBlock + ii;
                    int l_i32MatJJ = ui32Y * ui32WidthBlock  + jj;

                    if(l_i32MatII < oMat.rows && l_i32MatJJ < oMat.cols)
                    {
//                        if(oMat.ptr(l_i32MatII, l_i32MatJJ, false))
//                        {
                            aFBlock[ii * ui32WidthBlock + jj] = oMat.at<float>(l_i32MatII, l_i32MatJJ);
//                        }
//                        else
//                        {
//                            aFBlock[ii * ui32WidthBlock + jj] = 0.f;
//                        }
                    }
                    else // case where the block contains a padded part of the matrix
                    {
                        aFBlock[ii * ui32WidthBlock + jj] = 0.f;
                    }
                }
            }
        }

        static void updateMatWithBlock(cv::Mat &oMat, float *aFBlock, cuint ui32X, cuint ui32Y, cuint ui32HeightBlock, cuint ui32WidthBlock)
        {
             for(uint ii = 0; ii < ui32HeightBlock; ++ii)
             {
                 for(uint jj = 0; jj < ui32WidthBlock; ++jj)
                 {
                     int l_i32MatII = ui32X * ui32HeightBlock + ii;
                     int l_i32MatJJ = ui32Y * ui32WidthBlock  + jj;

                     if(l_i32MatII < oMat.rows && l_i32MatJJ < oMat.cols)
                     {
                         if(aFBlock[ii * ui32WidthBlock + jj] != 0.f)
                         {
                             oMat.at<float>(l_i32MatII, l_i32MatJJ) = aFBlock[ii * ui32WidthBlock + jj];
                         }
                     }
                 }
             }
        }

        static void blockMatrixMultiplication(cv::Mat &oMatA, cv::Mat &oMatB, cv::Mat &oMatRes, cint i32SizeMatBlock = 2)
        {
            int l_i32SizeMatBlock    = i32SizeMatBlock; // l_i32SizeMatBlock must be a multiple of BLOCKSIZE
            int l_i32SizeMatDivBlock = l_i32SizeMatBlock * BLOCKSIZE;

            // Padd matrix offset
                int l_i32PaddOffsetRowsA = (l_i32SizeMatDivBlock - (oMatA.rows % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;
                int l_i32PaddOffsetColsA = (l_i32SizeMatDivBlock - (oMatA.cols % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;
                int l_i32PaddOffsetRowsB = (l_i32SizeMatDivBlock - (oMatB.rows % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;
                int l_i32PaddOffsetColsB = (l_i32SizeMatDivBlock - (oMatB.cols % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;

            // Init A,B,C sizes
                Matrix A, B, C;
                A.height = oMatA.rows + l_i32PaddOffsetRowsA;
                A.width  = oMatA.cols + l_i32PaddOffsetColsA;
                B.height = oMatB.rows + l_i32PaddOffsetRowsB;
                B.width  = oMatB.cols + l_i32PaddOffsetColsB;
                C.height = A.height;
                C.width  = B.width;

            // Init subA, suB, subC
                Matrix subA, subB, subC;
                subA.height   = A.height / l_i32SizeMatBlock;
                subA.width    = A.width  / l_i32SizeMatBlock;
                subA.elements = new float[subA.height*subA.width];
                subB.height   = B.height / l_i32SizeMatBlock;
                subB.width    = B.width  / l_i32SizeMatBlock;
                subB.elements = new float[subB.height*subB.width];
                subC.height   = subA.height;
                subC.width    = subB.width;
                subC.elements = new float[subC.height*subC.width];

                float *l_fSubCCopy = new float[subC.height*subC.width];

                int l_i32ResHeight = C.height- l_i32PaddOffsetRowsA;
                int l_i32ResWidth  = C.width - l_i32PaddOffsetColsB;
//                int l_fI32Size[] = {l_i32ResHeight, l_i32ResWidth};
                oMatRes = cv::Mat(l_i32ResHeight,l_i32ResWidth, CV_32FC1);

                for(int ii = 0; ii < l_i32SizeMatBlock; ++ii) // C ii ..
                {
                    for(int jj = 0; jj < l_i32SizeMatBlock; ++jj) // C .. jj
                    {
                        std::fill_n(l_fSubCCopy, subC.height * subC.width, 0.f);

                        // compute Cij
                        for(int kk = 0; kk < l_i32SizeMatBlock; ++kk)
                        {
                            block(oMatA, subA.elements, ii, kk, subA.height, subA.width);

                            block(oMatB, subB.elements, kk, jj, subB.height, subB.width);

                            matMult(subA, subB, subC);

                            for(int ll = 0; ll < subC.height* subC.width; ++ll)
                            {
                                l_fSubCCopy[ll] += subC.elements[ll];
                            }
                        }

                        updateMatWithBlock(oMatRes, l_fSubCCopy, ii, jj, subC.height, subC.width);
                    }
                }

            delete[] subA.elements;
            delete[] subB.elements;
            delete[] subC.elements;
            delete[] l_fSubCCopy;
        }


        /**
         * \brief ...
         * \param [in]      oMat            : ...
         * \param [in,out]  aFBlock         : ...
         * \param [in]      ui32X           : ...
         * \param [in]      ui32Y           : ...
         * \param [in]      ui32HeightBlock : ...
         * \param [in]      ui32WidthBlock  : ...
         */
        static void block(cv::SparseMat_<float> &oMat, float *aFBlock, cuint ui32X, cuint ui32Y, cuint ui32HeightBlock, cuint ui32WidthBlock)
        {
            for(uint ii = 0; ii < ui32HeightBlock; ++ii)
            {
                for(uint jj = 0; jj < ui32WidthBlock; ++jj)
                {
                    int l_i32MatII = ui32X * ui32HeightBlock + ii;
                    int l_i32MatJJ = ui32Y * ui32WidthBlock  + jj;

                    if(l_i32MatII < oMat.size(0) && l_i32MatJJ < oMat.size(1))
                    {
                        if(oMat.ptr(l_i32MatII, l_i32MatJJ, false))
                        {
                            aFBlock[ii * ui32WidthBlock + jj] = oMat(l_i32MatII, l_i32MatJJ);
                        }
                        else
                        {
                            aFBlock[ii * ui32WidthBlock + jj] = 0.f;
                        }
                    }
                    else // case where the block contains a padded part of the matrix
                    {
                        aFBlock[ii * ui32WidthBlock + jj] = 0.f;
                    }
                }
            }
        }

        /**
         * \brief ...
         * \param [in]      oMat            : ...
         * \param [in,out]  aFBlock         : ...
         * \param [in]      ui32X           : ...
         * \param [in]      ui32Y           : ...
         * \param [in]      ui32HeightBlock : ...
         * \param [in]      ui32WidthBlock  : ...
         */
        static void updateMatWithBlock(cv::SparseMat_<float> &oMat, float *aFBlock, cuint ui32X, cuint ui32Y, cuint ui32HeightBlock, cuint ui32WidthBlock)
        {
             for(uint ii = 0; ii < ui32HeightBlock; ++ii)
             {
                 for(uint jj = 0; jj < ui32WidthBlock; ++jj)
                 {
                     int l_i32MatII = ui32X * ui32HeightBlock + ii;
                     int l_i32MatJJ = ui32Y * ui32WidthBlock  + jj;

                     if(l_i32MatII < oMat.size(0) && l_i32MatJJ < oMat.size(1))
                     {
                         if(aFBlock[ii * ui32WidthBlock + jj] != 0.f)
                         {
                             oMat.ref(l_i32MatII, l_i32MatJJ) = aFBlock[ii * ui32WidthBlock + jj];
                         }
                     }
                 }
             }
        }

        /**
         * \brief ...
         * \param [in]  oSMatA   : ...
         * \param [in]  oSMatB   : ...
         * \param [out] oSMatRes : ...
         * \param [in] i32SizeMatBlock : ...
         */
        static void blockMatrixMultiplication( cv::SparseMat_<float> &oSMatA,  cv::SparseMat_<float> &oSMatB,  cv::SparseMat_<float> &oSMatRes, cint i32SizeMatBlock = 2)
        {
            int l_i32SizeMatBlock    = i32SizeMatBlock; // l_i32SizeMatBlock must be a multiple of BLOCKSIZE
            int l_i32SizeMatDivBlock = l_i32SizeMatBlock * BLOCKSIZE;

            // Padd matrix offset
                int l_i32PaddOffsetRowsA = (l_i32SizeMatDivBlock - (oSMatA.size(0) % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;
                int l_i32PaddOffsetColsA = (l_i32SizeMatDivBlock - (oSMatA.size(1) % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;
                int l_i32PaddOffsetRowsB = (l_i32SizeMatDivBlock - (oSMatB.size(0) % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;
                int l_i32PaddOffsetColsB = (l_i32SizeMatDivBlock - (oSMatB.size(1) % l_i32SizeMatDivBlock)) % l_i32SizeMatDivBlock;

            // Init A,B,C sizes
                Matrix A, B, C;
                A.height = oSMatA.size(0) + l_i32PaddOffsetRowsA;
                A.width  = oSMatA.size(1) + l_i32PaddOffsetColsA;
                B.height = oSMatB.size(0) + l_i32PaddOffsetRowsB;
                B.width  = oSMatB.size(1) + l_i32PaddOffsetColsB;
                C.height = A.height;
                C.width  = B.width;

            // Init subA, suB, subC
                Matrix subA, subB, subC;
                subA.height   = A.height / l_i32SizeMatBlock;
                subA.width    = A.width  / l_i32SizeMatBlock;
                subA.elements = new float[subA.height*subA.width];
                subB.height   = B.height / l_i32SizeMatBlock;
                subB.width    = B.width  / l_i32SizeMatBlock;
                subB.elements = new float[subB.height*subB.width];
                subC.height   = subA.height;
                subC.width    = subB.width;
                subC.elements = new float[subC.height*subC.width];

            float *l_fSubCCopy = new float[subC.height*subC.width];

            int l_i32ResHeight = C.height- l_i32PaddOffsetRowsA;
            int l_i32ResWidth  = C.width - l_i32PaddOffsetColsB;
            int l_fI32Size[] = {l_i32ResHeight, l_i32ResWidth};
            oSMatRes = cv::SparseMat_<float>(2, l_fI32Size);

            for(int ii = 0; ii < l_i32SizeMatBlock; ++ii) // C ii ..
            {
                for(int jj = 0; jj < l_i32SizeMatBlock; ++jj) // C .. jj
                {
                    std::fill_n(l_fSubCCopy, subC.height * subC.width, 0.f);

                    // compute Cij
                    for(int kk = 0; kk < l_i32SizeMatBlock; ++kk)
                    {
                        block(oSMatA, subA.elements, ii, kk, subA.height, subA.width);

                        block(oSMatB, subB.elements, kk, jj, subB.height, subB.width);

                        matMult(subA, subB, subC);

                        for(int ll = 0; ll < subC.height* subC.width; ++ll)
                        {
                            l_fSubCCopy[ll] += subC.elements[ll];
                        }
                    }

                    updateMatWithBlock(oSMatRes, l_fSubCCopy, ii, jj, subC.height, subC.width);
                }
            }

            delete[] subA.elements;
            delete[] subB.elements;
            delete[] subC.elements;
            delete[] l_fSubCCopy;
        }
    }
}


#endif


//        /**
//         * \brief GPU matrix inversion.
//         * \param [in]  oInput   : input square matrix
//         * \param [out] oResult  : output inverse matrix
//         */
//        static void matrixInversion(const cv::SparseMat_<float> &oInput, cv::Mat &oResult)
//        {
//            float *l_aFDataIn  = new float[oInput.size(0) * oInput.size(1)];
//            float *l_aFDataOut = new float[oInput.size(0) * oInput.size(1)];

//            int l_i32CurrentIndexDiag = 0;

//            // input input/output array datas
//            for(int ii = 0; ii < oInput.size(0) * oInput.size(1); ++ii)
//            {
//                l_aFDataIn[ii] = oInput( (ii/oInput.size(0)), (ii%oInput.size(0)));

//                if(ii == l_i32CurrentIndexDiag) // init output array with id matrix
//                {
//                    l_aFDataOut[ii] = 1;
//                    l_i32CurrentIndexDiag += oInput.size(0) + 1;
//                }
//                else
//                {
//                    l_aFDataOut[ii] = 0;
//                }
//            }

//            // compute inverse mat
//            inverseMatSgesv(l_aFDataIn, l_aFDataOut, oInput.size(0));

//            oResult = cv::Mat(oInput.size(0), oInput.size(0), CV_32FC1);
//            for(int ii = 0; ii < oInput.size(0) * oInput.size(0); ++ii)
//            {
//                oResult.at<float>(ii) = (l_aFDataOut[ii]);
//            }

//            delete[] l_aFDataIn;
//            delete[] l_aFDataOut;
//        }
/**
 * \brief GPU matrix inversion.
 * \param [in]  oInput   : float input square matrix
 * \param [out] oX  : output X matrix
 */
//        static void resolveAX_B(const cv::Mat &oInput, cv::Mat &oX)
//        {
//            float *l_aFMatLU  = new float[oInput.rows * oInput.cols];

//            // input input/output array datas
//            for(int ii = 0; ii < oInput.rows*oInput.cols; ++ii)
//            {
//                l_aFMatLU[ii] = oInput.at<float>(ii);
//            }

//            // compute inverse mat
//            LUDecomposition(l_aFMatLU, oInput.rows);

//            // fill result mat
//            oX = cv::Mat(oInput.rows,oInput.rows, CV_32FC1);
//            for(int ii = 0; ii < oX.rows; ++ii)
//            {
//                for(int jj = 0; jj < oX.cols; ++jj)
//                    oX.at<float>(ii,jj) = (l_aFMatLU[jj*oX.rows + ii]);
//            }

//            delete[] l_aFMatLU;
//        }





//        static void solve(cv::Mat &oA,cv::Mat &oL, cv::Mat &oU, cv::Mat &oLU)//, cv::Mat &oB, cv::Mat &oX)
//        {
//            float **LU__ = new float*[oA.rows];

//            for(int ii = 0; ii < oA.rows; ++ii)
//            {
//                LU__[ii] = new float[oA.cols];

//                for(int jj = 0; jj < oA.cols;++jj)
//                {
//                    LU__[ii][jj] = oA.at<float>(ii,jj);
//                }
//            }

//            // CALL
//            _decomposeLU(LU__, oA.rows);

//            float **L__ = new float*[oA.rows];
//            float **U__ = new float*[oA.rows];

//            for(int ii = 0; ii < oA.rows; ++ii)
//            {
//                L__[ii] = new float[oA.cols];
//                U__[ii] = new float[oA.cols];

//                for(int jj = 0; jj < oA.cols;++jj)
//                {
//                    if(ii < jj)
//                    {
//                        L__[ii][jj] = LU__[ii][jj];
//                        U__[ii][jj] = 0.f;
//                    }
//                    else if(jj > ii)
//                    {
//                        U__[ii][jj] = LU__[ii][jj];
//                        L__[ii][jj] = 0.f;
//                    }
//                    else
//                    {
//                        L__[ii][jj] = 1.f;
//                        U__[ii][jj] = LU__[ii][jj];
//                    }
//                }
//            }


////            for(int ii = 0; ii < oA.rows; ++ii)
////            {
////                for(int jj = 0; jj < oA.rows; ++jj)
////                {
////                    printf("%f ", LU__[ii][jj]);
////                }
////                printf("#################\n\n");
////            }

//            oL = cv::Mat(oA.rows, oA.rows, CV_32FC1);

//            for(int ii = 0; ii < oA.rows; ++ii)
//            {
//                for(int jj = 0; jj < oA.rows; ++jj)
//                {
//                    oL.at<float>(ii,jj) = L__[ii][jj];
//                }
//            }

//            oU = cv::Mat(oA.rows, oA.rows, CV_32FC1);

//            for(int ii = 0; ii < oA.rows; ++ii)
//            {
//                for(int jj = 0; jj < oA.rows; ++jj)
//                {
//                    oU.at<float>(ii,jj) = U__[ii][jj];
//                }
//            }

//            oLU = cv::Mat(oA.rows, oA.rows, CV_32FC1);

//            for(int ii = 0; ii < oA.rows; ++ii)
//            {
//                for(int jj = 0; jj < oA.rows; ++jj)
//                {
//                    oLU.at<float>(ii,jj) = LU__[ii][jj];
//                }
//            }




//            for(int ii = 0; ii < oA.rows; ++ii)
//            {
//                delete[] LU__[ii];
//                delete[] L__[ii];
//                delete[] U__[ii];
//            }
//            delete[] LU__;
//            delete[] L__;
//            delete[] U__;

//        }




/**
 * @brief solveAX_b
 * @param A
 * @param B
 * @param oX
 */
//        static void solveAX_b(const cv::Mat &A, const cv::Mat &B, cv::Mat &oX)
//        {
//            float *l_aFDataA = new float[A.rows * A.cols];
//            float *l_aFDataB = new float[B.rows * B.cols];

//            // input input/output array datas
//            for(int ii = 0; ii < A.rows * A.cols; ++ii)
//            {
//                l_aFDataA[ii] = A.at<float>(ii);
//            }

//            for(int ii = 0; ii < B.rows * B.cols; ++ii)
//            {
//                l_aFDataB[ii] = B.at<float>(ii);
//            }

//            // compute X
//            std::cout << "Cula : " << doCulaSgesv(l_aFDataA, l_aFDataB, A.rows, B.cols) << std::endl;

//            delete[] l_aFDataA;

//             std::cout << "delete A " << std::endl;

//            // fill result mat
//            oX = cv::Mat(B.rows,B.cols, CV_32FC1);
//            std::cout <<"oX " << oX.rows << " " << oX.cols << std::endl;

//            for(int ii = 0; ii < oX.rows*oX.cols; ++ii)
//            {
////                std::cout << ii<< " ";
//                oX.at<float>(ii) = l_aFDataB[ii];
//            }
//            std::cout <<"fill X " << std::endl;


//            delete[] l_aFDataB;

//            std::cout << "end solve " << std::endl;
//        }
///**
// * \brief GPU matrix multiplication. Res(l,n) = A(l,m)*B(m,n)
// * \param [in]  oSMatA    : input A matrix
// * \param [in]  oSMatB    : input B matrix
// * \param [out] oSMatRes  : res C matrix
// */
//static void matrixMultiplication( cv::SparseMat_<float> &oSMatA,  cv::SparseMat_<float> &oSMatB,  cv::SparseMat_<float> &oSMatRes)
//{
//    // Padd matrix offset
//    int l_i32PaddOffsetRowsA = (BLOCKSIZE - (oSMatA.size(0) % BLOCKSIZE))% BLOCKSIZE;
//    int l_i32PaddOffsetColsA = (BLOCKSIZE - (oSMatA.size(1) % BLOCKSIZE))% BLOCKSIZE;
//    int l_i32PaddOffsetRowsB = (BLOCKSIZE - (oSMatB.size(0) % BLOCKSIZE))% BLOCKSIZE;
//    int l_i32PaddOffsetColsB = (BLOCKSIZE - (oSMatB.size(1) % BLOCKSIZE))% BLOCKSIZE;

//    Matrix A, B, C;
//    A.height = oSMatA.size(0) + l_i32PaddOffsetRowsA;
//    A.width  = oSMatA.size(1) + l_i32PaddOffsetColsA;
//    A.elements = new float[A.width * A.height];

//    B.height = oSMatB.size(0) + l_i32PaddOffsetRowsB;
//    B.width  = oSMatB.size(1) + l_i32PaddOffsetColsB;
//    B.elements = new float[B.width * B.height];

//    C.height = A.height;
//    C.width  = B.width;
//    C.elements = new float[C.width * C.height];

//    for(int ii = 0; ii < A.height; ++ii)
//    {
//        for(int jj = 0; jj < A.width; ++jj)
//        {
//            if(ii < oSMatA.size(0) && jj < oSMatA.size(1))
//            {
//                if(oSMatA.ptr(ii,jj, false))
//                {
//                    A.elements[ii*A.width + jj] = oSMatA(ii,jj);
//                }
//                else
//                {
//                    A.elements[ii*A.width + jj] = 0.f;
//                }
//            }
//            else
//            {
//                A.elements[ii*A.width + jj] = 0.f;
//            }
//        }
//    }

//    for(int ii = 0; ii < B.height; ++ii)
//    {
//        for(int jj = 0; jj < B.width; ++jj)
//        {
//            if(ii < oSMatB.size(0) && jj < oSMatB.size(1))
//            {
//                if(oSMatB.ptr(ii,jj, false))
//                {
//                    B.elements[ii*B.width + jj] = oSMatB(ii,jj);
//                }
//                else
//                {
//                    B.elements[ii*B.width + jj] = 0.f;
//                }
//            }
//            else
//            {
//                B.elements[ii*B.width + jj] = 0.f;
//            }
//        }
//    }

//    matMult(A, B, C);

//    delete[] A.elements;
//    delete[] B.elements;

//    int l_i32ResHeight = C.height- l_i32PaddOffsetRowsA;
//    int l_i32ResWidth  = C.width - l_i32PaddOffsetColsB;

//    int size[] = {l_i32ResHeight, l_i32ResWidth};

//    oSMatRes = cv::SparseMat_<float>(2, size);

//    for(int ii = 0; ii < oSMatRes.size(0); ++ii)
//    {
//        for(int jj = 0; jj < oSMatRes.size(1); ++jj)
//        {
//            if(C.elements[ii*C.width + jj] != 0.0)
//            {
//                oSMatRes.ref(ii, jj) = C.elements[ii*C.width + jj];
//            }
//        }
//    }

//    delete[] C.elements;
//}
