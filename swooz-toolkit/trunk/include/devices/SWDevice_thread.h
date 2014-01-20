
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