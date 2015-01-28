
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
 * \file FaceLab.h
 * \author Simon Robert
 * \date 03-05-2012
 * \brief The file containing the FaceLAB module template
 *
 * This file contains the template of a FaceLAB module, connecting to a
 * specific tracker and receiving head and gaze information as well as pupil
 * size. It eases the obtention of data by offering a convenient
 * interface for opening and closing connections to the server.
 */

#ifndef _FACE_LAB_
#define _FACE_LAB_

#include <eod/sdk.h>
#include <boost/thread.hpp>
#include <iostream>

#include "HeadGazeData.h"


namespace FaceLabTracker
{
    /**
     * \class CFaceLab
     * \author Simon Robert
     * \date 03-05-2012
     * \brief The CFaceLab class retrieves gaze data from an eye-tracker
     *
     * This class connects to a specified FaceLAB server, and starts
     * listening for updates in a separate thread. The contained value is
     * constantly updated and thus available for retrieval in simulated real-
     * time.
     */
    class CFaceLab
    {
        private:

            bool m_bListening;                                  /**< Whether the module is listening or not */

            int m_iPortNumber;                                  /**< UDP port number for listening */

            uint64 m_lLastUpdate;                               /**< Latest update's frame */

            sm::eod::EngineOutputData* m_pHeadGazeEngineData;   /**< Latest data */

            boost::thread* m_pListeningThread;                  /**< Side-running event thread */

        public:

            /**
             * \brief The CFaceLab constructor creates the connector with a given port
             * number
             *
             * This constructor creates the CFaceLab module which will connect to the
             * specified eye-tracker for data retrieval.
             * \param port[in] : UDP listening port
             */
            CFaceLab(int port);

            /**
             * \brief The handleEngineOutputData function handles received data
             * from FaceLAB server
             *
             * This function stores the received data from the FaceLAB server and
             * updates the latest frame number.
             * \param gazeData[in] : Engine output data retrieved
             * \param from[in] : Source IP address
             */
            void handleEngineOutputData(sm::eod::EngineOutputData&
                gazeData, const sm::eod::io::InetAddress& from);

            /**
             * \brief The handleDatagram function receives data from a socket sent
             * by the FaceLAB server
             *
             * This function receives and handles a socket sent by the FaceLAB
             * server and stores its data.
             * \param inputSocket[in] : Connection socket
             */
            void handleDatagram(sm::eod::io::DatagramSocket& input_socket);

            /**
             * \brief The init function begins the UDP listening for data
             *
             * This function starts listening for data sockets sent on the
             * specified port.
             * \return whether initialization was successful
             */
            bool init();

            /**
             * \brief The close function terminates the connection and listening
             *
             * This function terminates the listening mechanism and related processes
             * to stop the eye-tracker connection.
             * \return whether closure was successful
             */
            bool close();

            /**
             * \brief The grabData function returns the recent gaze data
             *
             * This function returns, if present, the most recently obtained
             * gaze data.
             * \return the last obtained gaze data
             */
            HeadGazeData grabData();

            /**
             * \brief The init function begins the UDP listening for data
             *
             * This function starts listening for data sockets sent on the
             * specified port.
             * \return whether initialization was successful
             */
            void listen();

            /**
             * \brief The getLastUpdate function returns the latest gaze data update's
             *  frame.
             *
             * This function returns the frame number representing the moment
             * when the latest gaze data update was received.
             * \return The latest update's frame
             */
            uint64 getLastUpdate();

        private:

            /**
             * \brief Convert face coordiante to 3D point
             */
            void CFaceLab::convFaceCoordinateToPoint3D(const sm::eod::FaceCoordinate &fc, Point3D &p);

            /**
             * \brief Starts a new thread listening for data sockets
             * sent on the specified port.
             */
            void startListening();

            /**
             * \brief Creates a structure from conventional output data.
             */
            HeadGazeData toStructure(sm::eod::EngineOutputData gazeData);
    };
}

#endif
