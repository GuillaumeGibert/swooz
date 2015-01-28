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

