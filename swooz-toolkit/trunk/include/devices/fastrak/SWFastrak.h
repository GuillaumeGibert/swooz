
/**
 * \file SWFastrak.h
 * \brief Defines SWFastrak
 * \author Florian Lance
 * \date 12/06/13
 */

#ifndef _SWFASTRAK_
#define _SWFASTRAK_

#include "commonTypes.h"

#include "Windows.h"
#include <tchar.h>

#include "PDI.h"

//! namespace for devices interfaces
namespace swDevice
{
	/**
	 * \class SWFastrak
	 * \brief A basic interface class for getting rigid motion data from the Polhemus Fastrak device in real time.
	 */	
    class SWFastrak
	{
		public:
			
			/**
			 * \brief Default SWFastrak constructor
			 */	
			SWFastrak();
		
			/**
			 * \brief SWFastrak destructor
			 */		
			~SWFastrak();
		
			/**
			 * \brief Initialization of the Polhemus fastrak device
			 * \return true if device detected else return false
			 */		
			virtual bool init();
		
			/**
			 * \brief Read fastrak data
			 * \return true if data available, if the fastrak is disconnected return false
			 */	
			virtual bool read();
			
			/**
			 * \brief Disconnect the fastrak
			 */			
			virtual void stop();
		
		private :
			
			CPDIfastrak  m_oFastrak; 	/**< fastrak polhemus device */
		
		public :
		
			float m_aFRotations[3];		/**< rigid motion rotation array */
			float m_aFTranslations[3];	/**< rigid motion translation array */

            bool m_bDisconnected; /**< indicates if the device is disconnected */
	};
};

#endif

