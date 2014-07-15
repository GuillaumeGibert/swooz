

/**
 * \file geometryUtility.h
 * \brief geometry functions definitions using only std
 * \author Florian Lance
 * \date 05/11/12
 */

#ifndef _SWGEOMETRYUTILITY_
#define _SWGEOMETRYUTILITY_


#include <iostream>
#include "commonTypes.h"
#include "math.h"
#include <vector>

namespace swUtil
{
    #define PI 3.14159265359

    template <typename T>
    static void inverse(std::vector<T> &v)
    {
        if(v.size() == v.size())
        {
            for(uint ii = 0; ii < v.size(); ++ii)
            {
                v[ii] = -v[ii];
            }
        }
    }

    template <typename T>
    static void add(std::vector<T> &v1, const std::vector<T> &v2)
    {
        if(v1.size() == v2.size())
        {
            for(uint ii = 0; ii < v1.size(); ++ii)
            {
                v1[ii] += v2[ii];
            }
        }
    }

    template <typename T>
    static std::vector<T> mul(const std::vector<T> &v, const T dVal)
    {
        std::vector<T> l_v;

        for(uint ii = 0; ii < v.size(); ++ii)
        {
            l_v.push_back(v[ii] * dVal);
        }

        return l_v;
    }

    template <typename T>
    static std::vector<T> mul(const std::vector<T> &v1, const std::vector<T> &v2)
    {
        std::vector<T> l_v3;

        if(v1.size() == v2.size())
        {
            for(uint ii = 0; ii < v1.size(); ++ii)
            {
                l_v3.push_back(v1[ii] * v2[ii]);
            }
        }

        return l_v3;
    }

    template <typename T>
    static std::vector<T> vec(const std::vector<T> &v1, const std::vector<T> &v2)
    {
        std::vector<T> l_v3(v1.size());

        if(v1.size() == v2.size())
        {
            for(uint ii = 0; ii < v1.size(); ++ii)
            {
                l_v3[ii] = v2[ii] - v1[ii];
            }
        }

        return l_v3;
    }

    template <typename T>
    static void vec(const std::vector<T> &v1, const std::vector<T> &v2, std::vector<T> &vec)
    {
        vec = std::vector<T>(v1.size());

        if(v1.size() == v2.size())
        {
            for(uint ii = 0; ii < v1.size(); ++ii)
            {
                vec[ii] = v2[ii] - v1[ii];
            }
        }
    }

    template <typename T>
    static T squareLength(const std::vector<T> &vD)
    {
        T l_TSquareLength = 0;
        for(uint ii = 0; ii < vD.size(); ++ii)
        {
            l_TSquareLength += vD[ii] * vD[ii];
        }
        return l_TSquareLength;
    }


    template <typename T>
    static T norm(const std::vector<T> &vD)
    {
        T l_TNorm = 0;
        for(uint ii = 0; ii < vD.size(); ++ii)
        {
            l_TNorm += vD[ii] * vD[ii];
        }
        return sqrt(l_TNorm);
    }

    template <typename T>
    static void normalize(std::vector<T> &vT)
    {
        T l_TNorm = norm(vT);
        for(uint ii = 0; ii < vT.size(); ++ii)
        {
            vT[ii] /= l_TNorm;
        }
    }

    template <typename T>
    static T dotProduct(const std::vector<T> &v1, const std::vector<T> &v2)
    {
        T l_TRes = 0;

        if(v1.size() == v2.size())
        {
            for(uint ii = 0; ii < v1.size(); ++ii)
            {
                l_TRes += v1[ii] * v2[ii];
            }
        }

        return l_TRes;
    }

    template <typename T>
    static std::vector<T> crossProduct(const std::vector<T> &v1, const std::vector<T> &v2)
    {
        std::vector<T> l_vRes;

        if(v1.size() == v2.size() && v1.size() == 3)
        {
            l_vRes.push_back(v2[1] * v1[2] - v2[2] * v1[1]);
            l_vRes.push_back(v2[2] * v1[0] - v2[0] * v1[2]);
            l_vRes.push_back(v2[0] * v1[1] - v2[1] * v1[0]);
        }

        return l_vRes;
    }

    template <typename T>
    static double vectorAngle(const std::vector<T> &v1, const std::vector<T> &v2)
    {
        return acos(dotProduct(v1,v2)/((double)norm(v1)*(double)norm(v2))) * 180.0 / PI;
    }


    /**
     * \brief Check if a 3D intersection exist between the input segment and triangle.
     * \param [in] vP   : first point of the segment
     * \param [in] vD   : second point of the segment
     * \param [in] vV0  : first point of the triangle
     * \param [in] vV1  : second point of the triangle
     * \param [in] vV2  : third point of the triangle
     * \return -1 triangle is degenerate (a segment or point),
     *          0 disjoint (no intersect)
     *          1 intersect in unique point
     *          2 are in the same plane
     */
    template <typename T>
    static int segmentTriangleIntersect(const std::vector<T> &vP,  const std::vector<T> &vD,
                                        const std::vector<T> &vV0, const std::vector<T> &vV1, const std::vector<T> &vV2)
    {
        std::vector<T> l_vU = vec(vV0, vV1); // triangle vectors
        std::vector<T> l_vV = vec(vV0, vV2);
        std::vector<T> l_vN  = crossProduct(l_vU, l_vV);

        if(l_vN[0] == 0.0 && l_vN[1] == 0.0 && l_vN[2] == 0.0)
        {
            return -1; // triangle is degenerate
        }

        std::vector<T> l_vDir = vec(vP, vD); // ray direction vector
        std::vector<T> l_vW0  = vec(vV0, vP);

        T l_TA = - dotProduct(l_vN, l_vW0);
        T l_TB =   dotProduct(l_vN, l_vDir);

        if(fabs(l_TB) < 0.00000001) // ray is  parallel to triangle plane
        {
            if(l_TA == 0)
            {
                return 0; // ray lies in triangle plane
            }
            else
            {
                return 2; // ray disjoint from plane
            }
        }

        // get intersect point of ray with triangle plane
        T l_TR = l_TA / l_TB;

        if(l_TR < 0.0) // ray goes away from triangle
        {
            return 0; // no intersect
        }

        if(l_TR > 1.0) // TODO : add an option to check only for rays
        {
            return 0; // segment test => no intersect
        }

        std::vector<T> l_vIntersectPoint = mul(l_vDir, l_TR);
        add(l_vIntersectPoint, vP);  // intersect point of ray and plane

        // is I inside T?
        T l_TUU = dotProduct(l_vU, l_vU);
        T l_TUV = dotProduct(l_vU, l_vV);
        T l_TVV = dotProduct(l_vV, l_vV);

        std::vector<T> l_vW(3,0.0);
        add(l_vW, vV0);
        inverse(l_vW);
        add(l_vW, l_vIntersectPoint);

        T l_TWU = dotProduct(l_vW, l_vU);
        T l_TWV = dotProduct(l_vW, l_vV);
        T l_TD  = l_TUV * l_TUV - l_TUU * l_TVV;

        // get and test parametric coords
        T l_TS = (l_TUV * l_TWV - l_TVV * l_TWU) / l_TD;
        if(l_TS <= 0.0 || l_TS >= 1.0)
        {
            return 0; // intersection point is outside
        }
        T l_TT = (l_TUV * l_TWU - l_TUU * l_TWV) / l_TD;
        if(l_TT <= 0.0 || (l_TS + l_TT >= 1.0))
        {
            return 0; // intersection point is outside
        }

        return true; // intersection point is inside
    }
	
	/*
	 * \brief Calculates roll-pitch-yaw angles between two vectors
	 * \param [in] vecAxis     : ...
	 * \param [in] vecRotation : ...
	 */
	template <typename T>
	std::vector<T> computeRollPitchYaw(const std::vector<T> & vecAxis, const std::vector<T> & vecRotation)
	{
		std::vector<T> vecUp	(3);	vecUp	[0] = 0.;	vecUp	[1] = 1.;	vecUp	[2] = 0.;
		std::vector<T> vecLeft	(3);	vecLeft	[0] = 1.;	vecLeft	[1] = 0.;	vecLeft	[2] = 0.;

		std::vector<T> vecAxisX (vecAxis);	vecAxisX[0] = 0.;
		std::vector<T> vecAxisZ (vecAxis);	vecAxisZ[2] = 0.;

		std::vector<T> roll_pitch_yaw (3);

		roll_pitch_yaw[1]	= (vecAxisX[2]>=0?-1:1) * swUtil::vectorAngle(vecAxisX, vecUp);
		roll_pitch_yaw[0]	= (vecAxisZ[0]>=0?-1:1) * swUtil::vectorAngle(vecAxisZ, vecUp);
		roll_pitch_yaw[2]	= (vecRotation[1]>=0?1:-1) * swUtil::vectorAngle(vecRotation, vecLeft);

		return roll_pitch_yaw;

	}

    /*
     * \brief Rad to degree conversion.
     * \param [in] An angle in rad
     * \return The same angle in degrees
     */
    template <typename T>
    T rad2Deg(T angle)
    {
        return angle * static_cast<T>(180 / PI);
    }

    /*
     * \brief Sets the value of an angle in degrees to the value of the same angle between -180 and 180 degrees.
     * \param [in] An angle in degrees
     * \return The same angle in degrees, bound between -180 and 180.
     */
    template <typename T>
    T degree180(T angle)
    {
        return angle > 180 ? degree180(angle - 360) : (angle <= -180 ? degree180(angle + 360) : angle);
    }
}

#endif
