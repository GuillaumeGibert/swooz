
/**
 * \file SWFastrak_thread.h
 * \brief Defines SWFastrak_thread
 * \author Florian Lance
 * \date 20/06/13
 */

#ifndef _SWFASTRAK_THREAD_
#define _SWFASTRAK_THREAD_

#include "devices/fastrak/SWFastrak.h"
#include "devices/SWDevice_thread.h"

#include <vector>

namespace swDevice
{
	/**
	 * \class SWFastrak_thread
	 * \brief A threaded fastrak module.
	 */	
	class SWFastrak_thread : public SWDevice_thread
	{
		public:
			
			/**
			 * \brief Default SWFastrak_thread constructor.
			 */	
			SWFastrak_thread();
		
			/**
			 * \brief SWFastrak_thread destructor.
			 */		
			virtual ~SWFastrak_thread(void);
		
			/**
			 * \brief Initialization of the Polhemus fastrak device
			 * \return true if device detected else return false
			 */		
			virtual  bool init();

			/**
			 * \brief Start listening fastrak data.
			 * \throw deviceError
			 */	
			virtual  void startListening();
		
			/**
			 * \brief Stop listening fastrak data.
			 */
			virtual  void stopListening();
		
		
			/**
			 * \brief Indicates if fastrak data is available.
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
		
            std::vector<float> m_v3FRotations;      /**< fastrak rigid motion rotations  */
			std::vector<float> m_v3FTranslations; 	/**< fastrak rigid motion translations */
		
            SWFastrak m_oFastrak;                   /**< fastrak module */
	};
};

#endif
