
/**
 * \file Tobii.cpp
 * \author Simon Robert
 * \date 23-04-2012
 * \brief The file containing the Tobii module template
 */


#include "devices/tobii/Tobii.h"

using namespace std;
using namespace TobiiTracker;



CTobii::CTobii(char* model):m_oMainloop(), m_oBrowser(m_oMainloop)
{
	m_sProdId = model;
	m_lLastUpdate = -1;
}
	
bool CTobii::init()
{
	m_pEyetracker = findEyetracker();
	m_pEyetracker->add_gaze_data_received_listener(boost::bind(&CTobii::receiveGaze, this, _1));
	m_pEyetracker->start_tracking();

	return true;
}
	

ts::tracking::gaze_data_item::pointer CTobii::grabGaze()
{
	if(m_lLastUpdate < 0)
	{
		return ts::tracking::gaze_data_item::pointer();
	}
	return m_pGazeDataItem;
}

void CTobii::initMainloop()
{
	m_pMainloopThread = new boost::thread(boost::bind(&CTobii::runMainloop, this));
}

void CTobii::receiveGaze(ts::tracking::gaze_data_item::pointer gd)
{
	m_pGazeDataItem = gd;
	m_lLastUpdate = gd->time_stamp;
}


void CTobii::runMainloop()
{
	m_oMainloop.run();
}

ts::tracking::eyetracker::pointer CTobii::findEyetracker()
{
		m_oBrowser.add_browser_event_listener(boost::bind(&CTobii::getInfos, this, _1, _2));
		m_oBrowser.start();

		initMainloop();

		//waits one second
		boost::this_thread::sleep(boost::posix_time::seconds(1));
		m_oBrowser.stop();

		if(m_pEyetrackerInfo == NULL)
		{
			return ts::tracking::eyetracker::pointer();
		}
		else
		{
			return ts::tracking::create_eyetracker(ts::discovery::factory_info(*m_pEyetrackerInfo), m_oMainloop);
		}
}

void CTobii::getInfos(ts::discovery::eyetracker_browser::event_type type, ts::discovery::eyetracker_info::pointer info)
{
	if(info->get_product_id() == m_sProdId)
	{
		m_pEyetrackerInfo = info;
	}
}

bool CTobii::close()
{
	if(m_pEyetracker != NULL)
	{
		m_pEyetracker->stop_tracking();
	}

	m_oMainloop.quit();

	if(m_pMainloopThread != NULL)
	{
		m_pMainloopThread->join();
	}

	return true;
}

uint64_t CTobii::getLastUpdate()
{
	return m_lLastUpdate;
}

