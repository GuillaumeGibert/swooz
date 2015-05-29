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
 * \file SWOculus_thread.cpp
 * \brief Defines SWOculus_thread
 * \author Guillaume Gibert
 * \date 28/05/15
 */

#include "devices/oculus/SWOculus_thread.h"

#include "SWExceptions.h"

using namespace std;
using namespace swDevice;
using namespace swExcept;

SWOculus_thread::SWOculus_thread() : m_bInitialized(false), m_bDataAvailable(false)
{}

SWOculus_thread::~SWOculus_thread(void)
{
	if(m_bListening)
	{
		stopListening();
	}
}

bool SWOculus_thread::init()
{	
	m_bInitialized = m_oOculus.init();
	
	return m_bInitialized;
}


void SWOculus_thread::startListening()
{
	if(m_bInitialized)
	{
		m_bListening 	   = true;
		m_pListeningThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&SWOculus_thread::doWork, this)));	
		
		int l_i32WaitTimeOut = 0;

		while(!isDataAvailable())
		{			            
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));

			if(l_i32WaitTimeOut % 10 == 0)
			{
				std::cout << "Waiting for Oculus data... " << std::endl;
			}
			if(l_i32WaitTimeOut > 500)
			{
				std::cerr << "Timeout... " << std::endl;
				throw oculusDataTimeOutError();
			}

			++l_i32WaitTimeOut;
		}
	}
	else
	{
		std::cerr << "Oculus module not initialized, the thread can't be started. " << std::endl;
		throw oculusInitError();		
	}
}

void SWOculus_thread::stopListening()
{
	if(m_bListening)
	{
		m_bListening 	 = false;
		m_bDataAvailable = false;
		m_pListeningThread->join();		
	}
}

void SWOculus_thread::doWork()
{
	while(m_bListening)
	{			
		clock_t l_oBeforeTime = clock();
		
		if(m_oOculus.read())
		{
			boost::lock_guard<boost::mutex> lock(m_oMutex);
			
			m_v3FRotations.clear();
			m_v3FTranslations.clear();
			
			for(uint ii = 0; ii < 3; ++ii)
			{
				m_v3FRotations.push_back(m_oOculus.m_aFRotations[ii]);
				m_v3FTranslations.push_back(m_oOculus.m_aFTranslations[ii]);
			}
			
			m_bDataAvailable	= true;
		}
	}
}

bool SWOculus_thread::isDataAvailable()
{
	boost::lock_guard<boost::mutex> lock(m_oMutex);
	return m_bDataAvailable;
}

std::vector<float> SWOculus_thread::rotations()
{
	{
		boost::lock_guard<boost::mutex> lock(m_oMutex);
		
		if(m_v3FRotations.size() == 3)
		{
			return m_v3FRotations;
		}
	}
	
	return std::vector<float>(3,0.f);
}


std::vector<float> SWOculus_thread::translations()
{
	{
		boost::lock_guard<boost::mutex> lock(m_oMutex);
		
		if(m_v3FTranslations.size() == 3)
		{
			return m_v3FTranslations;
		}
	}
	
	return std::vector<float>(3,0.f);
}
