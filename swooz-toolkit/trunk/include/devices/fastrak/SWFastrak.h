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

