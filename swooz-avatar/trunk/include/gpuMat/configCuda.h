
/**
 * \file configCuda.h
 * \brief defines CUDA configuration values
 * \author Florian Lance
 * \date 27/09/13
 */

#ifndef _CONFIGCUDA_
#define _CONFIGCUDA_

//namespace swUtil
//{
    #define BLOCKSIZE 16 // 16
    #define BLOCKSIZEMINUS1 15

    #define USELOOPUNROLLING 1
    #define AVOIDBANKCONFLICTS 0  



    // Matrices are stored in row-major order:
    // M(row, col) = *(M.elements + row * M.width + col)
    typedef struct {
      int width;
      int height;
      float* elements;
      int stride;
    } Matrix;





//}

#endif
