
/**
 * \file SWEmicpHeadTracking.h
 * \brief Defines SWEmicpHeadTracking class
 *
 *  ...
 *
 * \author Florian Lance
 * \date 09/12/13
 */

#ifndef _SWEMICPHEADTRACKING_
#define _SWEMICPHEADTRACKING_

// SWOOZ
#include "SWExceptions.h"

// INTERFACES
#include "interface/QtWidgets/SWGLCloudWidget.h"
#include "interface/QtWidgets/SWDisplayImageWidget.h"
#include "interface/QtWidgets/SWDisplayHistogramWidget.h"
#include "../genUI/SWUI_WEmicpHeadTracking.h"


// RGBD
#include "devices/rgbd/SWKinect_thread.h"

// EMICP
#include "cloud/SWCaptureHeadMotion.h"

// YARP
#include <yarp/dev/all.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>



namespace Ui {
    class SWUI_WEmicpHeadTracking;
}


/**
 * \class SWEmicpHeadTrackingWorker
 * \brief  Worker used in the emicp head tracking interface
 * \author Florian Lance
 * \date 10/12/13
 */
class SWEmicpHeadTrackingWorker : public QObject
{
    Q_OBJECT

    public :

        /**
         * \brief constructor of SWEmicpHeadTrackingWorker
         */
        SWEmicpHeadTrackingWorker();

        /**
         * \brief destructor of SWEmicpHeadTrackingWorker
         */
        ~SWEmicpHeadTrackingWorker();           

        /**
         * \brief Update parameters from the interface
         * \param [in] dTemplateCoeffReduc  : template cloud coefficient reduction
         * \param [in] dTargetCoeffReduc    : target cloud coefficient reduction
         * \param [in] dScoreComputingReduc : score alignment score cloud reduction
         * \param [in] i32K                 : number of previous computed rigidMotion used for temporal filtering on the current result (if == 0, the result is not smoothed)
         * \param [in] dKTransSmooth        : emipiric value used to compute the translation smoothing for the rigid motion
         * \param [in] dKRotSmooth          : emipiric value used to compute the rotation smoothing for the rigid motion
         * \param [in] dP2                  : emicp : P2
         * \param [in] dINF                 : emicp : INF
         * \param [in] dFactor              : emicp : Factor
         * \param [in] dD02                 : emicp : D02
         */
        void updateInterfaceParameters(cdouble dTemplateCoeffReduc, cdouble dTargetCoeffReduc, cdouble dScoreComputingReduc,
                                       cint i32K, cdouble dKTransSmooth, cdouble dKRotSmooth,
                                       cdouble dP2, cdouble dINF, cdouble dFactor, cdouble dD02);

    public slots:

        /**
         * \brief Start working thread
         */
        void doWork();

        /**
         * \brief Stop working thread
         */
        void stopWork();

        /**
         * \brief clean the worker
         */
        void clean();

        /**
         * \brief Configure form yarp resource file
         * \param [in] pRF : config file
         */
        void configure(yarp::os::ResourceFinder &pRF); 


    signals:

        /**
         * \brief Signal for calling the end of the program
         */
        void leaveProgram();

        /**
         * \brief Signal for sending the current cloud to display
         */
        void sendCloudToDisplay(swCloud::SWCloud*);

        /**
         * @brief Signal for sending the face rectangle
         */
        void sendFaceRect(cv::Rect*);

        /**
         * @brief Signal for sending the nose rectangle
         */
        void sendNoseRect(cv::Rect*);

        /**
         * @brief Signal for sending the rigid motion
         */
        void sendRigidMotion(swCloud::SWRigidMotion*);

        /**
         * @brief Signal for sending the delay
         */
        void sendDelay(float);


    private :

        bool m_bIsRGBDDeviceInitialized;/**< is rgbd device initialized ? */
        bool m_bVerbose;                /**< verbose info display ? */
        bool m_bDoWork;                 /**< do the work ? */
        bool m_bWorkStopped;            /**< is the work stopped ? */

        int m_i32Fps;                   /**< refresh rate of updateModule calling */

        std::string m_sHeadTrackingPortName;    /**< yarp head tracking port name */

        swDevice::SWKinectParams m_CKinectParams;      /**< kinect video params */

        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackingPort;   /**< yarp head tracking port */

        QReadWriteLock                  m_oLoopMutex;           /**< loop mutex */
        QReadWriteLock                  m_oParametersMutex;     /**< parameters mutex */

        cv::Rect                       *m_pCurrentFaceRect;     /**< current face rectangle */
        cv::Rect                       *m_pCurrentNoseRect;     /**< current nose rectangle */
        swCloud::SWRigidMotion         *m_pCurrentRigidMotion;  /**< current rigid motion */
        swCloud::SWCloud               *m_pReferenceCloud;      /**< reference cloud for alignment */
        swCloud::SWCloud               *m_pCurrCloud;           /**< current cloud */


        swCloud::SWRigidMotion          m_oCurrentRigidMotion;  /**< current rigid motion */
        swDevice::SWKinect_thread       m_oKinectThread;        /**< rgbd device */
        swCloud::SWCaptureHeadMotion    m_oCaptureHeadMotion;   /**< capture head motion module */
};



/**
 * \class SWEmicpHeadTrackingInterface
 * \brief  Interface for using the emicp head tracking
 * \author Florian Lance
 * \date 10/12/13
 *
 * Bottle content :
 *
 * ...
 */
class SWEmicpHeadTrackingInterface : public QMainWindow
{
    Q_OBJECT

    public :

        /**
         * \brief constructor of SWEmicpHeadTrackingInterface
         */
        SWEmicpHeadTrackingInterface();

        /**
         * \brief destructor of SWEmicpHeadTrackingInterface
         */
        ~SWEmicpHeadTrackingInterface();

        /**
         * @brief closeEvent
         * @param [in] event : close event
         */
        void closeEvent(QCloseEvent *event);

        /**
         * \brief Init configuration values with the config file
         * \param [in] oRf : icub resource config file
         */
        void configure(yarp::os::ResourceFinder &oRf);


        /**
         * \brief Timer event
         * \param [in] e : qt timer event
         */
        void timerEvent(QTimerEvent *e);

        /**
         * @brief Update the image display widget
         */
        void updateImageDisplay();


    public slots :

        /**
         * @brief Update the cloud display widget
         * @param [in] pCloud : cloud to display (the pointer is not deleted)
         */
        void updateCloudDisplay(swCloud::SWCloud *pCloud);

        /**
         * @brief Update the face rectangle to be displayed
         * @param [in] pFaceRect : face rectangle pointer
         */
        void updateFaceRectangle(cv::Rect *pFaceRect);

        /**
         * @brief Update the nose rectangle to be displayed
         * @param [in] pNoseRect : nose rectangle pointer
         */
        void updateNoseRectangle(cv::Rect *pNoseRect);

        /**
         * @brief Update the histograms with the current rigid motion values
         * @param [in] pRigidMotion : computed rigid motion pointer
         */
        void updateHistogramDisplay(swCloud::SWRigidMotion *pRigidMotion);

        /**
         * @brief Update all the values from the Qt interface
         * @param [in] i32Value : non-used value for the correspondence with the signal
         */
        void updateInterfaceValues(int i32Value);

        /**
         * @brief Update all the values from the Qt interface
         * @param [in] dValue : non-used value for the correspondence with the signal
         */
        void updateInterfaceValues(double dValue);

        /**
         * @brief updateDelay
         * @param fDelay
         */
        void updateDelay(float fDelay);

    signals:

        /**
         * \brief Signal for calling the clean function in the worker
         */
        void cleanModule();

        /**
         * @brief Signal for stopping the work in the worker
         */
        void stopModule();

    private :


        Ui::SWUI_WEmicpHeadTracking *m_uiMainWindow;            /**< qt main window */
        SWGLCloudWidget             *m_pGLCloudWidget;          /**< opengl cloud widget pointer*/
        SWDisplayImageWidget        *m_pDisplayImageWidget;     /**< image display widget pointer */
        SWDisplayHistogramWidget    *m_pDisplayHistoRotWidget;  /**< rotations histogram display widget pointer */
        SWDisplayHistogramWidget    *m_pDisplayHistoTransWidget;/**< translations histogram display widget pointer */

        QReadWriteLock              m_oMutex;                   /**< mutex used for updating data from the worker */
        QBasicTimer *m_oTimer;                                  /**< qt timer */

        float                       m_fDelay;                   /**< delay between two rigid motion computed */
        cv::Rect                    m_oFaceRect;                /**< face zone rectangle to be displayed */
        cv::Rect                    m_oNoseRect;                /**< nose zone rectangle to be displayed */

        swDevice::SWKinectParams    m_CKinectParams;            /**< kinect video params */
        swDevice::SWKinect_thread   m_oKinectThread;            /**< rgbd device */

        SWEmicpHeadTrackingWorker  *m_pWTracking;               /**< tracking worker */
        QThread                     m_TTracking;                /**< tracking thread */
};


#endif
