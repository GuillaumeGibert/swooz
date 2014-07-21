
/**
 * \file SWFastrak_thread.cpp
 * \brief Defines SWFastrak_thread
 * \author Florian Lance
 * \date 20/06/13
 */

#include "devices/fastrak/SWFastrak_thread.h"

#include "SWExceptions.h"

using namespace std;
using namespace swDevice;
using namespace swExcept;

SWFastrak_thread::SWFastrak_thread() : m_bInitialized(false), m_bDataAvailable(false)
{}

SWFastrak_thread::~SWFastrak_thread(void)
{
	if(m_bListening)
	{
		stopListening();
	}
}

bool SWFastrak_thread::init()
{	
	m_bInitialized = m_oFastrak.init();
	
	return m_bInitialized;
}


void SWFastrak_thread::startListening()
{
	if(m_bInitialized)
	{
		m_bListening 	   = true;
		m_pListeningThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&SWFastrak_thread::doWork, this)));	
		
		int l_i32WaitTimeOut = 0;

		while(!isDataAvailable())
		{			            
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));

			if(l_i32WaitTimeOut % 10 == 0)
			{
				std::cout << "Waiting for fastrak data... " << std::endl;
			}
			if(l_i32WaitTimeOut > 500)
			{
				std::cerr << "Timeout... " << std::endl;
				throw fastrakDataTimeOutError();
			}

			++l_i32WaitTimeOut;
		}
	}
	else
	{
		std::cerr << "Fastrak module not initialized, the thread can't be started. " << std::endl;
		throw fastrakInitError();		
	}
}

void SWFastrak_thread::stopListening()
{
	if(m_bListening)
	{
		m_bListening 	 = false;
		m_bDataAvailable = false;
		m_pListeningThread->join();		
	}
}

void SWFastrak_thread::doWork()
{
	while(m_bListening)
	{			
		clock_t l_oBeforeTime = clock();
		
		if(m_oFastrak.read())
		{
			boost::lock_guard<boost::mutex> lock(m_oMutex);
			
			m_v3FRotations.clear();
			m_v3FTranslations.clear();
			
			for(uint ii = 0; ii < 3; ++ii)
			{
				m_v3FRotations.push_back(m_oFastrak.m_aFRotations[ii]);
				m_v3FTranslations.push_back(m_oFastrak.m_aFTranslations[ii]);
			}
			
			m_bDataAvailable	= true;
		}
	}
}

bool SWFastrak_thread::isDataAvailable()
{
	boost::lock_guard<boost::mutex> lock(m_oMutex);
	return m_bDataAvailable;
}

std::vector<float> SWFastrak_thread::rotations()
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


std::vector<float> SWFastrak_thread::translations()
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
