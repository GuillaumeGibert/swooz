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

enum Modifier
{
    NO_MODIF, DAMPING, SHIFT, DAMPING_AND_SHIFT, DEFINED_BY_SEQUENCE
};

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
         * @brief applyDampingOnBottle
         * @param oBottleContent
         * @param vDDamping
         */
        void applyDampingOnBottle(SWBottleContent &oBottleContent, QVector<double> vDDamping);

        /**
         * @brief applyShiftOnBottle
         * @param oBottleContent
         * @param vDShifts
         */
        void applyShiftOnBottle(SWBottleContent &oBottleContent, QVector<double> vDShifts);


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
         * @param i32Modifier
         * @param dTimeTotal
         * @param dTimeBlock
         * @param sSequence
         */
        void updatePlanification(int i32Index, int i32Mode, int i32Modifier, double dTimeTotal, double dTimeBlock, QString sSequence);

        /**
         * @brief SWManipulationWorker::toggleOutPort
         * @param i32IndexPort
         */
        void toggleOutPort(int i32IndexPort);

        /**
         * @brief sequencePartTimeModifier
         * @param sSequence
         * @param vI32Times
         * @param vI32Modifiers
         */
        void sequencePartTimeModifier(const QString &sSequence, QVector<int> &vI32Times, QVector<int> &vI32Modifiers) const;

        /**
         * @brief sequencePartModifier
         * @param dCurrentTime
         * @param i32IndexSequence
         * @return
         */
        int sequencePartModifier(double dCurrentTime, cint i32IndexSequence) const;

        /**
         * @brief addAllTimeSequence
         * @param i32IndexSequence
         * @return
         */
        double addAllTimeSequence(cint i32IndexSequence) const;

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

        /**
         * @brief planificationState
         */
        void planificationState(int, bool, int, double);


    private :

        bool m_bDoLoop;                                 /**< do the work ? */
        bool m_bInitialization;                         /**< is initialized ? */

        int m_i32LoopPeriod;                            /**< loop period */

        int m_i32ConnectionsNb;                         /**< number of yarp input/output connections */
        int m_i32ModifiersNb;                           /**< number of bottle modifiers */

        // all modes
        QVector<SWBottleContent> m_vBottlesContent;     /**< current bottles content */
        QVector<int> m_vPlanMode;                       /**< planification mode for each connection */
        QVector<int> m_vPlanModifier;                   /**< planification modifier for each connection */
        QVector<double> m_vTimePlanification;           /**< time remaining for each connection (not for sequence mode) */
        QVector<bool> m_vBActiveBottlesOUTSend;         /**< is output active ? */
        QVector<bool> m_vBStartBottlesOutSend;          /**< is started ? */
        QVector<QVector<double> > m_vVDamping;          /**< damping to apply for each connection (not for normal mode) */
        QVector<QVector<double> > m_vVShift;            /**< shift to add for each connection (not for normal mode) */

        // NORMAL / MODIFIED mode
        QVector<double> m_vPlanTimeTotal;               /**< total time for the planification for each connection */

        // RANDOM mode
        QVector<double> m_vPlanTimeBlock;               /**< time block for each connection (only for random mode) */

        // SEQUENCE mode
        QVector<QVector<int> > m_vPlanSequenceTime;     /**< sequence planification time for each connection (only for sequence mode) */
        QVector<QVector<int> > m_vPlanSequenceModifier; /**< sequence planification modifier for each connection (only for sequence mode) */
        QVector<double> m_vTimePlanificationSequence;   /**< time remaining for each connection (only for sequence mode) */


        // mutex for protecting data from slot call modifications
        QReadWriteLock m_oLoopMutex;                    /**< mutex for the main worker loop */
        QReadWriteLock m_oActiveBottleMutex;            /**< mutex for activating/desactivating bottles output */
        QReadWriteLock m_oStartBottleMutex;             /**< mutex for starting/stoping the planifications */
        QReadWriteLock m_oModifiersMutex;               /**< mutex for update the bottles modifiers */

        QVector<QString> m_vSManipulationINPortName;    /**< vector of yarp IN port names */
        QVector<QString> m_vSManipulationOUTPortName;   /**< vector of yarp OUT port names */
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
         * @brief updateTimesDisplay
         */
        void updateTimesDisplay(int, bool, int, double);

        /**
         * @brief switchDisplayModifiersValues
         * @param i32Index
         */
        void switchDisplayModifiersValues(int i32Index);

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

        /**
         * @brief stopBottlePlanification
         */
        void stopBottlePlanification();

        /**
         * @brief stopAllBottlesPlanification
         */
        void stopAllBottlesPlanification();

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
        void sendPlanificationParams(int, int, int, double, double, QString);

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

        // styles
        QString m_sStyleGreenButton;                /**< ... */
        QString m_sStyleRedButton;                  /**< ... */

        // timers
        double m_dTimeLoop;                         /**< time between two time event call */
        QBasicTimer m_oTimer;                       /**< timer used for refreshing paramters data */

        // timeout
        QVector<double> m_vDTimeOutINBottles;       /**< timeout in bottles container */

        int m_i32YarpConnectNumber;                 /**< number of input/output yarp connections  */
        int m_i32ModifiersNumber;                   /**< number of bottle values to apply modifiers */

        // display timers
        QVector<QString> m_vSPlanStarted;           /**< ... */
        QVector<QString> m_vSTimeRemaining;         /**< ... */
        QVector<QString> m_vSModifiers;             /**< ... */

        // parameters container corrsponding to a connection row
        QVector<QVector<double> > m_vVDamping;      /**< container of each connection input dampings parameters */
        QVector<QVector<double> > m_vVShift;        /**< container of each connection input shifts parameters */
        QVector<double> m_vDTotalTime;              /**< container of each connection input total time */
        QVector<double> m_vDBlockTime;              /**< container of each connection input block time */
        QVector<int> m_vI32ModePlan;                /**< container of each connection input planification mode */
        QVector<QString> m_vSSequence;              /**< container of each connection input sequence */

        // widgets containers
        QVector<QLineEdit*>      m_vBottleDisplayLineEdit;      /**< bottle display line edit container */
        QVector<QLineEdit*>      m_vDampingDisplayLineEdit;     /**< damping display line edit container */
        QVector<QPushButton*>    m_vActiveButtons;              /**< active buttons container */
        QVector<QLabel*>         m_vSourceLabels;               /**< source labels container */
        QVector<QSlider*>        m_vDampingSliders;             /**< damping sliders container */
        QVector<QDoubleSpinBox*> m_vShiftDSpinBoxes;            /**< const shift spinbox container */
        QVector<QRadioButton*>   m_vPlanificationRadioButtons;  /**< planification radio buttons container */
};


#endif
