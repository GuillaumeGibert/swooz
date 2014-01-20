

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
