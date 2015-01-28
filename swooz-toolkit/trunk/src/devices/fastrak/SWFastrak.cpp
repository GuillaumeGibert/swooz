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
 * \file SWFastrak.cpp
 * \brief Defines SWFastrak
 * \author Florian Lance
 * \date 12/06/13
 */

#include "devices/fastrak/SWFastrak.h"
#include <iostream>

using namespace std;
using namespace swDevice;

/**
 * \struct SWKBinHdr
 * \brief Polhemus Fastrak head gaze data.
 */
struct SKBinHdr
{
	char m_cRecType;  /**< record type */
	char m_cStation;  /**< station */
	char m_cErr;	  /**< error */
};

SWFastrak::SWFastrak()
{
	for(uint ii = 0; ii < 3; ++ii)
	{
		m_aFRotations[ii]    = 0;
		m_aFTranslations[ii] = 0;
	}
}

SWFastrak::~SWFastrak(void)
{
	if(!m_bDisconnected)
	{
		stop();
	}
}


bool SWFastrak::init()
{
	m_oFastrak.ConnectUSB();
	int l_i32IsReady = m_oFastrak.CnxReady();
	
	m_oFastrak.SetMetric(true); // metric system values
	
	if(!l_i32IsReady)
	{
		cerr << "Warning : fastrak not detected. " << endl;
		return false;
	}
	else
	{
		cout << "Data from fastrak available." << endl;
		m_bDisconnected = false;
		return true;
	}
}

void SWFastrak::stop()
{
	m_oFastrak.Disconnect();
	m_bDisconnected = true;
}

bool SWFastrak::read()
{
	if(m_bDisconnected)
	{
		return false;
	}
	
	unsigned long l_ulSize;
	unsigned char *l_ucBuf;
	
	if(!m_oFastrak.ReadSinglePnoBuf(l_ucBuf, l_ulSize))
	{
		cerr << "Error grab polhemus fastrak. " << endl;
	}
	else
	{
		uint kk = 0;
		SKBinHdr *l_CBinHdr = (SKBinHdr*)(&l_ucBuf[kk]);
		kk += sizeof(SKBinHdr);
		
		float *l_aFPNO = (float*)(&l_ucBuf[kk]);
		
        m_aFTranslations[0] =  l_aFPNO[0];
		m_aFTranslations[1]	=  l_aFPNO[1];
		m_aFTranslations[2]	=  l_aFPNO[2];
		m_aFRotations[0]	=  l_aFPNO[3];
		m_aFRotations[1] 	=  l_aFPNO[4];
		m_aFRotations[2] 	=  l_aFPNO[5];
	}
	
	return true;
}
