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
 * \file SWOculus.cpp
 * \brief Defines SWOculus
 * \author Guillaume Gibert
 * \date 28/05/15
 */

#include "devices/oculus/SWOculus.h"
#include <iostream>

using namespace std;
using namespace swDevice;



SWOculus::SWOculus()
{
	for(uint ii = 0; ii < 3; ++ii)
	{
		m_aFRotations[ii]    = 0;
		m_aFTranslations[ii] = 0;
	}
}

SWOculus::~SWOculus(void)
{
	if(!m_bDisconnected)
	{
		stop();
	}
}


bool SWOculus::init()
{
	cout << "Oculus system:" << endl;
	
	if (ovr_Initialize(nullptr) == ovrSuccess)
	{
		cout << "   --> Initialized" << endl;
		m_ohmd    = nullptr;
		ovrResult result = ovrHmd_Create(0, &m_ohmd);
		
		
		if (result == ovrSuccess)
		{
			cout << "   --> Created" << endl;
			if (ovrHmd_ConfigureTracking(m_ohmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0) == ovrSuccess) 
			{
				cout << "   --> Data available" << endl;
				m_bDisconnected = false;
				return true;
			}
			else
			{
				cerr << "   --> Unable to detect Rift head tracker"<< endl;
				return false;
			}
		}
		else
		{
			cerr << "   --> Unable to detect Oculus system"<< endl;
			return false;
		}
		 
	}
	else
	{
		cerr << "   --> Unable to initialize Oculus system"<< endl;
		return false;
	}
}

void SWOculus::stop()
{
	ovrHmd_Destroy(m_ohmd);
	ovr_Shutdown();
	m_bDisconnected = true;
}

bool SWOculus::read()
{
	if(m_bDisconnected)
	{
		return false;
	}
	
	ovrTrackingState state = ovrHmd_GetTrackingState(m_ohmd, 0);
	
	if (state.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) 
	{
		OVR::Quatf orientationq = state.HeadPose.ThePose.Orientation;
		float yaw, eyePitch, eyeRoll;
		orientationq.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &eyePitch, &eyeRoll);
		
		m_aFRotations[0]		=  yaw * RADIANS_TO_DEGREES;
		m_aFRotations[1] 	=  eyePitch * RADIANS_TO_DEGREES;
		m_aFRotations[2] 	=  eyeRoll * RADIANS_TO_DEGREES;
		
		m_aFTranslations[0] 	=  state.HeadPose.ThePose.Position.x;
		m_aFTranslations[1]	=  state.HeadPose.ThePose.Position.y;
		m_aFTranslations[2]	=  state.HeadPose.ThePose.Position.z;
		
	}

	return true;
}
