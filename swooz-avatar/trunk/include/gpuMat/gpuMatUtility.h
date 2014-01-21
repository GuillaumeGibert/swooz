

#ifndef _GPUMATUTILITY_
#define _GPUMATUTILITY_

#include "commonTypes.h"

// CUDA
#include "gpuMat/configCuda.h"

// OPENCV
#include "opencv2/imgproc/imgproc.hpp"

int inverseMatSgesv(float *aFInputMat, float *aFOutputInvMat, int i32SizeSquareMat);

void transpose(float *idata, float *odata, int size_x, int size_y);

void matMult(const Matrix A, const Matrix B, Matrix C);

void cudaDummyCall(int argc, char **argv);


namespace swUtil
{
    namespace swCuda
    {
        /**
         * \brief GPU matrix inversion.
         * \param [in]  oInput   : input square matrix
         * \param [out] oResult  : output inverse matrix
         */
        static void matrixInversion(const cv::SparseMat_<double> &oInput, cv::Mat &oResult)
        {
            float *l_aFDataIn  = new float[oInput.size(0) * oInput.size(1)];
            float *l_aFDataOut = new float[oInput.size(0) * oInput.size(1)];

            int l_i32CurrentIndexDiag = 0;

            // input input/output array datas
            for(int ii = 0; ii < oInput.size(0) * oInput.size(1); ++ii)
            {
                l_aFDataIn[ii] = (float)oInput( (ii/oInput.size(0)), (ii%oInput.size(0)));

                if(ii == l_i32CurrentIndexDiag) // init output array with id matrix
                {
                    l_aFDataOut[ii] = 1;
                    l_i32CurrentIndexDiag += oInput.size(0) + 1;
                }
                else
                {
                    l_aFDataOut[ii] = 0;
                }
            }

            // compute inverse mat
            inverseMatSgesv(l_aFDataIn, l_aFDataOut, oInput.size(0));

            oResult = cv::Mat(oInput.size(0), oInput.size(0), CV_64FC1);
            for(int ii = 0; ii < oInput.size(0) * oInput.size(0); ++ii)
            {
                oResult.at<double>(ii) = (double)(l_aFDataOut[ii]);
            }

            delete[] l_aFDataIn;
            delete[] l_aFDataOut;
        }

        /**
         * \brief GPU matrix inversion.
         * \param [in]  oInput   : input square matrix
         * \param [out] oResult  : output inverse matrix
         */
        static void matrixInversion(const cv::Mat &oInput, cv::Mat &oResult)
        {
            float *l_aFDataIn  = new float[oInput.rows * oInput.cols];
            float *l_aFDataOut = new float[oInput.rows * oInput.cols];

            int l_i32CurrentIndexDiag = 0;

            // input input/output array datas
            for(int ii = 0; ii < oInput.rows * oInput.cols; ++ii)
            {
                l_aFDataIn[ii] = (float)(oInput.at<double>(ii));

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
            inverseMatSgesv(l_aFDataIn, l_aFDataOut, oInput.rows);

            // fill result mat
            oResult = cv::Mat(oInput.rows,oInput.rows, CV_64FC1);
            for(int ii = 0; ii < oInput.rows*oInput.rows; ++ii)
            {
                oResult.at<double>(ii) = (double)(l_aFDataOut[ii]);
            }

            delete[] l_aFDataIn;
            delete[] l_aFDataOut;
        }

        /**
         * \brief GPU matrix multiplication. Res(l,n) = A(l,m)*B(m,n)
         * \param [in]  oSMatA    : input A matrix
         * \param [in]  oSMatB    : input B matrix
         * \param [out] oSMatRes  : res C matrix
         */
        static void matrixMultiplication( cv::SparseMat_<double> &oSMatA,  cv::SparseMat_<double> &oSMatB,  cv::SparseMat_<double> &oSMatRes)
        {
            // Padd matrix offset
            int l_i32PaddOffsetRowsA = (BLOCKSIZE - (oSMatA.size(0) % BLOCKSIZE))% BLOCKSIZE;
            int l_i32PaddOffsetColsA = (BLOCKSIZE - (oSMatA.size(1) % BLOCKSIZE))% BLOCKSIZE;
            int l_i32PaddOffsetRowsB = (BLOCKSIZE - (oSMatB.size(0) % BLOCKSIZE))% BLOCKSIZE;
            int l_i32PaddOffsetColsB = (BLOCKSIZE - (oSMatB.size(1) % BLOCKSIZE))% BLOCKSIZE;

            Matrix A, B, C;
            A.height = oSMatA.size(0) + l_i32PaddOffsetRowsA;
            A.width  = oSMatA.size(1) + l_i32PaddOffsetColsA;
            A.elements = new float[A.width * A.height];

            B.height = oSMatB.size(0) + l_i32PaddOffsetRowsB;
            B.width  = oSMatB.size(1) + l_i32PaddOffsetColsB;
            B.elements = new float[B.width * B.height];

            C.height = A.height;
            C.width  = B.width;
            C.elements = new float[C.width * C.height];

            for(int ii = 0; ii < A.height; ++ii)
            {
                for(int jj = 0; jj < A.width; ++jj)
                {
                    if(ii < oSMatA.size(0) && jj < oSMatA.size(1))
                    {
                        if(oSMatA.ptr(ii,jj, false))
                        {
                            A.elements[ii*A.width + jj] = (float) oSMatA(ii,jj);
                        }
                        else
                        {
                            A.elements[ii*A.width + jj] = 0.f;
                        }
                    }
                    else
                    {
                        A.elements[ii*A.width + jj] = 0.f;
                    }
                }
            }

            for(int ii = 0; ii < B.height; ++ii)
            {
                for(int jj = 0; jj < B.width; ++jj)
                {
                    if(ii < oSMatB.size(0) && jj < oSMatB.size(1))
                    {
                        if(oSMatB.ptr(ii,jj, false))
                        {
                            B.elements[ii*B.width + jj] = (float)oSMatB(ii,jj);
                        }
                        else
                        {
                            B.elements[ii*B.width + jj] = 0.f;
                        }
                    }
                    else
                    {
                        B.elements[ii*B.width + jj] = 0.f;
                    }
                }
            }

            matMult(A, B, C);

            delete[] A.elements;
            delete[] B.elements;

            int l_i32ResHeight = C.height- l_i32PaddOffsetRowsA;
            int l_i32ResWidth  = C.width - l_i32PaddOffsetColsB;

            int size[] = {l_i32ResHeight, l_i32ResWidth};

            oSMatRes = cv::SparseMat_<double>(2, size);

            for(int ii = 0; ii < oSMatRes.size(0); ++ii)
            {
                for(int jj = 0; jj < oSMatRes.size(1); ++jj)
                {
                    if((double)C.elements[ii*C.width + jj] != 0.0)
                    {
                        oSMatRes.ref(ii, jj) = (double)C.elements[ii*C.width + jj];
                    }
                }
            }

            delete[] C.elements;
        }

        /**
         * \brief GPU matrix multiplication. Res(l,n) = A(l,m)*B(m,n)
         * \param [in]  oMatA    : input A matrix
         * \param [in]  oMatB    : input B matrix
         * \param [out] oMatRes  : res C matrix
         */
        static void matrixMultiplication(const cv::Mat &oMatA, const cv::Mat &oMatB, cv::Mat &oMatRes)
        {
            // Padd matrix offset
            int l_i32PaddOffsetRowsA = (BLOCKSIZE - (oMatA.rows % BLOCKSIZE))% BLOCKSIZE;
            int l_i32PaddOffsetColsA = (BLOCKSIZE - (oMatA.cols % BLOCKSIZE))% BLOCKSIZE;
            int l_i32PaddOffsetRowsB = (BLOCKSIZE - (oMatB.rows % BLOCKSIZE))% BLOCKSIZE;
            int l_i32PaddOffsetColsB = (BLOCKSIZE - (oMatB.cols % BLOCKSIZE))% BLOCKSIZE;

            Matrix A, B, C;
            A.height = oMatA.rows + l_i32PaddOffsetRowsA;
            A.width  = oMatA.cols + l_i32PaddOffsetColsA;
            A.elements = new float[A.width * A.height];

            B.height = oMatB.rows + l_i32PaddOffsetRowsB;
            B.width  = oMatB.cols + l_i32PaddOffsetColsB;
            B.elements = new float[B.width * B.height];

            C.height = A.height;
            C.width  = B.width;
            C.elements = new float[C.width * C.height];

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
                            A.elements[ii*A.width + jj] = (float)oMatA.at<double>(ii,jj);
                        }
                        else
                        {
                            A.elements[ii*A.width + jj] = (float)oMatA.at<int>(ii,jj);
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
                            B.elements[ii*B.width + jj] = (float)oMatB.at<double>(ii,jj);
                        }
                        else
                        {
                            B.elements[ii*B.width + jj] = (float)oMatB.at<int>(ii,jj);
                        }
                    }
                    else
                    {
                        B.elements[ii*B.width + jj] = 0.f;
                    }
                }
            }

            matMult(A, B, C);

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
                        oMatRes.at<double>(ii,jj) = (double)C.elements[ii*C.width + jj];
                    }
                    else
                    {
                        oMatRes.at<int>(ii,jj) = (int)C.elements[ii*C.width + jj];
                    }
                }
            }

            delete[] C.elements;
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
        static void block(cv::SparseMat_<double> &oMat, float *aFBlock, cuint ui32X, cuint ui32Y, cuint ui32HeightBlock, cuint ui32WidthBlock)
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
                            aFBlock[ii * ui32WidthBlock + jj] = (float)oMat(l_i32MatII, l_i32MatJJ);
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
        static void updateMatWithBlock(cv::SparseMat_<double> &oMat, float *aFBlock, cuint ui32X, cuint ui32Y, cuint ui32HeightBlock, cuint ui32WidthBlock)
        {
             for(uint ii = 0; ii < ui32HeightBlock; ++ii)
             {
                 for(uint jj = 0; jj < ui32WidthBlock; ++jj)
                 {
                     int l_i32MatII = ui32X * ui32HeightBlock + ii;
                     int l_i32MatJJ = ui32Y * ui32WidthBlock  + jj;

                     if(l_i32MatII < oMat.size(0) && l_i32MatJJ < oMat.size(1))
                     {
                         if(aFBlock[ii * ui32WidthBlock + jj] != 0.0)
                         {
                             oMat.ref(l_i32MatII, l_i32MatJJ) = (double)aFBlock[ii * ui32WidthBlock + jj];
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
        static void blockMatrixMultiplication( cv::SparseMat_<double> &oSMatA,  cv::SparseMat_<double> &oSMatB,  cv::SparseMat_<double> &oSMatRes, cint i32SizeMatBlock = 2)
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
            oSMatRes = cv::SparseMat_<double>(2, l_fI32Size);

            for(int ii = 0; ii < l_i32SizeMatBlock; ++ii) // C ii ..
            {
                for(int jj = 0; jj < l_i32SizeMatBlock; ++jj) // C .. jj
                {
                    std::fill_n(l_fSubCCopy, subC.height * subC.width, 0.f);

                    // compute Cij
                    for(int kk = 0; kk < l_i32SizeMatBlock; ++kk)
                    {
    //                    std::cout << "subA_";
                        block(oSMatA, subA.elements, ii, kk, subA.height, subA.width);
    //                                    std::cout  << std::endl << "display subA : " << std::endl;
    //                                    for(int _kk = 0; _kk < subA.height; ++_kk)
    //                                    {
    //                                        for(int ll = 0; ll < subA.width; ++ll)
    //                                            std::cout << subA.elements[_kk*subA.width + ll] << " ";
    //                                        std::cout << std::endl;
    //                                    }

    //                    std::cout << "subB_";
                        block(oSMatB, subB.elements, kk, jj, subB.height, subB.width);
    //                                    std::cout  << std::endl << "display subB : " << std::endl;
    //                                    for(int _kk = 0; _kk < subB.height; ++_kk)
    //                                    {
    //                                        for(int ll = 0; ll < subB.width; ++ll)
    //                                            std::cout << subB.elements[_kk*subB.width + ll] << " ";
    //                                        std::cout << std::endl;
    //                                    }


        //                gpuMult(subA, subB, subC);
        //                std::fill_n(subC.elements, subC.height * subC.width, 0.f);

                        matMult(subA, subB, subC);

    //                    std::cout << "subC_";
    //                                    std::cout  << std::endl << "display subC : " << std::endl;
    //                                    for(int _kk = 0; _kk < subC.height; ++_kk)
    //                                    {
    //                                        for(int ll = 0; ll < subC.width; ++ll)
    //                                            std::cout << subC.elements[_kk*subC.width + ll] << " ";
    //                                        std::cout << std::endl;
    //                                    }

    //                    std::cout << " sub copy : " << std::endl;
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
