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
 * \file commonTypes.h
 * \brief common types definitions and othres utility functions
 * \author Florian Lance
 * \date 05/11/12
 */

#ifndef _COMMONTYPES_
#define _COMMONTYPES_

#include <limits.h>
#include <float.h>
#include <string>
#include <vector>

#define NULL 0 /**< define NULL to 0*/

// unsigned
typedef unsigned char uchar;        /**< typedef for unsigned char */
typedef unsigned short ushort;      /**< typedef for unsigned short */
typedef unsigned int uint;          /**< typedef for unsigned int */
typedef unsigned long ulong;        /**< typedef for unsigned long */
typedef unsigned long long ullong;  /**< typedef for unsigned long long */

// bits
typedef uchar uint8;                /**< typedef for unsigned char*/
typedef ushort uint16;              /**< typedef for unsigned short */
typedef short int16;                /**< typedef for short */
typedef uint uint32;                /**< typedef for unsigned int */
typedef int int32;                  /**< typedef for int */
typedef ullong uint64;              /**< typedef for unsigned long long */
typedef signed long long int64;     /**< typedef for signed long long */

// const
//      base
typedef const bool cbool;           /**< typedef for const bool */
typedef const char cchar;           /**< typedef for const char */
typedef const int cint;             /**< typedef for const int */
typedef const long clong;           /**< typedef for const long */
typedef const float cfloat;         /**< typedef for const float */
typedef const double cdouble;       /**< typedef for const double */
//      unsigned
typedef const uchar cuchar;         /**< typedef for const unsigned char */
typedef const uint cuint;           /**< typedef for const unsigned int */
typedef const ulong culong;         /**< typedef for const unsigned long */
typedef const ullong cullong;       /**< typedef for const unsigned long long */
//      bits
typedef const uint8 cuint8;         /**< typedef for const unsigned char */
typedef const uint16 cuint16;       /**< typedef for const unsigned short */
typedef const int16 cint16;         /**< typedef for const short */
typedef const uint32 cuint32;       /**< typedef for const unsigned int */
typedef const int32 cint32;         /**< typedef for const int */
typedef const uint64 cuint64;       /**< typedef for const unsigned long long */
typedef const int64 cint64;         /**< typedef for const signed long long */


/**
 * \brief Delete the input object and set the value of the pointer to NULL
 * \param [in,out] pVal : object to be deleted
 */
template< class T > static void deleteAndNullify ( T*& pVal )
{
    if(pVal)
    {
        delete pVal;
    }

    pVal = NULL;
}

/**
 * \brief Delete the input array of objects and set the value of the pointer to NULL
 * \param [in,out] pVal : array of objects to be deleted
 */
template< class T > static void deleteAndNullifyArray( T*& pVal )
{
    if(pVal)
    {
        delete[] pVal;
    }

    pVal = NULL;
}

/**
 * \brief Check if the value is not equal to zero
 * \param [in] i32Val : value to be checked
 * \return the iput value if not equal to zero, else return 1
 */
static int check0Div(cint i32Val)
{
    if(!i32Val)
    {
        return 1;
    }

    return i32Val;
}

/**
 * \brief Check if the value is not equal to zero
 * \param [in] fVal : value to be checked
 * \return the iput value if not equal to zero, else return FLT_MIN
 */
static float check0Div(cfloat fVal)
{
    if(!fVal)
    {
        return FLT_MIN;
    }

    return fVal;
}

/**
 * \brief Check if the value is not equal to zero
 * \param [in] dVal : value to be checked
 * \return the iput value if not equal to zero, else return DBL_MIN
 */
static double check0Div(cdouble dVal)
{
    if(!dVal)
    {
        return DBL_MIN;
    }

    return dVal;
}


template< class T >
static void displayDebug(const std::string &sNameValue, const T vValue)
{
    std::cout << sNameValue << " : " << vValue << " | ";
}

template< class T >
static void displayVectorDebug(const std::string &sNameVector, const std::vector<T> &vVector)
{
    std::cout << sNameVector << " : ";
    for(uint ii = 0; ii < vVector.size(); ++ii)
    {
        std::cout << vVector[ii] << " ";
    }
    std::cout << std::endl;
}



#endif
