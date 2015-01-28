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

