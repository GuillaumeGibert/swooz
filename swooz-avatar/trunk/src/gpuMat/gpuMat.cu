
#include <cutil.h>
#include "cula.h"
#include <iostream>

#include "configCuda.h"


//using namespace swUtil;
void cudaDummyCall(int argc, char **argv)
{
    // init CUDA
    //CUT_DEVICE_INIT(argc, argv);
    cudaFree(0);
}

int inverseMatSgesv(float *aFInputMat, float *aFOutputInvMat, int i32SizeSquareMat)
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

// Matrix multiplication kernel called by MatMul()
__global__ void MatMulKernel(Matrix A, Matrix B, Matrix C)
{
    // Each thread computes one element of C
    // by accumulating results into Cvalue
    float Cvalue = 0.f;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if(row > A.height || col > B.width)
        return;

    for (int e = 0; e < A.width; ++e)
        Cvalue += (A.elements[row * A.width + e]) * (B.elements[e * B.width + col]);
    C.elements[row * C.width + col] = Cvalue;
}

// Matrix multiplication - Host code
// Matrix dimensions are assumed to be multiples of BLOCK_SIZE
void matMult(const Matrix A, const Matrix B, Matrix C)
{
    // Load A and B to device memory
    Matrix d_A;
    d_A.width = A.width;
    d_A.height = A.height;
    size_t size = A.width * A.height * sizeof(float);
    cudaError_t err = cudaMalloc(&d_A.elements, size);
//    printf("CUDA malloc A: %s\n",cudaGetErrorString(err));
    err = cudaMemcpy(d_A.elements, A.elements, size, cudaMemcpyHostToDevice);
//    printf("Copy A to device: %s\n",cudaGetErrorString(err));

    Matrix d_B;
    d_B.width = B.width;
    d_B.height = B.height;
    size = B.width * B.height * sizeof(float);
    err = cudaMalloc(&d_B.elements, size);
//    printf("CUDA malloc B: %s\n",cudaGetErrorString(err));
    err = cudaMemcpy(d_B.elements, B.elements, size, cudaMemcpyHostToDevice);
//    printf("Copy B to device: %s\n",cudaGetErrorString(err));

    // Allocate C in device memory
    Matrix d_C;
    d_C.width = C.width;
    d_C.height = C.height;
    size = C.width * C.height * sizeof(float);
    err = cudaMalloc(&d_C.elements, size);
//    printf("CUDA malloc C: %s\n",cudaGetErrorString(err));

    // Invoke kernel
    dim3 dimBlock(BLOCKSIZE, BLOCKSIZE);
    dim3 dimGrid((B.width + dimBlock.x - 1) / dimBlock.x,
           (A.height + dimBlock.y - 1) / dimBlock.y);

    MatMulKernel<<<dimGrid, dimBlock>>>(d_A, d_B, d_C);
    err = cudaThreadSynchronize();
//    printf("Run kernel: %s \n", cudaGetErrorString(err));

    // Read C from device memory
    err = cudaMemcpy(C.elements, d_C.elements, size, cudaMemcpyDeviceToHost);
//    printf("Copy C off of device: %s\n",cudaGetErrorString(err));

    // Free device memory
    cudaFree(d_A.elements);
    cudaFree(d_B.elements);
    cudaFree(d_C.elements);
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

