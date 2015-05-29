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
 * \file SWOculus_thread.h
 * \brief Defines SWOculus_thread
 * \author Guillaume Gibert
 * \date 28/05/15
 */

#ifndef _SWOCULUS_THREAD_
#define _SWOCULUS_THREAD_

#include "devices/oculus/SWOculus.h"
#include "devices/SWDevice_thread.h"

#include <vector>

namespace swDevice
{
	/**
	 * \class SWOculus_thread
	 * \brief A threaded Oculus module.
	 */	
	class SWOculus_thread : public SWDevice_thread
	{
		public:
			
			/**
			 * \brief Default SWOculus_thread constructor.
			 */	
			SWOculus_thread();
		
			/**
			 * \brief SWOculus_thread destructor.
			 */		
			virtual ~SWOculus_thread(void);
		
			/**
			 * \brief Initialization of the Polhemus Oculus device
			 * \return true if device detected else return false
			 */		
			virtual  bool init();

			/**
			 * \brief Start listening Oculus data.
			 * \throw deviceError
			 */	
			virtual  void startListening();
		
			/**
			 * \brief Stop listening Oculus data.
			 */
			virtual  void stopListening();
		
		
			/**
			 * \brief Indicates if Oculus data is available.
			 * \return true if data available, else return false.
			 */		
			bool isDataAvailable();
			
			
			/**
			* \brief Accessor for the rotation vector
			* \return vector of rotation angles
			 */					
			std::vector<float> rotations();

			/**
			* \brief Accessor for the translation vector
			* \return vector of translation angles
			 */		
			std::vector<float> translations();
			
		
		private :

			/**
			 * \brief Work thread.
			 */		
			void doWork();
		
			bool m_bInitialized;                    /**< is the module initialized ? */
			bool m_bDataAvailable;                  /**< is the data available ? */

			std::vector<float> m_v3FRotations;      /**< Oculus rigid motion rotations  */
			std::vector<float> m_v3FTranslations; 	/**< Oculus rigid motion translations */
		
			SWOculus m_oOculus;                   /**< Oculus module */
	};
};

#endif
