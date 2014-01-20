

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


#endif