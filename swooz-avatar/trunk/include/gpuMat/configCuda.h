/*******************************************************************************
**                                                                            **
**  SWoOz is a software platform written in C++ used for behavioral           **
**  experiments based on interactions between people and robots               **
**  or 3D avatars.                                                            **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
** *****************************************************************************
**          Authors: Guillaume Gibert, Florian Lance                          **
**  Website/Contact: http://swooz.free.fr/                                    **
**       Repository: https://github.com/GuillaumeGibert/swooz                 **
********************************************************************************/


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

    #define RADIX_BASE 10
    #define RADIX_BITS 32

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
