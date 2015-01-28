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
 * \file SWFaceLab.cpp
 * \brief Defines SWFaceLab
 * \author Florian Lance
 * \date 13/06/13
 */


#include "devices/faceLab/SWFaceLab.h"

using namespace swDevice;
using namespace swExcept;
using namespace FaceLabTracker; 

SWFaceLab::SWFaceLab() : m_pFaceLabModule(new CFaceLab(2002))
{}

SWFaceLab::~SWFaceLab()
{
	if(!m_bDisconnected)
	{
		stop();
	}
	
	if(m_pFaceLabModule)
	{
		delete m_pFaceLabModule;
	}
}


bool SWFaceLab::init()
{
	m_pFaceLabModule->init();
	
	// wait for initial data
	uint64_t l_ui64LastUpdate = 0;
	int l_i32TimeOut = 0;
	
	while(m_pFaceLabModule->getLastUpdate() <= l_ui64LastUpdate)
	{
		if(l_i32TimeOut > 10)
		{
			std::cerr << "Time out waiting for faceLab device. " << std::endl;
			throw swFaceLabError();
			return false; // if exception not catched
		}
		
		boost::this_thread::sleep( boost::posix_time::seconds(1) );
		std::cout << "Waiting for facelab device..." << std::endl;
		
		l_i32TimeOut++;
	}
	
	std::cout << "Data from FaceLab available." << std::endl;
	m_bDisconnected = false;
	
	return true;
}

void SWFaceLab::stop()
{
	if(!m_bDisconnected)
	{
		m_pFaceLabModule->close();
		m_bDisconnected = true;
		
		std::cout << "Facelab disconnected. " << std::endl;
	}
}

bool SWFaceLab::read()
{
	if(!m_bDisconnected)
	{
		m_oHeadGazeData = m_pFaceLabModule->grabData();
		return true;
	}
	else
	{
		return false;
	}
}
