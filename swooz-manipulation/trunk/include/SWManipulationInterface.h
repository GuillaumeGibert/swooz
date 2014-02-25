/**
 * \file SWManipulationInterface.h
 * \brief Defines SWManipulationInterface class
 *
 *  ...
 *
 * \author Florian Lance
 * \date 20/02/14
 */


#ifndef SWMANIPULATIONINTERFACE_H
#define SWMANIPULATIONINTERFACE_H

// SWOOZ
#include "SWExceptions.h"
#include "commonTypes.h"

// INTERFACES
#include "SWUI_Manipulation.h"


// YARP
//#include <yarp/dev/all.h>
#include <yarp/os/all.h>
//#include <yarp/sig/all.h>
//#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
//#include <yarp/os/ResourceFinder.h>

// QT
#include <QThread>
#include <QReadWriteLock>
#include <QDebug>
#include <QTime>
#include <QVector>
#include <QBasicTimer>

//using namespace yarp::os;
//using namespace yarp::dev;
//using namespace yarp::sig;


enum PlanificationMode
{
    NORMAL,MODIFIED,RANDOM,SEQUENCE
};

struct SWBottleContent
{
    int idLib;
    std::vector<double> dValues;

    void display() const
    {
        std::cout << idLib << " ";
        for(uint ii = 0; ii < dValues.size(); ++ii)
        {
            std::cout << dValues[ii] << " ";
        }
        std::cout << std::endl;
    }
};

/**
 * \class SWManipulationWorker
 * \brief  Worker used in the swooz manipulation interface
 * \author Florian Lance
 * \date 20/02/14
 */
class SWManipulationWorker : public QObject
{
    Q_OBJECT

    public :

        /**
         * @brief SWManipulationWorker
         */
        SWManipulationWorker();

        /**
         * @brief ~SWManipulationWorker
         */
        ~SWManipulationWorker();

        /**
         * @brief isInitialized
         * @return
         */
        bool isInitialized() const;

    private :

        /**
         * @brief retrieveBottleContent
         * @param oBottleContent
         * @param oPort
         * @return
         */
        bool retrieveBottleContent(SWBottleContent &oBottleContent, yarp::os::BufferedPort<yarp::os::Bottle> &oPort);

        /**
         * @brief applyModifierOnBottle
         * @param oBottleContent
         * @param vDShifts
         * @param vDConst
         */
        void applyModifierOnBottle(SWBottleContent &oBottleContent, QVector<double> vDShifts, QVector<double> vDConst);


    public slots :

        /**
         * @brief startLoop
         */
        void startLoop();

        /**
         * @brief stopLoop
         */
        void stopLoop();

        /**
         * @brief updateModifier
         * @param vShifts
         * @param vConsts
         * @param i32Index
         */
        void updateModifier(QVector<double> vShifts, QVector<double> vConsts, int i32Index);

        /**
         * @brief updatePlanification
         * @param i32Index
         * @param i32Mode
         * @param dTimeTotal
         * @param dTimeBlock
         * @param sSequence
         */
        void updatePlanification(int i32Index, int i32Mode, double dTimeTotal, double dTimeBlock, QString sSequence);

        /**
         * @brief SWManipulationWorker::toggleOutPort
         * @param i32IndexPort
         */
        void toggleOutPort(int i32IndexPort);

        /**
         * @brief updateBottleStart
         * @param i32IndexPort
         * @param bStart
         */
        void updateBottleStart(int i32IndexPort, bool bStart);


    signals :

        /**
         * @brief sendBottle
         */
        void sendBottle(SWBottleContent, int);


    private :

        bool m_bDoLoop;                                 /**< ... ? */
        bool m_bInitialization;                         /**< ... */

        int m_i32LoopPeriod;                            /**< ... */

        QReadWriteLock m_oLoopMutex;                    /**< mutex */
        QReadWriteLock m_oActiveBottleMutex;            /**< mutex */
        QReadWriteLock m_oStartBottleMutex;             /**< mutex */
        QReadWriteLock m_oModifiersMutex;               /**< mutex */

        QVector<bool> m_vBActiveBottlesOUTSend;         /**< ... */
        QVector<bool> m_vBStartBottlesOutSend;          /**< ... */
        QVector<double> m_vTimePlanification;           /**< ... */

        QVector<QVector<double> > m_vVShifts;           /**< ... */
        QVector<QVector<double> > m_vVConstants;        /**< ... */
        QVector<int> m_vPlanMode;                       /**< ... */
        QVector<double> m_vPlanTimeTotal;               /**< ... */
        QVector<double> m_vPlanTimeBlock;               /**< ... */
        QVector<QString> m_vPlanSequence;               /**< ... */

        std::vector<SWBottleContent> m_vBottlesContent; /**< ... */

        std::vector<std::string> m_vSManipulationINPortName;                        /**< vector of yarp IN port names */
        std::vector<std::string> m_vSManipulationOUTPortName;                       /**< vector of yarp OUT port names */
        QVector<yarp::os::BufferedPort<yarp::os::Bottle>*> m_vManipulationINPort;   /**< ... */
        QVector<yarp::os::BufferedPort<yarp::os::Bottle>*> m_vManipulationOUTPort;  /**< ... */
};


namespace Ui {
    class SWUI_Manipulation;
}

/**
 * \class SWManipulationInterface
 * \brief  Interface for using manipuling/modifiying yarp bottles form the tracking modules
 * \author Florian Lance
 * \date 20/02/14
 */
class SWManipulationInterface : public QMainWindow
{
    Q_OBJECT

    public :

        /**
         * \brief constructor of SWManipulationInterface
         */
        SWManipulationInterface();

        /**
         * \brief destructor of SWManipulationInterface
         */
        ~SWManipulationInterface();

        /**
         * @brief closeEvent
         * @param [in] event : close event
         */
        void closeEvent(QCloseEvent *event);

        /**
         * \brief Timer event
         * \param [in] e : qt timer event
         */
        void timerEvent(QTimerEvent *e);


    public slots :

        /**
         * @brief updateBottleContentDisplay
         */
        void updateBottleContentDisplay(SWBottleContent, int);

        /**
         * @brief switchModifiersValues
         * @param i32Index
         */
        void switchModifiersValues(int i32Index);

        /**
         * @brief resetModifiers
         */
        void resetModifiers();

        /**
         * @brief resetAllModifiers
         */
        void resetAllModifiers();

        /**
         * @brief checkActiveClick
         */
        void checkActiveClick();

        /**
         * @brief toggleActiveText
         * @param i32Index
         */
        void toggleActiveText(int i32Index);


        /**
         * @brief startBottlePlanification
         */
        void startBottlePlanification();

        /**
         * @brief startAllBottlesPlanification
         */
        void startAllBottlesPlanification();


    signals:

        /**
         * \brief Signal for calling the clean function in the worker
         */
        void cleanModule();

        /**
         * @brief startLoop
         */
        void startLoop();

        /**
         * @brief Signal for stopping the work in the worker
         */
        void stopLoop();

        /**
         * @brief sendBottleModifiers
         */
        void sendBottleModifiers(QVector<double>, QVector<double>, int);

        /**
         * @brief sendPlanificationParams
         */
        void sendPlanificationParams(int, int, double, double, QString);

        /**
         * @brief activeOutput
         */
        void activeOutput(int);

        /**
         * @brief startBottlePlan
         */
        void startBottlePlan(int, bool);


    private :

        // ui
        Ui::SWUI_Manipulation *m_uiManipulation;    /**< qt main window manipulation interface */

        // worker / thread
        SWManipulationWorker *m_pWManipulation;     /**< manipulation worker pointer */
        QThread               m_TManipulation;      /**< manipulation worker thread */

        // timers
        double m_bTimeLoop;                         /**< time between two time event call */
        QBasicTimer m_oTimer;                       /**< timer used for refreshing paramters data */

        // timeout
        QVector<double> m_vDTimeOutINBottles;       /**< timeout in bottles container */


        int m_i32YarpConnectInputNumber;            /**< ... */
        int m_i32ShiftConstNumber;                  /**< ... */

        // parameters container corrsponding to a connection row
        QVector<QVector<double> > m_vVShifts;       /**< container of each connection input shifts parameters */
        QVector<QVector<double> > m_vVConstants;    /**< container of each connection input constants parameters */
        QVector<double> m_vDTotalTime;              /**< container of each connection input total time */
        QVector<double> m_vDBlockTime;              /**< container of each connection input block time */
        QVector<int> m_vI32ModePlan;                /**< container of each connection input planification mode */
        QVector<QString> m_vSSequence;              /**< container of each connection input sequence */

        // widgets containers
        QVector<QLineEdit*>      m_vBottleDisplayLineEdit;      /**< bottle display line edit container */
        QVector<QLineEdit*>      m_vShiftDisplayLineEdit;       /**< shift display line edit container */
        QVector<QPushButton*>    m_vActiveButtons;              /**< active buttons container */
        QVector<QLabel*>         m_vSourceLabels;               /**< source labels container */
        QVector<QSlider*>        m_vShiftSliders;               /**< shift sliders container */
        QVector<QDoubleSpinBox*> m_vConstDSpinBoxes;            /**< const double spinbox container */
        QVector<QRadioButton*>   m_vPlanificationRadioButtons;  /**< planification radio buttons container */
};


#endif
