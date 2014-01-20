
/**
 * \file SWFaceLab.h
 * \brief Defines SWFaceLab
 * \author Florian Lance
 * \date 13/06/13
 */

#ifndef _SWFACELAB_
#define _SWFACELAB_


#include "commonTypes.h"
#include "SWExceptions.h"

#include "devices/faceLab/FaceLab.h"

namespace swDevice
{
    /**
     * \class SWFaceLab
     * \brief A basic interface class for getting data from the FaceLab device in real time.
     *
     * It's an user-friendly hight level class based on FaceLab.h, data is grabed on a dedicated thread.
     * for more informations (like how the threads are managed) see this one.
     */
    class SWFaceLab
    {
        public:

            /**
             * \brief Default SWFaceLab constructor
             */
            SWFaceLab();

            /**
             * \brief SWFaceLab destructor
             */
            ~SWFaceLab();

            /**
             * \brief Initialization of the FaceLab device
             * \return true if device detected else return false
             * \throw a swFaceLabError if no device detected
             */
            virtual bool init();

            /**
             * \brief Grab FaceLab data
             * \return false is the device is disconnected, else return true
             */
            virtual bool read();

            /**
             * \brief Close the faceLab module
             */
            virtual void stop();

        private :

            FaceLabTracker::CFaceLab* m_pFaceLabModule; /**< facelab device */

        public :

            bool m_bDisconnected;                       /**< indicates if the device is disconnected */
            HeadGazeData m_oHeadGazeData;               /**< faceLab data */

    };
};

#endif

