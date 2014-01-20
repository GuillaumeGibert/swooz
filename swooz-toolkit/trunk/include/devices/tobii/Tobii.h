/**
 * \file Tobii.h
 * \author Simon Robert
 * \date 23-04-2012
 * \brief The file containing the Tobii module template
 *
 * This file contains the template of a Tobii module, connecting to a
 * specific eye-tracker and receiving gaze information as well as pupil
 * size. It eases the obtention of data by offering a convenient
 * interface for opening and closing connections to eye-trackers.
 */


#ifndef _TOBII_
#define _TOBII_

#include "stdafx.h"
#include <iostream>
#include "boost/thread.hpp"

#include "tobii/sdk/cpp/init.hpp"
#include <tobii/sdk/cpp/discovery/eyetracker-info.hpp>
#include <tobii/sdk/cpp/discovery/factory-info.hpp>
#include <tobii/sdk/cpp/tracking/eyetracker.hpp>
#include <tobii/sdk/cpp/mainloop.hpp>
#include <tobii/sdk/cpp/tracking/eyetracker-factory.hpp>
#include <tobii/sdk/cpp/discovery/eyetracker-browser.hpp>

namespace ts = tobii::sdk::cpp;

namespace TobiiTracker
{

    /**
     * \class CTobii
     * \author Simon Robert
     * \date 23-04-2012
     * \brief The CTobii class retrieves gaze data from an eye-tracker
     *
     * This class connects to a specified Tobii eye-tracker model, and starts
     * listening for updates in a separate thread. The contained value is
     * constantly updated and thus available for retrieval in simulated real-
     * time.
     */
    class CTobii
    {
        private:

            char* m_sProdId; /**< Eye-tracker product ID  */
            uint64_t m_lLastUpdate; /**<  Latest update's timestamp */

            boost::thread* m_pMainloopThread; /**<  Side-running event thread */

            ts::mainloop m_oMainloop; /**<  Main loop */

            ts::discovery::eyetracker_info::pointer m_pEyetrackerInfo; /**<  Eye-tracker information */
            ts::discovery::eyetracker_browser m_oBrowser; /**<  Eye-tracker browser */

            ts::tracking::eyetracker::pointer m_pEyetracker; /**<  Eye-tracker connection */
            ts::tracking::gaze_data_item::pointer m_pGazeDataItem; /**< Latest gaze data  */

        public:
            /**
             * \brief The CTobii constructor creates the connector with a given product
             *  ID
             *
             * This constructor creates the CTobii module which will connect to the
             * specified eye-tracker for data retrieval.
             * \param model[in] : Tobii product ID
             */
            CTobii(char* model);

            /**
             * \brief The init function connects to the eye-tracker and starts the
             *  listening for data
             *
             * This function seeks connected eye-trackers for the specified model and,
             * if found, connects to it and begins data retrieval.
             * \return whether initialization was successful
             */
            bool init();

            /**
             * \brief The grabGaze function returns the recent gaze data
             *
             * This function returns, if present, the most recently obtained valid
             * gaze data.
             * \return the last obtained valid gaze data
             */
            ts::tracking::gaze_data_item::pointer grabGaze();

            /**
             * \brief The close function terminates the connection and listening
             *
             * This function terminates the listening mechanism and related processes
             * to stop the eye-tracker connection.
             * \return whether closure was successful
             */
            bool close();

            /**
             * \brief The getInfos function is an event listener for eye-tracker
             *  connections and disconnections
             *
             * This function listens for eye-tracker connections and disconnections
             * from the eye-tracker browser and determines if received information
             * corresponds to sought Tobii model.
             * \param event_type[in] : Event type
             * \param info[in] : Tobii eye-tracker information
             */
            void getInfos(ts::discovery::eyetracker_browser::event_type type, ts::discovery::eyetracker_info::pointer info);

            /**
             * \brief The receiveGaze function is an event listener for gaze data
             *  detections by the active connection.
             *
             * This function listens for gaze data sent from the eye-tracker connection.
             * \param gazeData[in] : Tobii eye-tracker information
             */
            void receiveGaze(ts::tracking::gaze_data_item::pointer gd);

            /**
             * \brief The getLastUpdate function returns the latest gaze data update's
             *  timestamp.
             *
             * This function returns the time in milliseconds representing the moment
             * when the latest gaze data update was received.
             * \return The latest update's timestamp
             */
            uint64_t getLastUpdate();

        private:

            /**
             * Runs the main loop.
             */
            void runMainloop();

            /**
             * \brief Listens for eye-tracker active connections to detect the requested
             * eye-tracker.
             */
            ts::tracking::eyetracker::pointer findEyetracker();

            /**
             * \brief Starts the main loop in a separate thread.
             */
            void initMainloop();
    };
}
#endif
