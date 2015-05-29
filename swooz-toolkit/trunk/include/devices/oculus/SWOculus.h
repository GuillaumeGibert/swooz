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
 * \file SWOculus.h
 * \brief Defines SWOculus
 * \author Guillaume Gibert
 * \date  28/05/15
 */

#ifndef _SWOCULUS_
#define _SWOCULUS_

#include "commonTypes.h"

#include "Windows.h"
#include <tchar.h>

#include "OVR_CAPI.h"
#include "Extras/OVR_Math.h"

#ifndef PI
	#define PI 3.14159265f
#endif

#ifndef RADIANS_TO_DEGREES
	#define RADIANS_TO_DEGREES (180.0f / PI)
#endif


//! namespace for devices interfaces
namespace swDevice
{
	/**
	 * \class SWOculus
	 * \brief A basic interface class for getting rigid motion data from the Polhemus Oculus device in real time.
	 */	
	class SWOculus
	{
		public:
			
			/**
			 * \brief Default SWOculus constructor
			 */	
			SWOculus();
		
			/**
			 * \brief SWOculus destructor
			 */		
			~SWOculus();
		
			/**
			 * \brief Initialization of the Polhemus Oculus device
			 * \return true if device detected else return false
			 */		
			virtual bool init();
		
			/**
			 * \brief Read Oculus data
			 * \return true if data available, if the Oculus is disconnected return false
			 */	
			virtual bool read();
			
			/**
			 * \brief Disconnect the Oculus
			 */			
			virtual void stop();
		
		private :
			
			ovrHmd m_ohmd; 	/**< Oculus polhemus device */
		
		public :
		
			float m_aFRotations[3];		/**< rigid motion rotation array */
			float m_aFTranslations[3];	/**< rigid motion translation array */

			bool m_bDisconnected; /**< indicates if the device is disconnected */
	};
};

#endif

