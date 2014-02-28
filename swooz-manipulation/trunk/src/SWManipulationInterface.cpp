

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

QString g_sDefaultSequence("10 s10 10 d10 10 ds10 10");

SWManipulationWorker::SWManipulationWorker() : m_bDoLoop(true), m_bInitialization(true), m_i32LoopPeriod(10)
{
    m_i32ConnectionsNb = 5;
    m_i32ModifiersNb = 9;

    // init ports vectors and bottles
    m_vBottlesContent     = QVector<SWBottleContent>(m_i32ConnectionsNb);
    m_vManipulationINPort = QVector<yarp::os::BufferedPort<yarp::os::Bottle>*>(m_i32ConnectionsNb, NULL);
    m_vManipulationOUTPort= QVector<yarp::os::BufferedPort<yarp::os::Bottle>*>(m_i32ConnectionsNb, NULL);

    // init paramters vectors
    m_vPlanMode     = QVector<int>(m_i32ConnectionsNb, 0);
    m_vPlanModifier = QVector<int>(m_i32ConnectionsNb, NO_MODIF);
    m_vPlanTimeTotal= QVector<double>(m_i32ConnectionsNb, 100.0);
    m_vPlanTimeBlock= QVector<double>(m_i32ConnectionsNb, 25.0);
    m_vPlanSequenceTime     = QVector<QVector<int> >(m_i32ConnectionsNb, QVector<int>(5, 20));
    m_vPlanSequenceModifier = QVector<QVector<int> >(m_i32ConnectionsNb, QVector<int>(5, NO_MODIF));
    m_vVDamping     = QVector<QVector<double> >(m_i32ConnectionsNb, QVector<double>(m_i32ModifiersNb, 1.0));
    m_vVShift       = QVector<QVector<double> >(m_i32ConnectionsNb, QVector<double>(m_i32ModifiersNb, 0.0));

    m_vBActiveBottlesOUTSend    = QVector<bool>(m_i32ConnectionsNb, false);
    m_vBStartBottlesOutSend     = QVector<bool>(m_i32ConnectionsNb, false);
    m_vTimePlanification        = QVector<double>(m_i32ConnectionsNb, 0.0);
    m_vTimePlanificationSequence= QVector<double>(m_i32ConnectionsNb, 0.0);

    // set in/out ports names
        for(int ii = 0; ii < m_i32ConnectionsNb; ++ii)
        {
            QString l_sPortName("/manipulation/");
            QString l_sIN("in"), l_sOUT("out");
            l_sIN += QString::number(ii, 10);
            l_sOUT+= QString::number(ii, 10);
            m_vSManipulationINPortName.push_back(l_sPortName + l_sIN);
            m_vSManipulationOUTPortName.push_back(l_sPortName + l_sOUT);

            // open port
            m_vManipulationINPort[ii]  = new yarp::os::BufferedPort<yarp::os::Bottle>();
            m_vManipulationOUTPort[ii] = new yarp::os::BufferedPort<yarp::os::Bottle>();

            if(!m_vManipulationINPort[ii]->open(m_vSManipulationINPortName[ii].toStdString().c_str()))
            {
                std::cerr << "-ERROR : init port "<< m_vSManipulationINPortName[ii].toStdString() << " failure. " << std::endl;
                m_bInitialization = false;
                break;
            }
            if(!m_vManipulationOUTPort[ii]->open(m_vSManipulationOUTPortName[ii].toStdString().c_str()))
            {
                std::cerr << "-ERROR : init port " << m_vSManipulationOUTPortName[ii].toStdString() << " failure. " << std::endl;
                m_bInitialization = false;
                break;
            }
        }
}

SWManipulationWorker::~SWManipulationWorker()
{
    for(int ii = 0; ii < m_i32ConnectionsNb; ++ii)
    {
        m_vManipulationINPort[ii]->interrupt();
        m_vManipulationINPort[ii]->close();
        delete m_vManipulationINPort[ii];
    }

    for(int ii = 0; ii < m_i32ConnectionsNb; ++ii)
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
            for(int ii = 0; ii < m_i32ConnectionsNb; ++ii)
            {
                l_vBottlesReceive[ii]  = retrieveBottleContent(m_vBottlesContent[ii], *m_vManipulationINPort[ii]);
            }

        // defines what to do according to the planification of each connection
            // retrieve mutex protected values
                m_oStartBottleMutex.lockForRead();
                    QVector<bool> l_vBStartBottlesOutSend = m_vBStartBottlesOutSend;
                m_oStartBottleMutex.unlock();

                m_oModifiersMutex.lockForRead();
                    QVector<double>           l_vPlanTimeTotal  = m_vPlanTimeTotal;
                    QVector<QVector<double> > l_vVDamping       = m_vVDamping;
                    QVector<QVector<double> > l_vVShift         = m_vVShift;
                    QVector<int>              l_vPlanMode       = m_vPlanMode;
                m_oModifiersMutex.unlock();

                m_oActiveBottleMutex.lockForRead();
                    QVector<bool> l_vBActiveBottlesOUTSend = m_vBActiveBottlesOUTSend;
                m_oActiveBottleMutex.unlock();

            // init actions booleans for each connection
                QVector<bool> l_vDO(l_vBStartBottlesOutSend.size(), false);
                QVector<bool> l_vMODIFIED(l_vBStartBottlesOutSend.size(), false);

                for(int ii = 0; ii < m_i32ConnectionsNb; ++ii)
                {
                    if(l_vBStartBottlesOutSend[ii])
                    {
                        // normal mode : no modification on bottles, lasts until time total is less than 0
                        if(l_vPlanMode[ii] == NORMAL)
                        {
                            // update time
                            m_vTimePlanification[ii] -= m_i32LoopPeriod*0.001;

                            // check remaining time
                            if(m_vTimePlanification[ii] > 0.0)
                            {
                                l_vDO[ii] = true;
                            }
                            else
                            {
                                m_oStartBottleMutex.lockForWrite();
                                    m_vBStartBottlesOutSend[ii] = false;
                                m_oStartBottleMutex.unlock();
                            }
                        }
                        // modified mode : apply modification on bottles, lasts until time total is less than 0
                        else if(l_vPlanMode[ii] == MODIFIED)
                        {
                            // update time
                            m_vTimePlanification[ii] -= m_i32LoopPeriod*0.001;

                            // check remaining time
                            if(m_vTimePlanification[ii] > 0.0)
                            {
                                l_vDO[ii]       = true;
                                l_vMODIFIED[ii] = true;
                            }
                            else
                            {
                                m_oStartBottleMutex.lockForWrite();
                                    m_vBStartBottlesOutSend[ii] = false;
                                m_oStartBottleMutex.unlock();
                            }
                        }
                         // random mode : ...
                        else if(l_vPlanMode[ii] == RANDOM)
                        {
                            // ...
                        }
                        // sequence mode : analyse the current sequence to check if the modification must be done, last until the cumulated times of the sequences is less than 0
                        else if(l_vPlanMode[ii] == SEQUENCE)
                        {
                            // update time
                            m_vTimePlanificationSequence[ii] -= m_i32LoopPeriod*0.001;

                            // check remaining time
                            if(m_vTimePlanificationSequence[ii] > 0.0)
                            {
                                l_vDO[ii] = true;

                                // define current modifier
                                m_vPlanModifier[ii] = sequencePartModifier(m_vTimePlanificationSequence[ii], ii);

                                if(m_vPlanModifier[ii] != NO_MODIF)
                                {
                                    l_vMODIFIED[ii] = true;
                                }
                            }
                            else
                            {
                                m_oStartBottleMutex.lockForWrite();
                                    m_vBStartBottlesOutSend[ii] = false;
                                m_oStartBottleMutex.unlock();
                            }
                        }
                    }
                    else
                    {
                        // if no start planification update the total time with the current value
                        m_vTimePlanification[ii]         = l_vPlanTimeTotal[ii];
                        m_vTimePlanificationSequence[ii] = addAllTimeSequence(ii);
                    }
                }

        // send bottles to the manipulation/outX ports
            for(int ii = 0; ii < m_i32ConnectionsNb; ++ii)
            {    
                // apply modifier on the bottle
                    if(l_vMODIFIED[ii])
                    {
                        if(m_vPlanModifier[ii] == DAMPING || m_vPlanModifier[ii] == DAMPING_AND_SHIFT)
                        {
                            applyDampingOnBottle(m_vBottlesContent[ii], l_vVDamping[ii]);
                        }

                        if(m_vPlanModifier[ii] == SHIFT || m_vPlanModifier[ii] == DAMPING_AND_SHIFT)
                        {
                            applyShiftOnBottle(m_vBottlesContent[ii], l_vVShift[ii]);
                        }
                    }

                // if port is active and planification says to do it and the bottle is received
                    if(l_vBActiveBottlesOUTSend[ii] && l_vDO[ii] && l_vBottlesReceive[ii])
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


            for(int ii = 0; ii < m_i32ConnectionsNb; ++ii)
            {
                // send bottles to the interface to be displayed
                if(l_vBottlesReceive[ii])
                {
                    emit sendBottle(m_vBottlesContent[ii], ii);
                }

                double l_dTime = m_vTimePlanification[ii];

                if(l_vPlanMode[ii] == SEQUENCE)
                {
                        l_dTime = m_vTimePlanificationSequence[ii];
                }

                emit planificationState(ii, l_vDO[ii], m_vPlanModifier[ii], l_dTime);
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


void SWManipulationWorker::applyDampingOnBottle(SWBottleContent &oBottleContent, QVector<double> vDDamping)
{
    for(uint ii = 0; ii < oBottleContent.dValues.size(); ++ii)
    {
        oBottleContent.dValues[ii] *= vDDamping[ii];
    }
}

void SWManipulationWorker::applyShiftOnBottle(SWBottleContent &oBottleContent, QVector<double> vDShifts)
{
    for(uint ii = 0; ii < oBottleContent.dValues.size(); ++ii)
    {
        oBottleContent.dValues[ii] += vDShifts[ii];
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

    return true;
}

void SWManipulationWorker::updateModifier(QVector<double> vShifts, QVector<double> vConsts, int i32Index)
{
    m_oModifiersMutex.lockForWrite();
        m_vVDamping[i32Index]   = vShifts;
        m_vVShift[i32Index] = vConsts;
    m_oModifiersMutex.unlock();
}

void SWManipulationWorker::updatePlanification(int i32Index, int i32Mode, int i32Modifier, double dTimeTotal, double dTimeBlock, QString sSequence)
{
    m_oModifiersMutex.lockForWrite();
        m_vPlanMode[i32Index]      = i32Mode;
        m_vPlanTimeTotal[i32Index] = dTimeTotal;
        m_vPlanTimeBlock[i32Index] = dTimeBlock;
        sequencePartTimeModifier(sSequence, m_vPlanSequenceTime[i32Index], m_vPlanSequenceModifier[i32Index]);
        m_vPlanModifier[i32Index]  = i32Modifier;
    m_oModifiersMutex.unlock();
}

void SWManipulationWorker::sequencePartTimeModifier(const QString &sSequence, QVector<int> &vI32Times, QVector<int> &vI32Modifiers) const
{
    // init times/modifier container
    vI32Times.clear();
    vI32Modifiers.clear();

    QString l_sSection(" ");

    int ii = 0;
    while(l_sSection != "")
    {
        l_sSection = sSequence.section(' ', ii ,ii);

        if(l_sSection != "")
        {
            bool l_bConvOk;
            int l_i32Modifier = NO_MODIF;

            if(l_sSection[0] == 'd')
            {
                l_sSection.remove(0,1);

                if(l_sSection[0] == 's')
                {
                    l_sSection.remove(0,1);
                    l_i32Modifier = DAMPING_AND_SHIFT;
                }
                else
                {
                    l_i32Modifier = DAMPING;
                }
            }
            else if(l_sSection[0] == 's')
            {
                l_sSection.remove(0,1);

                if(l_sSection[0] == 'd')
                {
                    l_sSection.remove(0,1);
                    l_i32Modifier = DAMPING_AND_SHIFT;
                }
                else
                {
                    l_i32Modifier = SHIFT;
                }
            }

            int l_i32SquencePart = l_sSection.toInt(&l_bConvOk, 10);

            if(l_bConvOk)
            {
                vI32Times.push_front(l_i32SquencePart);
                vI32Modifiers.push_front(l_i32Modifier);
            }
        }
        ++ii;
    }
}


int SWManipulationWorker::sequencePartModifier(double dCurrentTime, cint i32IndexSequence) const
{
    for(int ii = 0; ii < m_vPlanSequenceTime[i32IndexSequence].size(); ++ii)
    {
        dCurrentTime -= m_vPlanSequenceTime[i32IndexSequence][ii];

        if(dCurrentTime < 0)
        {
            return m_vPlanSequenceModifier[i32IndexSequence][ii];
        }
    }

    return NO_MODIF;
}


double SWManipulationWorker::addAllTimeSequence(cint i32IndexSequence) const
{
    double l_dTotal = 0.0;

    for(int ii = 0; ii < m_vPlanSequenceTime[i32IndexSequence].size(); ++ii)
    {
        if(m_vPlanSequenceTime[i32IndexSequence][ii] < 0)
        {
            l_dTotal -= m_vPlanSequenceTime[i32IndexSequence][ii];
        }
        else
        {
            l_dTotal += m_vPlanSequenceTime[i32IndexSequence][ii];
        }
    }

    return l_dTotal;
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
        m_vBStartBottlesOutSend[i32IndexPort] = bStart;
    m_oStartBottleMutex.unlock();
}



// ########################### SWManipulationInterface

SWManipulationInterface::SWManipulationInterface() : m_uiManipulation(new Ui::SWUI_Manipulation())
{
    m_i32YarpConnectNumber = 5;
    m_i32ModifiersNumber      = 9;
    m_dTimeLoop = 100;

    // init Qt interface
        // init main widget
            m_uiManipulation->setupUi(this);
            this->setWindowTitle(QString("SWoOz : Manipulation"));
            this->setWindowIcon(QIcon(QString("../data/images/logos/icon_swooz.png")));
            m_uiManipulation->pbHelpNORMAL->setEnabled(true);
            m_uiManipulation->pbHelpNORMAL->setToolTip("test1");
            m_uiManipulation->pbHelpMOD->setToolTip("test1");
            m_uiManipulation->pbHelpRAND->setToolTip("test1");
            m_uiManipulation->pbHelpSEQ->setToolTip("test1");

        // init new meta types
            qRegisterMetaType<SWBottleContent>("SWBottleContent");
            qRegisterMetaType<QVector<double> >("QVector<double>");

        // fill QlistWidget with yarp connect inputs
            QStringList l_slYarpINConnections;
            for(int ii = 0; ii < m_i32YarpConnectNumber; ++ii)
            {
                l_slYarpINConnections.push_back(QString("/manipulation/in") + QString::number(ii, 10));
            }
            m_uiManipulation->lwManipulation->addItems(l_slYarpINConnections);
            m_uiManipulation->lwManipulation->setCurrentRow(0);

        // init timeout and labels container
            m_vDTimeOutINBottles = QVector<double>(m_i32YarpConnectNumber, 0.0);
            m_vSourceLabels = QVector<QLabel*>(m_i32YarpConnectNumber, NULL);
            m_vSourceLabels[0] = m_uiManipulation->laSource0;
            m_vSourceLabels[1] = m_uiManipulation->laSource1;
            m_vSourceLabels[2] = m_uiManipulation->laSource2;
            m_vSourceLabels[3] = m_uiManipulation->laSource3;
            m_vSourceLabels[4] = m_uiManipulation->laSource4;

        // init buttons container
            m_vActiveButtons = QVector<QPushButton*>(m_i32YarpConnectNumber, NULL);
            m_vActiveButtons[0] = m_uiManipulation->pbActive0;
            m_vActiveButtons[1] = m_uiManipulation->pbActive1;
            m_vActiveButtons[2] = m_uiManipulation->pbActive2;
            m_vActiveButtons[3] = m_uiManipulation->pbActive3;
            m_vActiveButtons[4] = m_uiManipulation->pbActive4;

        // init line edit container
            m_vDampingDisplayLineEdit = QVector<QLineEdit*>(m_i32ModifiersNumber, NULL);
            m_vDampingDisplayLineEdit[0] = m_uiManipulation->leDV0;
            m_vDampingDisplayLineEdit[1] = m_uiManipulation->leDV1;
            m_vDampingDisplayLineEdit[2] = m_uiManipulation->leDV2;
            m_vDampingDisplayLineEdit[3] = m_uiManipulation->leDV3;
            m_vDampingDisplayLineEdit[4] = m_uiManipulation->leDV4;
            m_vDampingDisplayLineEdit[5] = m_uiManipulation->leDV5;
            m_vDampingDisplayLineEdit[6] = m_uiManipulation->leDV6;
            m_vDampingDisplayLineEdit[7] = m_uiManipulation->leDV7;
            m_vDampingDisplayLineEdit[8] = m_uiManipulation->leDV8;

        // init line edit bottle display container
            m_vBottleDisplayLineEdit = QVector<QLineEdit*>(m_i32YarpConnectNumber, NULL);
            m_vBottleDisplayLineEdit[0] = m_uiManipulation->leMan0;
            m_vBottleDisplayLineEdit[1] = m_uiManipulation->leMan1;
            m_vBottleDisplayLineEdit[2] = m_uiManipulation->leMan2;
            m_vBottleDisplayLineEdit[3] = m_uiManipulation->leMan3;
            m_vBottleDisplayLineEdit[4] = m_uiManipulation->leMan4;

        // init modifiers / planifications containers
            m_vVDamping    = QVector<QVector<double> >(m_i32YarpConnectNumber, QVector<double>(m_i32ModifiersNumber,1.0));
            m_vVShift = QVector<QVector<double> >(m_i32YarpConnectNumber, QVector<double>(m_i32ModifiersNumber,0.0));
            m_vDTotalTime = QVector<double>(m_i32YarpConnectNumber, 100.0);
            m_vDBlockTime = QVector<double>(m_i32YarpConnectNumber, 25.0);
            m_vI32ModePlan= QVector<int>(m_i32YarpConnectNumber, 0);
            m_vSSequence  = QVector<QString>(m_i32YarpConnectNumber, g_sDefaultSequence);

        // init display timers text vectors
            m_vSPlanStarted       = QVector<QString>(m_i32YarpConnectNumber, QString("No"));
            m_vSModifiers         = QVector<QString>(m_i32YarpConnectNumber, QString("NO_MODIF"));
            m_vSTimeRemaining     = QVector<QString>(m_i32YarpConnectNumber, QString("0.0"));

        // init sliders containers
            m_vDampingSliders  =  QVector<QSlider*>(m_i32ModifiersNumber,NULL);
            m_vDampingSliders[0] = m_uiManipulation->hsDamping0;
            m_vDampingSliders[1] = m_uiManipulation->hsDamping1;
            m_vDampingSliders[2] = m_uiManipulation->hsDamping2;
            m_vDampingSliders[3] = m_uiManipulation->hsDamping3;
            m_vDampingSliders[4] = m_uiManipulation->hsDamping4;
            m_vDampingSliders[5] = m_uiManipulation->hsDamping5;
            m_vDampingSliders[6] = m_uiManipulation->hsDamping6;
            m_vDampingSliders[7] = m_uiManipulation->hsDamping7;
            m_vDampingSliders[8] = m_uiManipulation->hsDamping8;

        // init double spinboxes containers
            m_vShiftDSpinBoxes = QVector<QDoubleSpinBox*>(m_i32ModifiersNumber,NULL);
            m_vShiftDSpinBoxes[0] = m_uiManipulation->dsbSV0;
            m_vShiftDSpinBoxes[1] = m_uiManipulation->dsbSV1;
            m_vShiftDSpinBoxes[2] = m_uiManipulation->dsbSV2;
            m_vShiftDSpinBoxes[3] = m_uiManipulation->dsbSV3;
            m_vShiftDSpinBoxes[4] = m_uiManipulation->dsbSV4;
            m_vShiftDSpinBoxes[5] = m_uiManipulation->dsbSV5;
            m_vShiftDSpinBoxes[6] = m_uiManipulation->dsbSV6;
            m_vShiftDSpinBoxes[7] = m_uiManipulation->dsbSV7;
            m_vShiftDSpinBoxes[8] = m_uiManipulation->dsbSV8;

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
            QObject::connect(m_uiManipulation->pbStop,    SIGNAL(clicked()), this, SLOT(stopBottlePlanification()));
            QObject::connect(m_uiManipulation->pbStopAll, SIGNAL(clicked()), this, SLOT(stopAllBottlesPlanification()));

        //  display
            QObject::connect(m_pWManipulation, SIGNAL(sendBottle(SWBottleContent,int)), this, SLOT(updateBottleContentDisplay(SWBottleContent,int)));
            QObject::connect(m_pWManipulation , SIGNAL(planificationState(int, bool, int, double)), this, SLOT(updateTimesDisplay(int, bool, int, double)));

        // modifiers
        //  listWidget
            QObject::connect(m_uiManipulation->lwManipulation, SIGNAL(currentRowChanged(int)), SLOT(switchDisplayModifiersValues(int)));
        //  update modifiers
            QObject::connect(this, SIGNAL(sendBottleModifiers(QVector<double>, QVector<double>, int)), m_pWManipulation, SLOT(updateModifier(QVector<double>, QVector<double>, int)));
        //  udpate planification params
            QObject::connect(this, SIGNAL(sendPlanificationParams(int, int, int, double, double, QString)),
                             m_pWManipulation, SLOT(updatePlanification(int, int, int, double, double, QString)));;
            QObject::connect(this, SIGNAL(startBottlePlan(int, bool)), m_pWManipulation, SLOT(updateBottleStart(int, bool)));
        //  active click buttons
            for(int ii = 0; ii < m_i32YarpConnectNumber; ++ii)
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

    m_oTimer.start(m_dTimeLoop, this);
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
        for(int ii = 0; ii < m_vDampingSliders.size(); ++ii)
        {
            // update shift and constants arrays
                m_vVDamping[l_i32Index][ii]    = 0.01 * m_vDampingSliders[ii]->value();
                m_vVShift[l_i32Index][ii] = m_vShiftDSpinBoxes[ii]->value();

            // uddate shift line edit display
                m_vDampingDisplayLineEdit[ii]->setText(QString::number(m_vVDamping[l_i32Index][ii], 10, 2));
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

    // set Modifier mode to send
        int l_i32ModifierMode = NO_MODIF;
        if(m_vI32ModePlan[l_i32Index] == MODIFIED)
        {
            if(m_uiManipulation->cbDampingMOD->isChecked() && m_uiManipulation->cbShiftMOD->isChecked())
            {
                l_i32ModifierMode = DAMPING_AND_SHIFT;
            }
            else if(m_uiManipulation->cbDampingMOD->isChecked())
            {
                l_i32ModifierMode = DAMPING;
            }
            else if(m_uiManipulation->cbShiftMOD->isChecked())
            {
                l_i32ModifierMode = SHIFT;
            }
        }
        else if(m_vI32ModePlan[l_i32Index] == RANDOM)
        {
            if(m_uiManipulation->cbDampingRAND->isChecked() && m_uiManipulation->cbShiftRAND->isChecked())
            {
                l_i32ModifierMode = DAMPING_AND_SHIFT;
            }
            else if(m_uiManipulation->cbDampingRAND->isChecked())
            {
                l_i32ModifierMode = DAMPING;
            }
            else if(m_uiManipulation->cbShiftRAND->isChecked())
            {
                l_i32ModifierMode = SHIFT;
            }
        }
        else if(m_vI32ModePlan[l_i32Index] == SEQUENCE)
        {
            l_i32ModifierMode = DEFINED_BY_SEQUENCE;
        }

    // send modifiers parameters
        for(int ii = 0; ii < m_vVDamping.size(); ++ii)
        {
            sendBottleModifiers(m_vVDamping[ii], m_vVShift[ii], ii);
        }
    // send planification parameters
        sendPlanificationParams(l_i32Index, m_vI32ModePlan[l_i32Index], l_i32ModifierMode, m_vDTotalTime[l_i32Index], m_vDBlockTime[l_i32Index], m_vSSequence[l_i32Index]);

    // check timeout and update display
    for(int ii = 0; ii < m_vDTimeOutINBottles.size(); ++ii)
    {
        // if no timeout
        if(m_vDTimeOutINBottles[ii] > m_dTimeLoop)
        {
            m_vDTimeOutINBottles[ii] -= m_dTimeLoop; // display
            m_vSourceLabels[ii]->setText(QString("Active"));
            m_vActiveButtons[ii]->setEnabled(true);
        }
        // if timeout
        else if(m_vDTimeOutINBottles[ii] > 0)
        {
            m_vDTimeOutINBottles[ii] -= m_dTimeLoop;
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

    // update display timers groupBox elements
        m_uiManipulation->lePlanStarted->setText(m_vSPlanStarted[l_i32Index]);
        m_uiManipulation->leTimeRemaining->setText(m_vSTimeRemaining[l_i32Index]);
        m_uiManipulation->leCurrentModifier->setText(m_vSModifiers[l_i32Index]);
}

void SWManipulationInterface::updateBottleContentDisplay(SWBottleContent oBottle, int i32NumBottle)
{
    // set timeout value
        m_vDTimeOutINBottles[i32NumBottle] = 5000;

    // set the text to be displayed
        QString l_sTextToDisplay = QString::fromStdString(swTracking::returnStringValue(oBottle.idLib));

        for(uint ii = 0; ii < oBottle.dValues.size(); ++ii)
        {
            l_sTextToDisplay += QString(" ") + QString::number(oBottle.dValues[ii], 10, 2);
        }

        m_vBottleDisplayLineEdit[i32NumBottle]->setText(l_sTextToDisplay);
}

void SWManipulationInterface::updateTimesDisplay(int i32Index, bool bDo, int i32Modifier, double dRemainingTime)
{
    if(bDo)
    {
        m_vSPlanStarted[i32Index] = QString("Yes");
    }
    else
    {
        m_vSPlanStarted[i32Index] = QString("No");
    }

    m_vSTimeRemaining[i32Index] = QString::number(dRemainingTime, 10, 2);

    if(i32Modifier == NO_MODIF)
    {
        m_vSModifiers[i32Index] = QString("NO_MODIF");
    }
    else if(i32Modifier == DAMPING)
    {
        m_vSModifiers[i32Index] = QString("DAMPING");
    }
    else if(i32Modifier == SHIFT)
    {
        m_vSModifiers[i32Index] = QString("SHIFT");
    }
    else if(i32Modifier == DAMPING_AND_SHIFT)
    {
        m_vSModifiers[i32Index] = QString("BOTH");
    }
}

void SWManipulationInterface::switchDisplayModifiersValues(int i32Index)
{
    // modifiers
        for(int ii = 0; ii < m_vDampingSliders.size(); ++ii)
        {
            m_vDampingSliders[ii]->setValue((int)(100* m_vVDamping[i32Index][ii]));
            m_vShiftDSpinBoxes[ii]->setValue(m_vVShift[i32Index][ii]);
        }

        m_uiManipulation->dsbTotalTime->setValue(m_vDTotalTime[i32Index]);
        m_uiManipulation->dsbTimeBlock->setValue(m_vDBlockTime[i32Index]);
        m_vPlanificationRadioButtons[m_vI32ModePlan[i32Index]]->setChecked(true);
        m_uiManipulation->teSequence->setPlainText(m_vSSequence[i32Index]);
}



void SWManipulationInterface::resetModifiers()
{
    // reset shift / consts
        for(int ii = 0; ii < m_vDampingSliders.size(); ++ii)
        {
            m_vDampingSliders[ii]->setValue(100);
            m_vShiftDSpinBoxes[ii]->setValue(0.0);
        }
    // reset planification
        m_uiManipulation->rbNormals->setChecked(true);
        m_uiManipulation->dsbTotalTime->setValue(100.0);
        m_uiManipulation->dsbTimeBlock->setValue(25.0);
        m_uiManipulation->teSequence->setPlainText(g_sDefaultSequence);
}

void SWManipulationInterface::resetAllModifiers()
{
    resetModifiers();
    m_vVDamping   = QVector<QVector<double> >(m_i32YarpConnectNumber, QVector<double>(m_i32ModifiersNumber,1.0));
    m_vVShift     = QVector<QVector<double> >(m_i32YarpConnectNumber, QVector<double>(m_i32ModifiersNumber,0.0));
    m_vDTotalTime = QVector<double>(m_i32ModifiersNumber, 100.0);
    m_vDBlockTime = QVector<double>(m_i32ModifiersNumber, 25.0);
    m_vI32ModePlan= QVector<int>(m_i32ModifiersNumber, 0);
    m_vSSequence  = QVector<QString>(m_i32ModifiersNumber, g_sDefaultSequence);
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
    for(int ii = 0; ii < m_i32YarpConnectNumber; ++ii)
    {
        emit startBottlePlan(ii, true);
    }
}

void SWManipulationInterface::stopBottlePlanification()
{
    int l_i32Index =  m_uiManipulation->lwManipulation->currentRow();

    emit startBottlePlan(l_i32Index, false);
}

void SWManipulationInterface::stopAllBottlesPlanification()
{
    for(int ii = 0; ii < m_i32YarpConnectNumber; ++ii)
    {
        emit startBottlePlan(ii, false);
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
//        l_oManipulationInterface.resize(QSize(1200,600));
        l_oManipulationInterface.move(50,50);
        l_oManipulationInterface.show();

    return l_oApp.exec();
}
