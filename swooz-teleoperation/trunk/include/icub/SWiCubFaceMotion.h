

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

    class SWIcubFaceLEDCommand
    {
        public :

            SWIcubFaceLEDCommand() : m_dLeftThresholdDown(0),m_dLeftThresholdNeutral(0),m_dLeftThresholdUp1(0),m_dLeftThresholdUp2(0),
                                     m_dRightThresholdDown(0),m_dRightThresholdNeutral(0),m_dRightThresholdUp1(0),m_dRightThresholdUp2(0),
                                     m_dThresholdMouth(0.01)
            {}

            void setNeutralPoints(const std::vector<double> &l_vLeftEyebrowPoints, const std::vector<double> &l_vRightEyebrowPoints,
                                  const std::vector<double> &l_vCenterLeftEye,    const std::vector<double> &l_vCenterRightEye)
            {
                double l_dLeftXMedian = 0.0, l_dLeftYMedian = 0.0;
                double l_dRightXMedian = 0.0, l_dRightYMedian = 0.0;

                for(uint ii = 0; ii < l_vLeftEyebrowPoints.size()/3; ++ii)
                {
                    l_dLeftXMedian += l_vLeftEyebrowPoints[ii*3];
                    l_dLeftYMedian += l_vLeftEyebrowPoints[ii*3+1];

                    l_dRightXMedian += l_vRightEyebrowPoints[ii*3];
                    l_dRightYMedian += l_vRightEyebrowPoints[ii*3+1];
                }

                l_dLeftXMedian /= 3.0; l_dLeftYMedian /= 3.0;
                l_dRightXMedian /= 3.0; l_dRightYMedian /= 3.0;

                double l_dLeftEyeSquareDist  = (l_vCenterLeftEye[0] - l_dLeftXMedian)*(l_vCenterLeftEye[0] - l_dLeftXMedian) + (l_vCenterLeftEye[1] - l_dLeftYMedian)*(l_vCenterLeftEye[1] - l_dLeftYMedian);
                double l_dRightEyeSquareDist = (l_vCenterRightEye[0] - l_dRightXMedian)*(l_vCenterRightEye[0] - l_dRightXMedian) + (l_vCenterRightEye[1] - l_dRightYMedian)*(l_vCenterRightEye[1] - l_dRightYMedian);

                m_dLeftThresholdUp2     = DBL_MAX;
                m_dLeftThresholdUp1     = 1.4 * l_dLeftEyeSquareDist;
                m_dLeftThresholdNeutral = 1.2 * l_dLeftEyeSquareDist;
                m_dLeftThresholdDown    = 0.8 * l_dLeftEyeSquareDist;

                m_dRightThresholdUp2     = DBL_MAX;
                m_dRightThresholdUp1     = 1.4 * l_dRightEyeSquareDist;
                m_dRightThresholdNeutral = 1.2 * l_dRightEyeSquareDist;
                m_dRightThresholdDown    = 0.8 * l_dRightEyeSquareDist;
            }

            std::string lipCommand(const std::vector<double> &a3DLip1, const std::vector<double> &a3DLip2)
            {
                if(a3DLip1.size() != a3DLip2.size() && a3DLip1.size() != 3)
                {
                    std::cerr << "Error size input vectors : lipCommand. " << std::endl;
                    return std::string("M0A");
                }

                double l_dLipsDistance = sqrt( pow(a3DLip2[0] - a3DLip1[0], 2) + pow(a3DLip2[1] - a3DLip1[1], 2) + pow(a3DLip2[2] - a3DLip1[2], 2));


                if(l_dLipsDistance > m_dThresholdMouth)
                {
                    return std::string("M16");
                }
                else
                {
                    return std::string("M08");
                }
            }

            std::string eyeBrowCommand(const std::vector<double> &a3DEyeBrow, const std::vector<double> &a3DEyeCenter, const bool bLeftEye)
            {
                double l_dXMedian = 0.0, l_dYMedian = 0.0;

                for(uint ii = 0; ii < a3DEyeBrow.size()/3; ++ii)
                {
                    l_dXMedian += a3DEyeBrow[ii*3];
                    l_dYMedian += a3DEyeBrow[ii*3+1];
                }

                l_dXMedian /= 3.0; l_dYMedian /= 3.0;

                double l_dEyeSquareDist  = (a3DEyeBrow[0] - l_dXMedian)*(a3DEyeBrow[0] - l_dXMedian) + (a3DEyeBrow[1] - l_dYMedian)*(a3DEyeBrow[1] - l_dYMedian);

                bool l_bDown, l_bNeutral, l_bUp1, l_bUp2;

                if(bLeftEye)
                {
                    l_bDown     = l_dEyeSquareDist < m_dLeftThresholdDown;
                    l_bNeutral  = l_dEyeSquareDist < m_dLeftThresholdNeutral;
                    l_bUp1      = l_dEyeSquareDist < m_dLeftThresholdUp1;
                    l_bUp2      = l_dEyeSquareDist < m_dLeftThresholdUp2;
                }
                else
                {
                    l_bDown     = l_dEyeSquareDist < m_dRightThresholdDown;
                    l_bNeutral  = l_dEyeSquareDist < m_dRightThresholdNeutral;
                    l_bUp1      = l_dEyeSquareDist < m_dRightThresholdUp1;
                    l_bUp2      = l_dEyeSquareDist < m_dRightThresholdUp2;
                }

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
                    return std::string("03");
                }
                if(l_bUp2)
                {
                    return std::string("04");
                }

                std::cerr << "Bad values input points eyeBrowCommand : "<< l_dEyeSquareDist << std::endl;

                return std::string("02");
            }

            std::string leftEyeBrowCommand(const std::vector<double> &a3DLeftEyeBrow, const std::vector<double> &a3DLeftEyeCenter)
            {
                if(a3DLeftEyeBrow.size() != 9 && a3DLeftEyeCenter.size() != 3)
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

             std::string rightEyeBrowCommand(const std::vector<double> &a3DRightEyeBrow, const std::vector<double> &a3DRightEyeCenter)
            {
                if(a3DRightEyeBrow.size() != 9 && a3DRightEyeCenter.size() != 3)
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


        private :

            double m_dLeftThresholdDown;    /**< ... */
            double m_dLeftThresholdNeutral; /**< ... */
            double m_dLeftThresholdUp1;     /**< ... */
            double m_dLeftThresholdUp2;     /**< ... */

            double m_dRightThresholdDown;   /**< ... */
            double m_dRightThresholdNeutral;/**< ... */
            double m_dRightThresholdUp1;    /**< ... */
            double m_dRightThresholdUp2;    /**< ... */

            double m_dThresholdMouth;       /**< ... */
    };

//    static double m_dLeftThresholdDown    = 0.061;
//    static double m_dLeftThresholdNeutral = 0.068;
//    static double m_dLeftThresholdUp1     = 0.074;
//    static double m_dLeftThresholdUp2     = 0.085;

//    static double m_dRightThresholdDown    = 0.061;
//    static double m_dRightThresholdNeutral = 0.068;
//    static double m_dRightThresholdUp1     = 0.074;
//    static double m_dRightThresholdUp2     = 0.085;

//	static double m_dThresholdMouth   = 0.01;

//	static std::string lipCommand(const std::vector<double> &a3DLip1, const std::vector<double> &a3DLip2)
//	{
//		if(a3DLip1.size() != a3DLip2.size() && a3DLip1.size() != 3)
//		{
//			std::cerr << "Error size input vectors : lipCommand. " << std::endl;
//			return std::string("M0A");
//		}
		
//		double l_dLipsDistance = sqrt( pow(a3DLip2[0] - a3DLip1[0], 2) + pow(a3DLip2[1] - a3DLip1[1], 2) + pow(a3DLip2[2] - a3DLip1[2], 2));
		
//		//std::cout << " [l->" <<l_dLipsDistance << "] " << std::endl;
		
//		if(l_dLipsDistance > m_dThresholdMouth)
//		{
//			return std::string("M16");
//		}
//		else
//		{
//			return std::string("M08");
//		}
//	}

//    static std::string eyeBrowCommand(const std::vector<double> &a3DEyeBrow, const std::vector<double> &a3DNoseCenter, const bool bLeftEye)
//	{
//        double l_dNoseDistance = sqrt( pow(a3DEyeBrow[0] - a3DNoseCenter[0], 2) + pow(a3DEyeBrow[1] -
//                        a3DNoseCenter[1], 2) + pow(a3DEyeBrow[2] - a3DNoseCenter[2], 2));

//        bool l_bDown, l_bNeutral, l_bUp1, l_bUp2;

//        if(bLeftEye)
//        {
//            l_bDown     = l_dNoseDistance < m_dLeftThresholdDown;
//            l_bNeutral  = l_dNoseDistance < m_dLeftThresholdNeutral;
//            l_bUp1      = l_dNoseDistance < m_dLeftThresholdUp1;
//            l_bUp2      = l_dNoseDistance < m_dLeftThresholdUp2;
//        }
//        else
//        {
//            l_bDown     = l_dNoseDistance < m_dRightThresholdDown;
//            l_bNeutral  = l_dNoseDistance < m_dRightThresholdNeutral;
//            l_bUp1      = l_dNoseDistance < m_dRightThresholdUp1;
//            l_bUp2      = l_dNoseDistance < m_dRightThresholdUp2;
//        }

//		//std::cout << " [e"<< bLeftEye << "->" <<l_dEyeDistance << "] " << std::endl;
//		//std::cout  <<l_dEyeDistance << std::endl;
		
//		if(l_bDown)
//		{
//			return std::string("01");
//		}
//		if(l_bNeutral)
//		{
//			return std::string("02");
//		}
//		if(l_bUp1)
//		{
//			return std::string("04");
//		}
//		if(l_bUp2)
//		{
//			return std::string("04");
//		}
		
//		std::cerr << "Bad values input points eyeBrowCommand " << std::endl;
		
//		return std::string("02");
//	}
	
//    static std::string leftEyeBrowCommand(const std::vector<double> &a3DLeftEyeBrow, const std::vector<double> &a3DNoseCenter)
//	{
//        if(a3DLeftEyeBrow.size() != a3DNoseCenter.size() && a3DNoseCenter.size() != 9)
//		{
//			std::cerr << "Error size input vectors : leftEyeBrowCommand. " << std::endl;
//			return std::string("");
//		}
		
//        std::string l_sEyeBrowCommand = eyeBrowCommand(a3DLeftEyeBrow, a3DNoseCenter, true);
		
//		if(l_sEyeBrowCommand.size() == 0)
//		{
//			return std::string("");
//		}
		
//		return std::string("L") + l_sEyeBrowCommand;
//	}
	
//    static std::string rightEyeBrowCommand(const std::vector<double> &a3DRightEyeBrow, const std::vector<double> &a3DNoseCenter)
//	{
//        if(a3DRightEyeBrow.size() != a3DNoseCenter.size() && a3DNoseCenter.size() != 9)
//		{
//			std::cerr << "Error size input vectors : rightEyeBrowCommand. " << std::endl;
//			return std::string("");
//		}
		
//        std::string l_sEyeBrowCommand = eyeBrowCommand(a3DRightEyeBrow, a3DNoseCenter, false);
		
//		if(l_sEyeBrowCommand.size() == 0)
//		{
//			return std::string("");
//		}
		
//		return std::string("R") + l_sEyeBrowCommand;
//	}

}

#endif
