

/**
 * \file SWiCubFaceMotion.h
 * \brief Defines useful functions to get bottle command strings to control the icub face motion with input face specific points.
 * \author Florian Lance
 * \date 31/05/13
 */

#ifndef _SWICUBFACEMOTION_
#define _SWICUBFACEMOTION_

#include "commonTypes.h"
#include <iostream>


namespace swIcub
{	
    static double m_dLeftThresholdDown    = 0.061;
    static double m_dLeftThresholdNeutral = 0.068;
    static double m_dLeftThresholdUp1     = 0.074;
    static double m_dLeftThresholdUp2     = 0.085;

    static double m_dRightThresholdDown    = 0.061;
    static double m_dRightThresholdNeutral = 0.068;
    static double m_dRightThresholdUp1     = 0.074;
    static double m_dRightThresholdUp2     = 0.085;

	static double m_dThresholdMouth   = 0.01;		

	static std::string lipCommand(const std::vector<double> &a3DLip1, const std::vector<double> &a3DLip2)
	{
		if(a3DLip1.size() != a3DLip2.size() && a3DLip1.size() != 3)
		{
			std::cerr << "Error size input vectors : lipCommand. " << std::endl;
			return std::string("M0A");
		}
		
		double l_dLipsDistance = sqrt( pow(a3DLip2[0] - a3DLip1[0], 2) + pow(a3DLip2[1] - a3DLip1[1], 2) + pow(a3DLip2[2] - a3DLip1[2], 2));
		
		//std::cout << " [l->" <<l_dLipsDistance << "] " << std::endl;
		
		if(l_dLipsDistance > m_dThresholdMouth)
		{
			return std::string("M16");
		}
		else
		{
			return std::string("M08");
		}
	}

    static std::string eyeBrowCommand(const std::vector<double> &a3DEyeBrow, const std::vector<double> &a3DEyeCenter, const bool bLeftEye)
	{
		double l_dEyeDistance = sqrt( pow(a3DEyeBrow[0] - a3DEyeCenter[0], 2) + pow(a3DEyeBrow[1] -
						a3DEyeCenter[1], 2) + pow(a3DEyeBrow[2] - a3DEyeCenter[2], 2));		

        bool l_bDown, l_bNeutral, l_bUp1, l_bUp2;

        if(bLeftEye)
        {
            l_bDown     = l_dEyeDistance < m_dLeftThresholdDown;
            l_bNeutral  = l_dEyeDistance < m_dLeftThresholdNeutral;
            l_bUp1      = l_dEyeDistance < m_dLeftThresholdUp1;
            l_bUp2      = l_dEyeDistance < m_dLeftThresholdUp2;
        }
        else
        {
            l_bDown     = l_dEyeDistance < m_dRightThresholdDown;
            l_bNeutral  = l_dEyeDistance < m_dRightThresholdNeutral;
            l_bUp1      = l_dEyeDistance < m_dRightThresholdUp1;
            l_bUp2      = l_dEyeDistance < m_dRightThresholdUp2;
        }

		//std::cout << " [e"<< bLeftEye << "->" <<l_dEyeDistance << "] " << std::endl;
		//std::cout  <<l_dEyeDistance << std::endl;
		
		if(l_bDown)
		{
			return std::string("01");
		}
		if(l_bNeutral)
		{
			return std::string("02");
		}
		if(l_bUp1)
		{
			return std::string("04");
		}
		if(l_bUp2)
		{
			return std::string("04");
		}
		
		std::cerr << "Bad values input points eyeBrowCommand " << std::endl;
		
		return std::string("02");
	}
	
	static std::string leftEyeBrowCommand(const std::vector<double> &a3DLeftEyeBrow, const std::vector<double> &a3DLeftEyeCenter)
	{
		if(a3DLeftEyeBrow.size() != a3DLeftEyeCenter.size() && a3DLeftEyeCenter.size() != 3)
		{
			std::cerr << "Error size input vectors : leftEyeBrowCommand. " << std::endl;
			return std::string("");
		}		
		
        std::string l_sEyeBrowCommand = eyeBrowCommand(a3DLeftEyeBrow, a3DLeftEyeCenter, true);
		
		if(l_sEyeBrowCommand.size() == 0)
		{
			return std::string("");
		}
		
		return std::string("L") + l_sEyeBrowCommand;	
	}
	
	static std::string rightEyeBrowCommand(const std::vector<double> &a3DRightEyeBrow, const std::vector<double> &a3DRightEyeCenter)
	{
		if(a3DRightEyeBrow.size() != a3DRightEyeCenter.size() && a3DRightEyeCenter.size() != 3)
		{
			std::cerr << "Error size input vectors : rightEyeBrowCommand. " << std::endl;
			return std::string("");
		}
		
        std::string l_sEyeBrowCommand = eyeBrowCommand(a3DRightEyeBrow, a3DRightEyeCenter, false);
		
		if(l_sEyeBrowCommand.size() == 0)
		{
			return std::string("");
		}
		
		return std::string("R") + l_sEyeBrowCommand;
	}	

}

#endif
