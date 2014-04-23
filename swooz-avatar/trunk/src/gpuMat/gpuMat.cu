
//#include <cutil.h>
#include "cublas.h"
#include "cula.h"
#include <iostream>

#include "gpuMat/configCuda.h"


int doCulaSgesv(float *aFInputMat, float *aFOutputInvMat, int i32SizeSquareMat)
{
    culaStatus l_oStatus;

    // init cula
    l_oStatus = culaInitialize();
    // check error
    if(l_oStatus != culaNoError)
    {
        std::cerr << "Error cuda init : " << culaGetErrorInfo() << std::endl;
        return -1; // TODO : create a throw
    }

    int *l_aI32Ipiv = new int[i32SizeSquareMat * i32SizeSquareMat * sizeof(float)];

    // launch gpu computing
    l_oStatus = culaSgesv(i32SizeSquareMat, i32SizeSquareMat, aFInputMat, i32SizeSquareMat, l_aI32Ipiv, aFOutputInvMat, i32SizeSquareMat);

    delete[] l_aI32Ipiv;

    if(l_oStatus != culaNoError)
    {
        std::cerr << "Error culaSgesv : " << culaGetErrorInfo() << std::endl;
        return -1; // TODO : create a throw
    }

    culaShutdown();

    return 0;
}

int LUDecomposition(float *aFMat, int i32SizeSquareMat)
{
    culaStatus l_oStatus;

    // init cula
    l_oStatus = culaInitialize();
    // check error
    if(l_oStatus != culaNoError)
    {
        std::cerr << "Error cuda init : " << culaGetErrorInfo() << std::endl;
        return -1; // TODO : create a throw
    }

    int *l_aI32Ipiv = new int[i32SizeSquareMat * i32SizeSquareMat * sizeof(float)];

    // launch gpu computing
    l_oStatus = culaSgetrf(i32SizeSquareMat, i32SizeSquareMat, aFMat, i32SizeSquareMat, l_aI32Ipiv);

    if(l_oStatus != culaNoError)
    {
        std::cerr << "Error culaSgesv : " << culaGetErrorInfo() << std::endl;
        return -1; // TODO : create a throw
    }

    delete[] l_aI32Ipiv;

    return 1;
}

// This kernel is optimized to ensure all global reads and writes are coalesced,
// and to avoid bank conflicts in shared memory.  This kernel is up to 11x faster
// than the naive kernel below.  Note that the shared memory array is sized to
// (BLOCK_DIM+1)*BLOCK_DIM.  This pads each row of the 2D block in shared memory
// so that bank conflicts do not occur when threads address the array column-wise.
__global__ void transpose_kernel(float *odata, float *idata, int width, int height)
{
    __shared__ float block[BLOCKSIZE][BLOCKSIZE+1];

    // read the matrix tile into shared memory
    unsigned int xIndex = blockIdx.x * BLOCKSIZE + threadIdx.x;
    unsigned int yIndex = blockIdx.y * BLOCKSIZE + threadIdx.y;
    if((xIndex < width) && (yIndex < height))
    {
        unsigned int index_in = yIndex * width + xIndex;
        block[threadIdx.y][threadIdx.x] = idata[index_in];
    }

    __syncthreads();

    // write the transposed matrix tile to global memory
    xIndex = blockIdx.y * BLOCKSIZE + threadIdx.x;
    yIndex = blockIdx.x * BLOCKSIZE + threadIdx.y;
    if((xIndex < height) && (yIndex < width))
    {
        unsigned int index_out = yIndex * height + xIndex;
        odata[index_out] = block[threadIdx.x][threadIdx.y];
    }
}


// This naive transpose kernel suffers from completely non-coalesced writes.
// It can be up to 10x slower than the kernel above for large matrices.
__global__ void transpose_naive_kernel(float *odata, float* idata, int width, int height)
{
   unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
   unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;

   if (xIndex < width && yIndex < height)
   {
       unsigned int index_in  = xIndex + width * yIndex;
       unsigned int index_out = yIndex + height * xIndex;
       odata[index_out] = idata[index_in];
   }
}

void transpose(float *idata, float *odata, int width, int height)
{
    // size of memory required to store the matrix
    const unsigned int mem_size = sizeof(float) * width * height;

    // allocate device memory
    float* d_idata;
    float* d_odata;
    cudaMalloc( (void**) &d_idata, mem_size);
    cudaMalloc( (void**) &d_odata, mem_size);

    // copy host memory to device
    cudaMemcpy( d_idata, idata, mem_size,
                                cudaMemcpyHostToDevice);

    // setup execution parameters
    dim3 grid(width / BLOCKSIZE, height / BLOCKSIZE, 1);
    dim3 threads(BLOCKSIZE, BLOCKSIZE, 1);

    // warmup so we don't time CUDA startup
//    transpose_naive_kernel<<< grid, threads >>>(d_odata, d_idata, size_x, size_y);
    transpose_kernel<<< grid, threads >>>(d_odata, d_idata, width, height);

    // synchronize here, so we make sure that we don't count any time from the asynchronize kernel launches.
    cudaThreadSynchronize();

    printf("Transposing a %d by %d matrix of floats...\n", width, height);

    // execute the kernel
    transpose_kernel<<< grid, threads >>>(d_odata, d_idata, width, height);
    cudaThreadSynchronize();

    // copy result from device to    host
    odata = (float*) malloc(mem_size);
    cudaMemcpy( odata, d_odata, mem_size,
                                cudaMemcpyDeviceToHost);

    cudaFree(d_idata);
    cudaFree(d_odata);

    cudaThreadExit();
}



// Get a matrix element
__device__ float GetElement(const Matrix A, int row, int col)
{
    return A.elements[row * A.stride + col];
}

// Set a matrix element
__device__ void SetElement(Matrix A, int row, int col,
                           float value)
{
    A.elements[row * A.stride + col] = value;
}

// Get the BLOCK_SIZExBLOCK_SIZE sub-matrix Asub of A that is
// located col sub-matrices to the right and row sub-matrices down
// from the upper-left corner of A
 __device__ Matrix GetSubMatrix(Matrix A, int row, int col)
{
    Matrix Asub;
    Asub.width    = BLOCKSIZE;
    Asub.height   = BLOCKSIZE;
    Asub.stride   = A.stride;
    Asub.elements = &A.elements[A.stride * BLOCKSIZE * row
                                         + BLOCKSIZE * col];
    return Asub;
}

// Matrix multiplication kernel called by MatMul()
 __global__ void MatMulKernel(Matrix A, Matrix B, Matrix C)
{
    // Block row and column
    int blockRow = blockIdx.y;
    int blockCol = blockIdx.x;

    // Each thread block computes one sub-matrix Csub of C
    Matrix Csub = GetSubMatrix(C, blockRow, blockCol);

    // Each thread computes one element of Csub
    // by accumulating results into Cvalue
    float Cvalue = 0;

    // Thread row and column within Csub
    int row = threadIdx.y;
    int col = threadIdx.x;

    // Loop over all the sub-matrices of A and B that are
    // required to compute Csub
    // Multiply each pair of sub-matrices together
    // and accumulate the results
    for (int m = 0; m < (A.width / BLOCKSIZE); ++m) {

        // Get sub-matrix Asub of A
        Matrix Asub = GetSubMatrix(A, blockRow, m);

        // Get sub-matrix Bsub of B
        Matrix Bsub = GetSubMatrix(B, m, blockCol);

        // Shared memory used to store Asub and Bsub respectively
        __shared__ float As[BLOCKSIZE][BLOCKSIZE];
        __shared__ float Bs[BLOCKSIZE][BLOCKSIZE];

        // Load Asub and Bsub from device memory to shared memory
        // Each thread loads one element of each sub-matrix
        As[row][col] = GetElement(Asub, row, col);
        Bs[row][col] = GetElement(Bsub, row, col);

        // Synchronize to make sure the sub-matrices are loaded
        // before starting the computation
        __syncthreads();

        // Multiply Asub and Bsub together
        for (int e = 0; e < BLOCKSIZE; ++e)
            Cvalue += As[row][e] * Bs[e][col];

        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        __syncthreads();
    }

    // Write Csub to device memory
    // Each thread writes one element
    SetElement(Csub, row, col, Cvalue);
}

 // Matrix multiplication - Host code
 // Matrix dimensions are assumed to be multiples of BLOCK_SIZE
 void matMult(const Matrix A, const Matrix B, Matrix C, const int blockSize)
 {
     // Load A and B to device memory
     Matrix d_A;
     d_A.width = d_A.stride = A.width; d_A.height = A.height;
     size_t size = A.width * A.height * sizeof(float);
     cudaMalloc(&d_A.elements, size);
     cudaMemcpy(d_A.elements, A.elements, size,
                cudaMemcpyHostToDevice);
     Matrix d_B;
     d_B.width = d_B.stride = B.width; d_B.height = B.height;
     size = B.width * B.height * sizeof(float);
     cudaMalloc(&d_B.elements, size);
     cudaMemcpy(d_B.elements, B.elements, size,
     cudaMemcpyHostToDevice);

     // Allocate C in device memory
     Matrix d_C;
     d_C.width = d_C.stride = C.width; d_C.height = C.height;
     size = C.width * C.height * sizeof(float);
     cudaMalloc(&d_C.elements, size);

     // Invoke kernel
     dim3 dimBlock(blockSize, blockSize);
     dim3 dimGrid(B.width / dimBlock.x, A.height / dimBlock.y);
     MatMulKernel<<<dimGrid, dimBlock>>>(d_A, d_B, d_C);

     // Read C from device memory
     cudaMemcpy(C.elements, d_C.elements, size,
                cudaMemcpyDeviceToHost);

     // Free device memory
     cudaFree(d_A.elements);
     cudaFree(d_B.elements);
     cudaFree(d_C.elements);
 }


  //***************************************************************************************
  //void DecomposeLU( int M, int N, int lda , float* A,
  //                    int* permute, float epsilon, InfoStat& stat)
  //
  // M         :     Num of rows of A
  // N         :     Num of column of A
  // A         :     Float Matrix of size M*N
  //                on the output contains the result of the LU decomposition
  //                The diagonal elements for L are not stored in A ( assuming they are all 1)
  //lda        :    Leading dim of A lda < std::max(1,M)
  //P          :      Permutation vector of size M
  //epsilon    :     Epsilon (used to test for singularities)
  //stat          :  return status
  // **************************************************************************************



//  void DecomposeLU(int M, int N, int lda , float* A, int* P, float epsilon)//, InfoStat& stat)
//  {
//       cublasStatus cuStat;
//       //Preconditions
//       if ( M<=0 || N<=0 || lda < std::max(1,M) )
//       {
//           printf("bad preconditions\n");
// //           stat._info = -1;
// //           if (M<=0)
// //               stat._str = "M<=0";
// //           if (N<=0)
// //               stat._str = "M<=0";
// //           if (lda < std::max(1,M))
// //               stat._str = "lda < std::max(1,M)";
//            return;
//       }
//       int minDim = std::min( M, N );
//       for (int k=0; k<minDim-1; k++)
//       {
//            int pivotRow = k-1+cublasIsamax(M-k,A+k + k*lda, 1); // row relative to the current submatrix
//            int kp1 = k+1;
//            P[k] = pivotRow;
//            if (pivotRow!=k)
//            {
//                 cublasSswap(N, A+pivotRow, lda, A+k, lda);
//            }
//            float valcheck;
//            cublasGetVector(1,sizeof(float),A+k+ k*lda, 1, &valcheck, 1);
//            if (fabs(valcheck) < epsilon)
//            {
//                printf("Matrix is Singular\n");
// //                stat._info =k+1;
// //                stat._str = " Matrix is Singular ";
//                 return;
//            }
//            if (kp1 < M)
//           {
//                cublasSscal(M-kp1, 1.0f/valcheck,A+kp1+ k*lda, 1);
//           }
//           if ( kp1 < minDim )
//           {
//                cublasSger (M-kp1, N-kp1, -1.0f,A+kp1+ k*lda, 1, A+k+ kp1*lda, lda,A+ kp1*lda+kp1, lda);
//           }
//       }
// //      CHECK_CUBLAS("decomposeLU pb");
//  }




//// DecomposeBlockedLU(A.size(), A.size(), std::max(1,A.size()),   );

// //***************************************************************************************
// //void DecomposeBlockedLU ( int M, int N,int lda,
// //                          float *A,
// //                          int* P, int blockSize,float epsilon, InfoStat &stat )
// //
// // M            :   Num of rows of A
// // N            :   Num of column of A
// // A            :   Float Matrix of size M*N
// //                  on the output contains the result of the LU decomposition
// //                  The diagonal elements for L are not stored in A ( assuming they are all 1)
// //lda           :   Leading dim of A lda < std::max(1,M)
// //P             :   Permutation vector of size M
// //blockSize     :   Size of the submatrices
// //                  if blockSize>=M || blockSize==1 unblocked decomposition is called
// //epsilon       :   Epsilon (used to test for singularities)
// //stat          :  return status
// // **************************************************************************************
// void DecomposeBlockedLU (   int M, int N,int lda,
//                             float *A,
//                             int* P, int blockSize,float epsilon)//, InfoStat &stat )
// {

//     cublasStatus cuStat;
//     //Preconditions
//     if (M < 0 || N < 0 || lda < std::max(1,M) )
//     {
////         stat._info = -1;
////         if (M<=0)
////             stat._str = "M<=0";
////         if (N<=0)
////             stat._str = "M<=0";
////         if (lda < std::max(1,M))
////             stat._str = "lda < std::max(1,M)";

//         return;
//     }

//     int minSize = std::min(M,N);

//     if ( blockSize > minSize || blockSize == 1)
//     {
//         //straight LU decomposition
//         DecomposeLU( M, N, lda, A, P, epsilon);//, stat);
//     }
//     else
//     {
//         //blocked decomposition
//         for (int i =0; i< minSize ; i+=blockSize)
//         {
//             int realBlockSize  = std::min(minSize - i, blockSize);

//             //decompose the current rectangular block
//             DecomposeLU( M-i, realBlockSize, lda, A+i+i*lda, P+i, epsilon);//, stat);

//             //adjust pivot infos
//             //Todo : write a kernel for that
//             for (int p = i; p< std::min( M, i+realBlockSize)-1; p++)
//             {
//                     P[p] = P[p]+i;
//                     if (P[p] != p)
//                     {
//                         // Apply interchanges to columns 0:i.
//                         cublasSswap(i, A+p , lda, A+ P[p], lda);
//                         // Apply interchanges to columns i+blockSize:N.
//                         cublasSswap(N-i-realBlockSize, A+p+(i+realBlockSize)*lda , lda, A+ P[p]+(i+realBlockSize)*lda, lda);
//                     }

//             }

//             // Compute block row of U.
//             cublasStrsm( 'l','l','n','u', realBlockSize, N-i-realBlockSize, 1.0f,
//                          A +i +i*lda, lda, A +i + (i+realBlockSize)*lda, lda);
////             CHECK_CUBLAS("decomposeBlockedLU cublasStrsm");

//             if (i+realBlockSize < M)
//             {
//                  cublasSgemm('n','n',  M-i-realBlockSize, N-i-realBlockSize, realBlockSize,
//                              -1.0f,
//                              A+i+realBlockSize+i*lda,lda,
//                              A+i+(realBlockSize+i)*lda,lda,
//                              1.0f,
//                              A+i+realBlockSize+(realBlockSize+i)*lda,lda );
////                  CHECK_CUBLAS("decomposeBlockedLU cublasSgemm");
//             }
//         }
//     }

// }


// int decomp(float* src_d, float *dst_d, int n)
// {
//     cublasHandle_t handle;
//     cublasCreate_v2(&handle);

//     int batchSize = 1;
//     int *P, *INFO;
//     cudaMalloc<int>(&P,n * batchSize * sizeof(int));
//     cudaMalloc<int>(&INFO,batchSize * sizeof(int));

//     int lda = n;//spitch/sizeof(float);

//     float *A[] = { src_d };
//     float** A_d;
//     cudaMalloc<float*>(&A_d,sizeof(A));
//     cudaMemcpy(A_d,A,sizeof(A),cudaMemcpyHostToDevice);

//     cublasStatus_t status = cublasSgetrfBatched(handle,
//                                                 n,
//                                                 A_d,
//                                                 lda,
//                                                 P,
//                                                 INFO,
//                                                 batchSize);


//      int INFOh = 0;
//      cudaMemcpy(&INFOh,INFO,sizeof(int),cudaMemcpyDeviceToHost);

//      if(INFOh == n)
//      {
//              /* Singular */
//              cublasDestroy_v2(handle);
//              cudaFree(INFO);
//              cudaFree(P);
//              cudaFree(A_d);
//              return -1;
//      }

//      if(CUBLAS_STATUS_SUCCESS != status)
//      {
//              cublasDestroy_v2(handle);
//              cudaFree(INFO);
//              cudaFree(P);
//              cudaFree(A_d);
//              return -2;
//      }
// }


//// void _decomposeLU2(float *src_d, float *dst_d, int n)
// void _decomposeLU2(float *src_d, int n)
// {
//     cudaError_t cudaStat;
//     cublasStatus stat;
//     cublasHandle_t handle;
////     cublasCreate(&handle );

//     int batchSize = n;
//     int *P, *INFO;
//     cudaStat = cudaMalloc<int>(&P,n * batchSize * sizeof(int));
//     printf("malloc1 : %d\n", cudaStat);
//     cudaStat = cudaMalloc<int>(&INFO, batchSize * sizeof(int));
//     printf("malloc2 : %d\n", cudaStat);

//     float *A[] = {src_d};
////     float *LU[] = {dst_d};

//     float** A_d;
//     cudaStat = cudaMalloc<float*>(&A_d, sizeof(A));
//     printf("malloc3 : %d\n", cudaStat);

//     cudaStat =cudaMemcpy(A_d, A, sizeof(A), cudaMemcpyHostToDevice);
//     printf("memcopy : %d\n", cudaStat);

//     cublasStatus_t statut =
//             cublasSgetrfBatched(handle,    // handle to the CUBLAS library context.
//                                 n,         // number of rows and columns of Aarray[i].
//                                 A_d,       // array of pointers to <type> array, with each array of dim. n x n with lda>=max(1,n).
//                                 n,         // leading dimension of two-dimensional array used to store each matrix Aarray[i].
//                                 P,         // array of size n x batchSize that contains the pivoting sequence of each factorization of Aarray[i] stored in a linear fashion.
//                                 INFO,      // array of size batchSize that info(=infoArray[i]) contains the information of factorization of Aarray[i].
//                                 batchSize  // number of pointers contained in A
//                                );
//     printf("statut : %d\n", statut);

//     int INFOh = 0;
//     cudaMemcpy(&INFOh,INFO,sizeof(int),cudaMemcpyDeviceToHost);
//     printf("Info : %d\n", INFOh);

////     cudaMemcpy(LU, A_d, sizeof(A), cudaMemcpyHostToDevice);
//     cudaMemcpy(A, A_d, sizeof(A), cudaMemcpyHostToDevice);

//     cudaFree(A_d);
//     cudaFree(P);
//     cudaFree(INFO);


////     cublasDestroy(handle);
// }



// void _decomposeLU(float **A, int rows)
// {
//     int *P__ = new int[rows*rows];
//     int *infoArray = new int[rows];


////     float** A_d;
////    cudaMalloc<float*>(&A_d,sizeof(A));

//     cublasHandle_t handle;
//     cublasStatus_t statut = cublasSgetrfBatched(handle,   // handle to the CUBLAS library context.
//                                                 rows,  // number of rows and columns of Aarray[i].
//                                                 A,     // array of pointers to <type> array, with each array of dim. n x n with lda>=max(1,n).
//                                                 rows,        // leading dimension of two-dimensional array used to store each matrix Aarray[i].
//                                                 P__,      //array of size n x batchSize that contains the pivoting sequence of each factorization of Aarray[i] stored in a linear fashion.
//                                                 infoArray,//array of size batchSize that info(=infoArray[i]) contains the information of factorization of Aarray[i].
//                                                 rows   //number of pointers contained in A
//                                                 );
//     printf("statut : %d\n", statut);

//     for(int ii = 0; ii < rows; ++ii)
////         if(infoArray[ii] != 0)
//             printf(" e[%d %d] ", ii, infoArray[ii]);

//     delete[] P__;
//     delete[] infoArray;
// }





// void testCublas()
// {
////     float *data;

////     DecomposeLU(A.size(), A.size(), std::max(1,A.size()), A.data(), p.datat(), 0.f)

//////     float **data = new float[];


////     //     cublasStatus_t cublasSgetrfBatched(cublasHandle_t handle,   handle to the CUBLAS library context.
////     //                                        int n,                   number of rows and columns of Aarray[i].
////     //                                        float *Aarray[],         array of pointers to <type> array, with each array of dim. n x n with lda>=max(1,n).
////     //                                        int lda,                 leading dimension of two-dimensional array used to store each matrix Aarray[i].
////     //                                        int *PivotArray,         array of size n x batchSize that contains the pivoting sequence of each factorization of Aarray[i] stored in a linear fashion.
////     //                                        int *infoArray,          array of size batchSize that info(=infoArray[i]) contains the information of factorization of Aarray[i].
////     //                                        int batchSize);          number of pointers contained in A

////     // LU Factorization
////     int *pivot;
////     int *info;
////     float **testF;
////     cublasHandle_t handle;
////     cublasStatus_t statu = cublasSgetrfBatched(
////                                                handle,
////                                                100,
////                                                testF,
////                                                0,
////                                                pivot,
////                                                info,
////                                                10
////                                                );

// }




// //unsigned int xIndex = blockDim.x * blockIdx.x + threadIdx.x;
// //unsigned int yIndex = blockDim.y * blockIdx.y + threadIdx.y;

// __global__ void GetVals(sparse_matrix A, sparse_matrix B,sparse_matrix C, int* indexTable)
// {
//     const int laneId = threadIdx.x;
//     const int blockId = blockIdx.x;
//     __shared__ unsigned int back;
//     int rowAStart; // The index into A.jc and A.val
//     int rowAEnd; // The boundary index for A
//     float valA; // The value of the current A nonzero
//     int rowBStart; // The index into B.jc and B.val
//     int rowBEnd; // The boundary index for B
//     int colB; // The current column in B being used
//     int rowCStart; // The index into C.jc and C.val
//     int rowCEnd; // The boundary index for C
//     int hash; // The calculated hash value
//     int i, j; // Loop iterators
//     // Set the global hash table to point to the space
//     // used by this warp
//     int* gColHashTable;
//     float* gValHashTable;
//     int globalEntries;
//     indexTable = &indexTable[C.cols * blockId];
//     if(laneId == 0)
//         back = 0;


//     for(int rowA = blockId; rowA < A.rows; rowA += gridDim.x)
//     {
//         rowAStart = A.ir[rowA];
//         rowAEnd = A.ir[rowA + 1];

//         for(i = laneId; i < C.cols; ++i)
//         {
//             indexTable[i] = -1;
//         }

//         __syncthreads();

//         // Set the location of the global hash table
//         rowCStart = C.ir[rowA];
//         rowCEnd = C.ir[rowA + 1];
//         globalEntries = rowCEnd - rowCStart;
//         gColHashTable = &C.jc[rowCStart];
//         gValHashTable = &C.val[rowCStart];

//         for(i = rowAStart; i < rowAEnd; ++i)
//         {
//             valA = A.val[i];
//             rowBStart = B.ir[A.jc[i]];
//             rowBEnd = B.ir[A.jc[i] + 1];
//             int curIdx;
//             int* storeInt;
//             float* storeFloat;
//             float valB;

//             for(j = rowBStart + laneId; __any(j < rowBEnd); j += warpSize)
//             {
//                 colB = j < rowBEnd ? B.jc[j] : -1;
//                 curIdx = colB == -1 ? -1 : indexTable[colB];
//                 hash = colB != -1 && curIdx == -1 ?
//                 atomicInc(&back, globalEntries - 1) : curIdx;
//                 storeInt = hash == -1 ? &hash : &indexTable[colB];
//                 *storeInt = hash;

//                 storeInt = hash == -1 ? &colB : &gColHashTable[hash];
//                 *storeInt = colB;
//                 valB = colB == -1 ? 1 : B.val[j];
//                 storeFloat = hash == -1 ? &valA : &gValHashTable[hash];
//                 *storeFloat += valB * valA;
//             }
//         } // For each nonzero in the A row
//     } // For each assigned row in A
// }

// __global__ void SortCols(sparse_matrix C, int maxRowNNZ, int* workQueue)
// {
//     const int laneId = threadIdx.x;

//     const int blockId = blockIdx.x;
//     // Dynamic shared memory
//     extern __shared__ int sharedMem[];
//     // The maximum size of the queue
//     const int queueSize = (maxRowNNZ / 2) + 1;
//     // The maximum number of passes needed
//     int maxShift = __log2f(C.cols) / RADIX_BITS;
//     // The number of passes for the work in the queue
//     int* workPasses = &workQueue[blockId * queueSize];
//     // The front of the bucket for the work in the queue
//     int* workFronts = &workQueue[gridDim.x * queueSize];
//     workFronts = &workFronts[blockId * queueSize];
//     // The back of the bucket for the work in the queue
//     int* workBacks = &workQueue[gridDim.x * queueSize * 2];
//     workBacks = &workBacks[blockId * queueSize];
//     int front; // The front of the work queue.
//     __shared__ unsigned int back; // The back of the work queue.
//     // Holds the sizes for the buckets being sorted by the threads
//     int* bucketSizes = &sharedMem[laneId * RADIX_BASE];
//     // The ending index of the buckets being sorted
//     int* bucketBounds = &sharedMem[blockDim.x * RADIX_BASE];
//     bucketBounds = &bucketBounds[laneId * RADIX_BASE];
//     int pass; // The pass number of the current bucket
//     int bucketFront; // The index of the front of the bucket
//     int bucketBack; // The index of the back of the bucket
//     int bucketIdx; // The index of an item in the bucket
//     int shiftCount; // The number of bits to shift to get the index
//     int iTmp; // A temporary variable for swapping
//     float fTmp;
//     int swapIdx; // The index to swap with
//     int queueIdx; // An index into the work queue
//     int prev; // The previous bucket offset
//     int subIdx;
//     for(int rowC = blockId; rowC < C.rows; rowC += gridDim.x)
//     {
//         // Skip if there are not non-zeros to sort
//         if(C.ir[rowC] == C.ir[rowC + 1])
//             continue;

//         // Clear the work queue
//         for(int i = laneId + 1; i < queueSize; i += blockDim.x)
//         {
//             workPasses[i] = -1;
//         }
//         workPasses[0] = 0;
//         workFronts[0] = C.ir[rowC];
//         workBacks[0] = C.ir[rowC + 1];
//         front = 0;
//         back = 1;
//         __syncthreads();

//         // While there is more work in the queue
//         while(front != back)
//         {
//             queueIdx = (front + laneId) % queueSize;
//             // Get the work
//             pass = workPasses[queueIdx];
//             bucketFront = workFronts[queueIdx];
//             bucketBack = workBacks[queueIdx];
//             // Clear this work
//             workPasses[queueIdx] = -1;
//             // Move the front forward
//             if((back > front && back - front <= blockDim.x) || (back < front && (back + queueSize) - front <= blockDim.x))
//             {
//                 front = back;
//             }
//             else
//             {
//                 front = (front + blockDim.x) % queueSize;
//             }

//             // There is work to do
//             if(pass >= 0)
//             {
//                 // Clear the bucket sizes
//                 for(int i = 0; i < RADIX_BASE; ++i)
//                 {
//                     bucketSizes[i] = 0;
//                 }
//                 shiftCount = (maxShift - pass) * RADIX_BITS;

//                 // First, determine the size of the buckets
//                 for(int i = bucketFront; i < bucketBack; ++i)
//                 {
//                     ++bucketSizes[(C.jc[i] >> shiftCount) & RADIX_MASK];
//                 }

//                 // Determine the indexes of the buckets and put
//                 // them into the work queue
//                 prev = bucketFront;
//                 for(int i = 0; i < RADIX_BASE; ++i)
//                 {
//                     // Determine the bucket end
//                     bucketIdx = bucketSizes[i] + prev;
//                     // Place the bucket into the work queue only
//                     // if it has items to be sorted
//                     if(bucketSizes[i] > 1)
//                     {
//                         queueIdx = atomicInc(&back, queueSize - 1);
//                         workPasses[queueIdx] = pass + 1;
//                         workFronts[queueIdx] = prev;
//                         workBacks[queueIdx] = bucketIdx;
//                     }
//                     // Store the bucket end
//                     bucketSizes[i] = bucketIdx;
//                     bucketBounds[i] = bucketIdx;
//                     prev = bucketIdx;
//                 }

//                 // Place the items into the buckets
//                 bucketIdx = bucketFront;
//                 while(bucketIdx != bucketBack)
//                 {
//                     subIdx = (C.jc[bucketIdx] >> shiftCount) & RADIX_MASK;
//                     swapIdx = --bucketSizes[subIdx];;
//                     // Done sorting this bucket, move to the next open one
//                     if(swapIdx == bucketIdx)
//                     {
//                         do
//                         {
//                             bucketIdx = bucketBounds[subIdx++];
//                         } while(bucketIdx != bucketBack && bucketSizes[subIdx] == bucketIdx);
//                     }
//                     else
//                     {
//                         // Swap swapIdx and bucketIdx
//                         iTmp = C.jc[swapIdx];
//                         C.jc[swapIdx] = C.jc[bucketIdx];
//                         C.jc[bucketIdx] = iTmp;
//                         fTmp = C.val[swapIdx];
//                         C.val[swapIdx] = C.val[bucketIdx];
//                         C.val[bucketIdx] = fTmp;
//                     }
//                 }
//             } // If this thread has work
//             __syncthreads();
//         } // While there is work to do
//     } // For all rows in C
// }

// //#include <thrust/version.h>
// //#include <cusp/version.h>

// //#include <cusp/multiply.h>
// //#include <cusp/array2d.h>
// //#include <cusp/print.h>

// //int testCuSparse(void)
// //{
// //    int cuda_major =  CUDA_VERSION / 1000;
// //    int cuda_minor = (CUDA_VERSION % 1000) / 10;

// //    int thrust_major = THRUST_MAJOR_VERSION;
// //    int thrust_minor = THRUST_MINOR_VERSION;

// //    int cusp_major = CUSP_MAJOR_VERSION;
// //    int cusp_minor = CUSP_MINOR_VERSION;

// //    std::cout << "CUDA   v" << cuda_major   << "." << cuda_minor   << std::endl;
// //    std::cout << "Thrust v" << thrust_major << "." << thrust_minor << std::endl;
// //    std::cout << "Cusp   v" << cusp_major   << "." << cusp_minor   << std::endl;


// //    // initialize matrix
// //    cusp::array2d<float, cusp::host_memory> A(2,2);
// //    A(0,0) = 10;  A(0,1) = 20;
// //    A(1,0) = 40;  A(1,1) = 50;

// //    // initialize input vector
// //    cusp::array1d<float, cusp::host_memory> x(2);
// //    x[0] = 1;
// //    x[1] = 2;

// //    // allocate output vector
// //    cusp::array1d<float, cusp::host_memory> y(2);

// //    // compute y = A * x
// //    cusp::multiply(A, x, y);

// //    // print y
// //    cusp::print(y);

// //    return 0;
// //}





// __global__ void GetNNZ(sparse_matrix A, sparse_matrix B, sparse_matrix C,int* workingSet)
// {
//     const int laneId = threadIdx.x;
//     const int warpId = blockIdx.x;
//     int* nonzeros;
//     int rowAStart, rowAEnd, rowBStart, rowBEnd;
//     int nnz;
//     int colC;
//     extern __shared__ int nzCount[];
//     nonzeros = &workingSet[warpId * B.cols];
//     // Iterate through each assigned row in A.
//     for(int rowA = warpId; rowA < A.rows; rowA += gridDim.x)
//     {
//         rowAStart = A.ir[rowA];
//         rowAEnd = A.ir[rowA + 1];
//         // There are no non-zeros in this row so continue
//         if(rowAStart == rowAEnd)
//         {
//             if (laneId == 0)
//                 C.ir[rowA] = 0;
//             __syncthreads();
//             continue;
//         }

//         // Reset the nz counts
//         nzCount[laneId] = 0;
//         // reset the nonzeros table
//         for (int i=laneId; i<B.cols; i+= warpSize)
//         {
//             nonzeros[i] = 0;
//         }
//         __syncthreads();

//         for(int i = rowAStart; i < rowAEnd; ++i)
//         {
//             rowBStart = B.ir[A.jc[i]];
//             rowBEnd = B.ir[A.jc[i]+1];

//             for (int j = rowBStart + laneId; j < rowBEnd; j += warpSize)
//             {
//                 colC = B.jc[j];
//                 nzCount[laneId] += nonzeros[colC] == 0;
//                 nonzeros[colC] = 1;
//             }
//             __syncthreads();
//         }

//         if(laneId == 0)
//         {
//             nnz = nzCount[0];
//             for(int i = 1; i < warpSize; ++i)
//             {
//                 nnz += nzCount[i];
//             }

//             C.ir[rowA] = nnz;
//         }

//         __syncthreads();
//     }
// }




 //#include <cuda_runtime.h>
 //#include <cublas_v2.h>


 //using namespace swUtil;
 //void cudaDummyCall()
 //{
 //    // init CUDA
 //    //CUT_DEVICE_INIT(argc, argv);
 //    cudaFree(0);
 //}
 //void checkStatus(culaStatus status)
 //{
 //    char buf[256];

 //    if(!status)
 //        return;

 //    culaGetErrorInfoString(status, culaGetErrorInfo(), buf, sizeof(buf));
 //    printf("%s\n", buf);

 //    culaShutdown();
 //    exit(EXIT_FAILURE);
 //}
 //int doCulaSgesv(float *aFInputMat, float *aFOutputInvMat, int i32N, int i32NRHS)
 //{
 //    culaStatus l_oStatus;

 //    // init cula
 //    l_oStatus = culaInitialize();
 //    // check error
 //    if(l_oStatus != culaNoError)
 //    {
 //        std::cerr << "Error cuda init : " << culaGetErrorInfo() << std::endl;
 //        return -1; // TODO : create a throw
 //    }

 //    int *l_aI32Ipiv = new int[i32N * sizeof(float)];

 //    // launch gpu computing
 //    l_oStatus = culaSgesv(i32N, i32NRHS, aFInputMat, i32N, l_aI32Ipiv, aFOutputInvMat, i32N);

 //    delete[] l_aI32Ipiv;

 //    if(l_oStatus != culaNoError)
 //    {
 //        std::cerr << "Error culaSgesv : " << culaGetErrorInfo() << std::endl;
 //        return -1; // TODO : create a throw
 //    }

 //    culaShutdown();

 //    return 0;
 //}

 //int solveAX_B(float *A, int i32SizeSquareMat)
 //{
 //    culaStatus l_oStatus;

 //    // init cula
 //    l_oStatus = culaInitialize();
 //    // check error
 //    if(l_oStatus != culaNoError)
 //    {
 //        std::cerr << "Error cuda init : " << culaGetErrorInfo() << std::endl;
 //        return -1; // TODO : create a throw
 //    }

 //    int *l_aI32Ipiv = new int[i32SizeSquareMat * i32SizeSquareMat * sizeof(float)];

 //    // launch gpu computing
 //    l_oStatus = culaSgetrf(i32SizeSquareMat, i32SizeSquareMat, A, i32SizeSquareMat, l_aI32Ipiv);

 //    if(l_oStatus != culaNoError)
 //    {
 //        std::cerr << "Error culaSgesv : " << culaGetErrorInfo() << std::endl;
 //        return -1; // TODO : create a throw
 //    }

 //    float *LU = A;

 //    float *L = LU;
 //    float *U = new float[i32SizeSquareMat * i32SizeSquareMat * sizeof(float)];

 //    for(int ii = 0; ii < i32SizeSquareMat; ++ii)
 //    {
 //        for(int jj = 0; jj < i32SizeSquareMat; ++jj)
 //        {
 //            if(ii < jj)
 //            {
 //                U[ii*i32SizeSquareMat + jj] = 0.f;
 //            }
 //            else if(jj > ii)
 //            {
 //                U[ii*i32SizeSquareMat + jj] = LU[ii*i32SizeSquareMat + jj];
 //                L[ii*i32SizeSquareMat + jj] = 0.f;
 //            }
 //            else
 //            {
 //                L[ii*i32SizeSquareMat + jj] = 1.f;
 //                U[ii*i32SizeSquareMat + jj] = LU[ii*i32SizeSquareMat + jj];
 //            }
 //        }
 //    }


 ////    l_oStatus = culaSgesv(i32SizeSquareMat, i32SizeSquareMat, aFInputMat, i32SizeSquareMat, l_aI32Ipiv, aFOutputInvMat, i32SizeSquareMat);



 //    delete[] l_aI32Ipiv;
 //}

 //culaSgesv





 //  void _decomposeLU3(float *src_d, int n)
 //  {
 //      cudaError_t cudaStat;
 //      cublasStatus stat;
 //      cublasHandle_t handle;

 //      int batchSize = n;
 //      int *P, *INFO;
 //      cudaStat = cudaMalloc<int>(&P,n * batchSize * sizeof(int));
 //      printf("malloc1 : %d\n", cudaStat);
 //      cudaStat = cudaMalloc<int>(&INFO, batchSize * sizeof(int));
 //      printf("malloc2 : %d\n", cudaStat);

 //      float *A[] = {src_d};
 // //     float *LU[] = {dst_d};

 //      printf("A sizeof %d\n", sizeof(A));

 //      float** A_d;


 //      cudaStat = cudaMalloc<float*>(&A_d, sizeof(A));
 //      printf("malloc3 : %d\n", cudaStat);

 //      cudaStat =cudaMemcpy(A_d, A, sizeof(A), cudaMemcpyHostToDevice);
 //      printf("memcopy : %d\n", cudaStat);

 //      cublasStatus_t statut =
 //              cublasSgetrfBatched(handle,    // handle to the CUBLAS library context.
 //                                  n,         // number of rows and columns of Aarray[i].
 //                                  A_d,       // array of pointers to <type> array, with each array of dim. n x n with lda>=max(1,n).
 //                                  n,         // leading dimension of two-dimensional array used to store each matrix Aarray[i].
 //                                  P,         // array of size n x batchSize that contains the pivoting sequence of each factorization of Aarray[i] stored in a linear fashion.
 //                                  INFO,      // array of size batchSize that info(=infoArray[i]) contains the information of factorization of Aarray[i].
 //                                  batchSize  // number of pointers contained in A
 //                                 );
 //      printf("statut : %d\n", statut);

 //      int INFOh = 0;
 //      cudaMemcpy(&INFOh,INFO,sizeof(int),cudaMemcpyDeviceToHost);
 //      printf("Info : %d\n", INFOh);

 // //     cudaMemcpy(LU, A_d, sizeof(A), cudaMemcpyHostToDevice);
 //      cudaMemcpy(A, A_d, sizeof(A), cudaMemcpyHostToDevice);

 //      cudaFree(A_d);
 //      cudaFree(P);
 //      cudaFree(INFO);
 //  }
