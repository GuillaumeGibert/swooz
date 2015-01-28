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
 * \file SWKinectRFModule.h
 * \brief Defines SWKinectRFModule
 * \author  Florian Lance
 * \date 16/12/14
 */

#ifndef SWKINECTRFMODULE_H
#define SWKINECTRFMODULE_H


#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>


#include "devices/rgbd/SWKinect_thread.h"


/**
 * \class SWKinectRFModule
 * \author Florian Lance
 * \date 16-12-2014
 * \brief send kinect rgb/depth data to yarp
 */
class SWKinectRFModule : public yarp::os::RFModule
{
    public:

        /**
         * \brief Init configuration values with config files, herited from RFModule
         * \param [in] oRf : icub resource config file
         * \return true if configuration successful
         */
        bool configure();

        /**
         * \brief Called periodically every getPeriod() seconds
         * \return true
         */
        bool updateModule();

        /**
         * \brief Retrieve the update function call period of the module.
         * \return the period
         */
        double getPeriod();

        /**
         * \brief Interrupt the module.
         * \return true to confirm that the module is ready to be closed
         */
        bool interruptModule();

        /**
         * \brief Close connections.
         * \return true (Module heritage)
         */
        bool close();

    private:

        yarp::sig::ImageOf<yarp::sig::PixelRgb>    m_rgb;

        swDevice::SWKinect_thread m_kinectThread;

        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > m_rgbPort;
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgbInt> > m_depthPort; // s

        int m_fps;   /**< refresh rate of updateModule calling */
};

#endif





