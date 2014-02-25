

/**
 * \file SWManipulationInterface.cpp
 * \brief Defines SWManipulation class
 * \author Florian Lance
 * \date 20/02/14
 */

// SWOOZ
#include "SWManipulationInterface.h"
#include <commonTypes.h>
#include "SWTrackingDevice.h"

// MOC
#include "moc_SWManipulationInterface.cpp"

// STD
#include <ctime>
#include <sstream>


using namespace swExcept;

// ########################### SWManipulationWorker


SWManipulationWorker::SWManipulationWorker() : m_bDoLoop(true), m_bInitialization(true), m_i32LoopPeriod(10)
{
    uint l_ui32ConnectionsNb = 5;
    uint l_ui32ModifiersNb = 9;

    // init ports vectors and bottles
    m_vBottlesContent     = std::vector<SWBottleContent>(l_ui32ConnectionsNb);
    m_vManipulationINPort = QVector<yarp::os::BufferedPort<yarp::os::Bottle>*>(l_ui32ConnectionsNb, NULL);
    m_vManipulationOUTPort= QVector<yarp::os::BufferedPort<yarp::os::Bottle>*>(l_ui32ConnectionsNb, NULL);

    // init paramters vectors
    m_vPlanMode     = QVector<int>(l_ui32ConnectionsNb, 0);
    m_vPlanTimeTotal= QVector<double>(l_ui32ConnectionsNb, 100.0);
    m_vPlanTimeBlock= QVector<double>(l_ui32ConnectionsNb, 25.0);
    m_vPlanSequence = QVector<QString>(l_ui32ConnectionsNb, QString("25 -25 25 -25 25 -25"));
    m_vVShifts      = QVector<QVector<double> >(l_ui32ConnectionsNb, QVector<double>(l_ui32ModifiersNb, 1.0));
    m_vVConstants   = QVector<QVector<double> >(l_ui32ConnectionsNb, QVector<double>(l_ui32ModifiersNb, 0.0));

    m_vBActiveBottlesOUTSend = QVector<bool>(l_ui32ConnectionsNb, false);
    m_vStartBottlesOutSend   = QVector<bool>(l_ui32ConnectionsNb, false);

    // set in/out ports names
        for(uint ii = 0; ii < l_ui32ConnectionsNb; ++ii)
        {
            std::string l_sPortName("/manipulation/");
            std::string l_sIN("in"), l_sOUT("out");
            std::ostringstream l_os;
            l_os << ii;
            l_sIN  += l_os.str();
            l_sOUT += l_os.str();
            m_vSManipulationINPortName.push_back(l_sPortName + l_sIN);
            m_vSManipulationOUTPortName.push_back(l_sPortName + l_sOUT);

            // open port
            m_vManipulationINPort[ii]  = new yarp::os::BufferedPort<yarp::os::Bottle>();
            m_vManipulationOUTPort[ii] = new yarp::os::BufferedPort<yarp::os::Bottle>();

            if(!m_vManipulationINPort[ii]->open(m_vSManipulationINPortName[ii].c_str()))
            {
                std::cerr << "-ERROR : init port "<< m_vSManipulationINPortName[ii] << " failure. " << std::endl;
                m_bInitialization = false;
                break;
            }

            if(!m_vManipulationOUTPort[ii]->open(m_vSManipulationOUTPortName[ii].c_str()))
            {
                std::cerr << "-ERROR : init port " << m_vSManipulationOUTPortName[ii] << " failure. " << std::endl;
                m_bInitialization = false;
                break;
            }
        }
}

SWManipulationWorker::~SWManipulationWorker()
{
    for(int ii = 0; ii < m_vManipulationINPort.size(); ++ii)
    {
        m_vManipulationINPort[ii]->interrupt();
        m_vManipulationINPort[ii]->close();
        delete m_vManipulationINPort[ii];
    }

    for(int ii = 0; ii < m_vManipulationOUTPort.size(); ++ii)
    {
        m_vManipulationOUTPort[ii]->interrupt();
        m_vManipulationOUTPort[ii]->close();
        delete m_vManipulationOUTPort[ii];
    }

    yarp::os::Network::fini();
}

bool SWManipulationWorker::isInitialized() const
{
    return m_bInitialization;
}

void SWManipulationWorker::startLoop()
{
    bool l_bDoLoop;
    m_oLoopMutex.lockForRead();
        l_bDoLoop = m_bDoLoop;
    m_oLoopMutex.unlock();

    QTime l_oStartTime;
    l_oStartTime.start();

    while(l_bDoLoop)
    {
        // control the time of the loop
            while(l_oStartTime.elapsed() < m_i32LoopPeriod)
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents, m_i32LoopPeriod - l_oStartTime.elapsed());
            }
            l_oStartTime.restart();

        // retrieve bottles content
            QVector<bool> l_vBottlesReceive(m_vBottlesContent.size(), false);
            for(int ii = 0; ii < m_vManipulationINPort.size(); ++ii)
            {
                l_vBottlesReceive[ii]  = retrieveBottleContent(m_vBottlesContent[ii], *m_vManipulationINPort[ii]);
            }

        // TIME MANAGEMENTS // SEQUENCE / BLOCKS



             // ################################################################################################## J'en étais là !!!
        //
//            m_oStartBottleMutex.lockForRead();
//                QVector<bool> l_vBStartBottlesOutSend = m_vBStartBottlesOutSend;
//            m_oStartBottleMutex.unlock();

//        // init timers if the plan hasn't started
//            m_oModifiersMutex.lockForRead();
//                for(int ii = 0; ii < l_vBStartBottlesOutSend.size(); ++ii)
//                {
//                    if(!l_vBStartBottlesOutSend[ii])
//                    {
//                        m_vTimePlanification[ii] = m_vPlanTimeTotal[ii];
//                    }
//                }
//            m_oModifiersMutex.unlock();

//        QVector<bool> l_vDO(l_vBStartBottlesOutSend.size(), false);
//        QVector<bool> l_vMODIFIED(l_vBStartBottlesOutSend.size(), false);

//        for(int ii = 0; ii < l_vBStartBottlesOutSend.size(); ++ii)
//        {
//            if(m_vTimePlanification[ii])
//            {

//            }

//            if(m_vPlanMode[ii] == NORMAL)
//            {
//                l_vDO[ii] = true;
//            }
//            else if(m_vPlanMode[ii] == MODIFIED)
//            {
//                l_vDO[ii] = true;
//                l_vMODIFIED[ii] = true;
//            }
//            else if(m_vPlanMode[ii] == RANDOM)
//            {

//            }
//            else if(m_vPlanMode[ii] == SEQUENCE)
//            {

//            }
//        }




        // apply modifers
            m_oModifiersMutex.lockForRead();
                for(int ii = 0; ii < l_vBottlesReceive.size(); ++ii)
                {
                    if(l_vBottlesReceive[ii])
                    {
                        applyModifierOnBottle(m_vBottlesContent[ii], m_vVShifts[ii], m_vVConstants[ii]);
                    }
                }
            m_oModifiersMutex.unlock();



        // send bottles to the manipulation/outX ports
            for(int ii = 0; ii < m_vBActiveBottlesOUTSend.size(); ++ii)
            {
    //            if(!) continue

                m_oActiveBottleMutex.lockForRead();
                    bool l_bIsActive = m_vBActiveBottlesOUTSend[ii];
                m_oActiveBottleMutex.unlock();

                if(l_bIsActive)
                {
                    yarp::os::Bottle &l_oBottle = m_vManipulationOUTPort[ii]->prepare();
                    l_oBottle.clear();

                     // device lib id
                    l_oBottle.addInt(m_vBottlesContent[ii].idLib);

                    for(uint jj = 0; jj < m_vBottlesContent[ii].dValues.size(); ++jj)
                    {
                        l_oBottle.addDouble(m_vBottlesContent[ii].dValues[jj]);
                    }

                    m_vManipulationOUTPort[ii]->write();
                }
            }




        // send bottles to the interface to be displayed
            for(int ii = 0; ii < l_vBottlesReceive.size(); ++ii)
            {
                if(l_vBottlesReceive[ii])
                {
                    emit sendBottle(m_vBottlesContent[ii], ii);
                }
            }

        // check end of the loop
            m_oLoopMutex.lockForRead();
                l_bDoLoop = m_bDoLoop;
            m_oLoopMutex.unlock();
    }
}

void SWManipulationWorker::stopLoop()
{
    m_oLoopMutex.lockForWrite();
        m_bDoLoop = false;
    m_oLoopMutex.unlock();
}

void SWManipulationWorker::applyModifierOnBottle(SWBottleContent &oBottleContent, QVector<double> vDShifts, QVector<double> vDConst)
{
    for(uint ii = 0; ii < oBottleContent.dValues.size(); ++ii)
    {
        oBottleContent.dValues[ii] *= vDShifts[ii];
        oBottleContent.dValues[ii] += vDConst[ii];
    }
}

bool SWManipulationWorker::retrieveBottleContent(SWBottleContent &oBottleContent, yarp::os::BufferedPort<yarp::os::Bottle> &oPort)
{
    yarp::os::Bottle *l_pIN = NULL;

    l_pIN = oPort.read(false);

    if(!l_pIN)
    {
        return false;
    }

    oBottleContent.idLib = l_pIN->get(0).asInt();
    std::vector<double> l_vDValues;
    for(int ii = 1; ii < l_pIN->size(); ++ii)
    {
        l_vDValues.push_back(l_pIN->get(ii).asDouble());
    }
    oBottleContent.dValues = l_vDValues;

//    oBottleContent.display();

    return true;
}

void SWManipulationWorker::updateModifier(QVector<double> vShifts, QVector<double> vConsts, int i32Index)
{
    m_oModifiersMutex.lockForWrite();
        m_vVShifts[i32Index]   = vShifts;
        m_vVConstants[i32Index] = vConsts;
    m_oModifiersMutex.unlock();
}

void SWManipulationWorker::updatePlanification(int i32Index, int i32Mode, double dTimeTotal, double dTimeBlock, QString sSequence)
{
    m_oModifiersMutex.lockForWrite();
        m_vPlanMode[i32Index]      = i32Mode;
        m_vPlanTimeTotal[i32Index] = dTimeTotal;
        m_vPlanTimeBlock[i32Index] = dTimeBlock;
        m_vPlanSequence[i32Index]  = sSequence;
    m_oModifiersMutex.unlock();
}

void SWManipulationWorker::toggleOutPort(int i32IndexPort)
{
    m_oActiveBottleMutex.lockForWrite();
        m_vBActiveBottlesOUTSend[i32IndexPort] = !m_vBActiveBottlesOUTSend[i32IndexPort];
    m_oActiveBottleMutex.unlock();
}

void SWManipulationWorker::updateBottleStart(int i32IndexPort, bool bStart)
{
    m_oStartBottleMutex.lockForWrite();
        m_vStartBottlesOutSend[i32IndexPort] = bStart;
    m_oStartBottleMutex.unlock();
}


// ########################### SWManipulationInterface

SWManipulationInterface::SWManipulationInterface() : m_uiManipulation(new Ui::SWUI_Manipulation())//, m_oTimer(new QBasicTimer) //m_fDelay(0.f)
{
    m_i32YarpConnectInputNumber = 5;
    m_i32ShiftConstNumber      = 9;
    m_bTimeLoop = 100;

    // init Qt interface
        // init main widget
            m_uiManipulation->setupUi(this);
            this->setWindowTitle(QString("SWoOz : Manipulation"));

        // init new meta types
            qRegisterMetaType<SWBottleContent>("SWBottleContent");
            qRegisterMetaType<QVector<double> >("QVector<double>");

        // fill QlistWidget with yarp connect inputs
            QStringList l_slYarpINConnections;
            for(int ii = 0; ii < m_i32YarpConnectInputNumber; ++ii)
            {
                std::ostringstream l_os;
                l_os << ii;
                l_slYarpINConnections.push_back(QString::fromStdString("/manipulation/in" + l_os.str()));
            }
            m_uiManipulation->lwManipulation->addItems(l_slYarpINConnections);
            m_uiManipulation->lwManipulation->setCurrentRow(0);

        // init timeout and labels container
            m_vDTimeOutINBottles = QVector<double>(m_i32YarpConnectInputNumber, 0.0);
            m_vSourceLabels = QVector<QLabel*>(m_i32YarpConnectInputNumber, NULL);
            m_vSourceLabels[0] = m_uiManipulation->laSource0;
            m_vSourceLabels[1] = m_uiManipulation->laSource1;
            m_vSourceLabels[2] = m_uiManipulation->laSource2;
            m_vSourceLabels[3] = m_uiManipulation->laSource3;
            m_vSourceLabels[4] = m_uiManipulation->laSource4;

        // init buttons container
            m_vActiveButtons = QVector<QPushButton*>(m_i32YarpConnectInputNumber, NULL);
            m_vActiveButtons[0] = m_uiManipulation->pbActive0;
            m_vActiveButtons[1] = m_uiManipulation->pbActive1;
            m_vActiveButtons[2] = m_uiManipulation->pbActive2;
            m_vActiveButtons[3] = m_uiManipulation->pbActive3;
            m_vActiveButtons[4] = m_uiManipulation->pbActive4;

        // init line edit container
            m_vShiftDisplayLineEdit = QVector<QLineEdit*>(m_i32ShiftConstNumber, NULL);
            m_vShiftDisplayLineEdit[0] = m_uiManipulation->leSV1;
            m_vShiftDisplayLineEdit[1] = m_uiManipulation->leSV2;
            m_vShiftDisplayLineEdit[2] = m_uiManipulation->leSV3;
            m_vShiftDisplayLineEdit[3] = m_uiManipulation->leSV4;
            m_vShiftDisplayLineEdit[4] = m_uiManipulation->leSV5;
            m_vShiftDisplayLineEdit[5] = m_uiManipulation->leSV6;
            m_vShiftDisplayLineEdit[6] = m_uiManipulation->leSV7;
            m_vShiftDisplayLineEdit[7] = m_uiManipulation->leSV8;
            m_vShiftDisplayLineEdit[8] = m_uiManipulation->leSV9;

        // init line edit bottle display container
            m_vBottleDisplayLineEdit = QVector<QLineEdit*>(m_i32YarpConnectInputNumber, NULL);
            m_vBottleDisplayLineEdit[0] = m_uiManipulation->leMan0;
            m_vBottleDisplayLineEdit[1] = m_uiManipulation->leMan1;
            m_vBottleDisplayLineEdit[2] = m_uiManipulation->leMan2;
            m_vBottleDisplayLineEdit[3] = m_uiManipulation->leMan3;
            m_vBottleDisplayLineEdit[4] = m_uiManipulation->leMan4;

        // init modifiers / planifications containers
            m_vVShifts    = QVector<QVector<double> >(m_i32YarpConnectInputNumber, QVector<double>(m_i32ShiftConstNumber,1.0));
            m_vVConstants = QVector<QVector<double> >(m_i32YarpConnectInputNumber, QVector<double>(m_i32ShiftConstNumber,0.0));
            m_vDTotalTime = QVector<double>(m_i32YarpConnectInputNumber, 100.0);
            m_vDBlockTime = QVector<double>(m_i32YarpConnectInputNumber, 25.0);
            m_vI32ModePlan= QVector<int>(m_i32YarpConnectInputNumber, 0);
            m_vSSequence  = QVector<QString>(m_i32YarpConnectInputNumber, QString("25 -25 25 -25 25 -25"));

        // init sliders containers
            m_vShiftSliders  =  QVector<QSlider*>(m_i32ShiftConstNumber,NULL);
            m_vShiftSliders[0] = m_uiManipulation->hsShift1;
            m_vShiftSliders[1] = m_uiManipulation->hsShift2;
            m_vShiftSliders[2] = m_uiManipulation->hsShift3;
            m_vShiftSliders[3] = m_uiManipulation->hsShift4;
            m_vShiftSliders[4] = m_uiManipulation->hsShift5;
            m_vShiftSliders[5] = m_uiManipulation->hsShift6;
            m_vShiftSliders[6] = m_uiManipulation->hsShift7;
            m_vShiftSliders[7] = m_uiManipulation->hsShift8;
            m_vShiftSliders[8] = m_uiManipulation->hsShift9;

        // init double spinboxes containers
            m_vConstDSpinBoxes = QVector<QDoubleSpinBox*>(m_i32ShiftConstNumber,NULL);
            m_vConstDSpinBoxes[0] = m_uiManipulation->dsbCV1;
            m_vConstDSpinBoxes[1] = m_uiManipulation->dsbCV2;
            m_vConstDSpinBoxes[2] = m_uiManipulation->dsbCV3;
            m_vConstDSpinBoxes[3] = m_uiManipulation->dsbCV4;
            m_vConstDSpinBoxes[4] = m_uiManipulation->dsbCV5;
            m_vConstDSpinBoxes[5] = m_uiManipulation->dsbCV6;
            m_vConstDSpinBoxes[6] = m_uiManipulation->dsbCV7;
            m_vConstDSpinBoxes[7] = m_uiManipulation->dsbCV8;
            m_vConstDSpinBoxes[8] = m_uiManipulation->dsbCV9;

        // init radio button containers
            m_vPlanificationRadioButtons = QVector<QRadioButton*>(4, NULL);
            m_vPlanificationRadioButtons[0] = m_uiManipulation->rbNormals;
            m_vPlanificationRadioButtons[1] = m_uiManipulation->rbManipulated;
            m_vPlanificationRadioButtons[2] = m_uiManipulation->rbRandom;
            m_vPlanificationRadioButtons[3] = m_uiManipulation->rbSequence;

        // init worker
            m_pWManipulation = new SWManipulationWorker();

        // init connections
        //  loop -> start/stop
            QObject::connect(this, SIGNAL(startLoop()), m_pWManipulation, SLOT(startLoop()));
            QObject::connect(this, SIGNAL(stopLoop()), m_pWManipulation, SLOT(stopLoop()));
        //  push buttons
            QObject::connect(m_uiManipulation->pbResetDefaultValues,    SIGNAL(clicked()), this, SLOT(resetModifiers()));
            QObject::connect(m_uiManipulation->pbResetDefaultValuesAll, SIGNAL(clicked()), this, SLOT(resetAllModifiers()));
            QObject::connect(m_uiManipulation->pbStart,    SIGNAL(clicked()), this, SLOT(startBottlePlanification()));
            QObject::connect(m_uiManipulation->pbStartAll, SIGNAL(clicked()), this, SLOT(startAllBottlesPlanification()));

        //  display
            QObject::connect(m_pWManipulation, SIGNAL(sendBottle(SWBottleContent,int)), this, SLOT(updateBottleContentDisplay(SWBottleContent,int)));

        // modifiers
        //  listWidget
            QObject::connect(m_uiManipulation->lwManipulation, SIGNAL(currentRowChanged(int)), SLOT(switchModifiersValues(int)));
        //  update modifiers
            QObject::connect(this, SIGNAL(sendBottleModifiers(QVector<double>, QVector<double>, int)), m_pWManipulation, SLOT(updateModifier(QVector<double>, QVector<double>, int)));
        //  udpate planification params
            QObject::connect(this, SIGNAL(sendPlanificationParams(int, int, double, double, QString)),
                             m_pWManipulation, SLOT(updatePlanification(int, int, double, double, QString)));;
            QObject::connect(this, SIGNAL(startBottlePlan(int, bool)), m_pWManipulation, SLOT(updateBottleStart(int, bool)));
        //  active click buttons
            for(int ii = 0; ii < m_i32YarpConnectInputNumber; ++ii)
            {
                QObject::connect(m_vActiveButtons[ii], SIGNAL(pressed()), this, SLOT(checkActiveClick()));
            }
        //  open / close out ports
            QObject::connect(this, SIGNAL(activeOutput(int)), m_pWManipulation, SLOT(toggleOutPort(int)));

        // init thread
            m_pWManipulation->moveToThread(&m_TManipulation);
            m_TManipulation.start();

        // start main loop
            if(m_pWManipulation->isInitialized())
            {
                emit startLoop();
            }
            else
            {
                // std::cerr
            }

    m_oTimer.start(m_bTimeLoop, this);
}


SWManipulationInterface::~SWManipulationInterface()
{
    m_TManipulation.quit();
    m_TManipulation.wait();

    deleteAndNullify(m_pWManipulation);
}

void SWManipulationInterface::closeEvent(QCloseEvent *event)
{
    emit stopLoop();

    QTime l_oDieTime = QTime::currentTime().addMSecs(500);
    while( QTime::currentTime() < l_oDieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void SWManipulationInterface::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    int l_i32Index =  m_uiManipulation->lwManipulation->currentRow();

    // retrieve shifts / constants parameters
        for(int ii = 0; ii < m_vShiftSliders.size(); ++ii)
        {
            // update shift and constants arrays
                m_vVShifts[l_i32Index][ii]    = 0.01 * m_vShiftSliders[ii]->value();
                m_vVConstants[l_i32Index][ii] = m_vConstDSpinBoxes[ii]->value();

            // uddate shift line edit display
                std::ostringstream l_oS;
                l_oS << m_vVShifts[l_i32Index][ii];
                m_vShiftDisplayLineEdit[ii]->setText(QString::fromStdString(l_oS.str()));
        }
    // retrieve planification parameters
        {
            m_vDTotalTime[l_i32Index]  = m_uiManipulation->dsbTotalTime->value();
            m_vDBlockTime[l_i32Index]  = m_uiManipulation->dsbTimeBlock->value();

            for(int ii = 0; ii < m_vPlanificationRadioButtons.size(); ++ii)
            {
                if(m_vPlanificationRadioButtons[ii]->isChecked())
                {
                    m_vI32ModePlan[l_i32Index] = ii;
                    break;
                }
            }

            m_vSSequence[l_i32Index]   = m_uiManipulation->teSequence->toPlainText();
        }

    // send modifiers parameters
        for(int ii = 0; ii < m_vVShifts.size(); ++ii)
        {
            sendBottleModifiers(m_vVShifts[ii], m_vVConstants[ii], ii);
        }
    // send planification parameters
        sendPlanificationParams(l_i32Index, m_vI32ModePlan[l_i32Index], m_vDTotalTime[l_i32Index], m_vDBlockTime[l_i32Index], m_vSSequence[l_i32Index]);

    // check timeout and update display
    for(int ii = 0; ii < m_vDTimeOutINBottles.size(); ++ii)
    {
        // if no timeout
        if(m_vDTimeOutINBottles[ii] > m_bTimeLoop)
        {
            m_vDTimeOutINBottles[ii] -= m_bTimeLoop; // display
            m_vSourceLabels[ii]->setText(QString("Active"));
            m_vActiveButtons[ii]->setEnabled(true);
        }
        // if timeout
        else if(m_vDTimeOutINBottles[ii] > 0)
        {
            m_vDTimeOutINBottles[ii] -= m_bTimeLoop;
            m_vSourceLabels[ii]->setText(QString("No source."));
            m_vActiveButtons[ii]->setDisabled(true);

            // disactive output
            if(m_vActiveButtons[ii]->text() == QString("Disactive output"))
            {
                activeOutput(ii);
                toggleActiveText(ii);
            }
        }
    }
}

void SWManipulationInterface::updateBottleContentDisplay(SWBottleContent oBottle, int i32NumBottle)
{
    // set timeout value
        m_vDTimeOutINBottles[i32NumBottle] = 5000;

    // set the text to be displayed
        std::string l_sTextToDisplay = swTracking::returnStringValue(oBottle.idLib);

        for(uint ii = 0; ii < oBottle.dValues.size(); ++ii)
        {
            std::ostringstream l_os;
            l_os << oBottle.dValues[ii];
            l_sTextToDisplay += " " + l_os.str();
        }

        m_vBottleDisplayLineEdit[i32NumBottle]->setText(QString::fromStdString(l_sTextToDisplay));
}


void SWManipulationInterface::switchModifiersValues(int i32Index)
{
    for(int ii = 0; ii < m_vShiftSliders.size(); ++ii)
    {
        m_vShiftSliders[ii]->setValue((int)(100* m_vVShifts[i32Index][ii]));
        m_vConstDSpinBoxes[ii]->setValue(m_vVConstants[i32Index][ii]);
    }

    m_uiManipulation->dsbTotalTime->setValue(m_vDTotalTime[i32Index]);
    m_uiManipulation->dsbTimeBlock->setValue(m_vDBlockTime[i32Index]);
    m_vPlanificationRadioButtons[m_vI32ModePlan[i32Index]]->setChecked(true);
    m_uiManipulation->teSequence->setPlainText(m_vSSequence[i32Index]);
}

void SWManipulationInterface::resetModifiers()
{
    // reset shift / consts
        for(int ii = 0; ii < m_vShiftSliders.size(); ++ii)
        {
            m_vShiftSliders[ii]->setValue(100);
            m_vConstDSpinBoxes[ii]->setValue(0.0);
        }
    // reset planification
        m_uiManipulation->rbNormals->setChecked(true);
        m_uiManipulation->dsbTotalTime->setValue(100.0);
        m_uiManipulation->dsbTimeBlock->setValue(25.0);
        m_uiManipulation->teSequence->setPlainText(QString("25 -25 25 -25 25 -25"));
}

void SWManipulationInterface::resetAllModifiers()
{
    resetModifiers();
    m_vVShifts    = QVector<QVector<double> >(m_i32YarpConnectInputNumber, QVector<double>(m_i32ShiftConstNumber,1.0));
    m_vVConstants = QVector<QVector<double> >(m_i32YarpConnectInputNumber, QVector<double>(m_i32ShiftConstNumber,0.0));
    m_vDTotalTime = QVector<double>(m_i32ShiftConstNumber, 100.0);
    m_vDBlockTime = QVector<double>(m_i32ShiftConstNumber, 25.0);
    m_vI32ModePlan= QVector<int>(m_i32ShiftConstNumber, 0);
    m_vSSequence  = QVector<QString>(m_i32ShiftConstNumber, QString("25 -25 25 -25 25 -25"));
}

void SWManipulationInterface::checkActiveClick()
{
    for(int ii = 0; ii < m_vActiveButtons.size(); ++ii)
    {
        if(m_vActiveButtons[ii]->isDown())
        {
            activeOutput(ii);
            toggleActiveText(ii);
        }
    }
}

void SWManipulationInterface::toggleActiveText(int i32Index)
{
    if(m_vActiveButtons[i32Index]->text() == QString("Active output"))
    {
        m_vActiveButtons[i32Index]->setText(QString("Disactive output"));
    }
    else
    {
        m_vActiveButtons[i32Index]->setText(QString("Active output"));
    }
}

void SWManipulationInterface::startBottlePlanification()
{
    int l_i32Index =  m_uiManipulation->lwManipulation->currentRow();

    emit startBottlePlan(l_i32Index, true);
}

void SWManipulationInterface::startAllBottlesPlanification()
{
    for(int ii = 0; ii < m_i32YarpConnectInputNumber; ++ii)
    {
        emit startBottlePlan(ii, true);
    }
}

int main(int argc, char* argv[])
{
    // initialize yarp network
        yarp::os::Network l_oYarp;
        if (!l_oYarp.checkNetwork())
        {
            std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
            return -1;
        }

    // create module
        QApplication l_oApp(argc, argv);
        SWManipulationInterface l_oManipulationInterface;
        l_oManipulationInterface.resize(QSize(1200,600));
        l_oManipulationInterface.move(50,50);
        l_oManipulationInterface.show();

    return l_oApp.exec();
}
