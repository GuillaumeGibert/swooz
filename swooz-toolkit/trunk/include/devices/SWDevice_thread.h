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
 * \file SWDevice_thread.h
 * \brief Defines SWDevice_thread
 * \author Florian Lance
 * \date 20/06/13
 */

#ifndef _SWDEVICE_THREAD_
#define _SWDEVICE_THREAD_


#include "boost/thread.hpp"

namespace swDevice
{
	/**
	 * \class SWDevice_thread
	 * \brief ...
	 */	
	class SWDevice_thread
	{
		protected :
			
			SWDevice_thread() : m_bListening(false) {}
		 
			/**
			 * \brief Start listening data.
			 * \throw deviceError
			 */					
			virtual void startListening() = 0;
					
			/**
			 * \brief Work thread.
			 */					
			virtual void doWork() = 0;
				
			/**
			 * \brief Stop listening data.
			 */
			virtual void stopListening() = 0;				
		
				
			bool m_bListening;	/**< is the listening thread launched ? */
		
			boost::shared_ptr<boost::thread> m_pListeningThread;	/**< listening thread */
			boost::mutex m_oMutex;					/**< mutex */				
	};
}
	
	
#endif
