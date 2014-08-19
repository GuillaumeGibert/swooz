

/**
 * \file SWMorphingWorker.cpp
 * \author LANCE Florian
 * \date 19/11/13
 * \brief definition of SWMorphingWorker
 */

#include "interface/QtWorkers/SWMorphingWorker.h"

#include "moc_SWMorphingWorker.cpp"

SWMorphingWorker::SWMorphingWorker(SWGLOptimalStepNonRigidICP *pGLOSNRICP) : m_pGLOSNRICP(pGLOSNRICP), m_bDoMorphing(false)
{}

SWMorphingWorker::~SWMorphingWorker()
{}

void SWMorphingWorker::startMorphing()
{
    emit startMorphingSignal();

    m_oMutex.lockForWrite();
        m_bDoMorphing = true;
    m_oMutex.unlock();

    bool l_bEscapeLoop = false;
    int l_i32Iteration;    
    double l_dDiff      = DBL_MAX;
    double l_dAlpha     = m_pGLOSNRICP->startAlpha();
    double l_dMinAlpha  = m_pGLOSNRICP->minAlpha();
    double l_dDiffMax   = m_pGLOSNRICP->diffMax();
    double l_dCoeffAlpha= m_pGLOSNRICP->coeffAlpha();

    qDebug() << "StartMorphing : " << l_dAlpha << " " << l_dMinAlpha << " " << l_dDiffMax << " " << l_dCoeffAlpha << endl;

    while(l_dAlpha > l_dMinAlpha) // outer loop
    {
        l_i32Iteration = 1;

        while(l_dDiff > l_dDiffMax)
        {
            // resolve
            l_dDiff = m_pGLOSNRICP->morph(l_dAlpha);                        

            if(l_dDiff < 0 || l_i32Iteration > 20)
            {
                l_bEscapeLoop = true;
                break;
            }

            // refresh display
            emit updateSceneDisplaySignal();

            // check OSNRICP initialization
            if(l_dDiff < 0)
            {
                break;
            }

            qDebug() << "iter " << l_i32Iteration++ << "  diff : " << l_dDiff;

            // wait (necessary to get the events)
            QTime l_oDieTime = QTime::currentTime().addMSecs(5);
            while( QTime::currentTime() < l_oDieTime)
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
            }

            // check if stopMorphing has been sent
            m_oMutex.lockForRead();
            if(!m_bDoMorphing)
            {
                l_bEscapeLoop = true;
                m_oMutex.unlock();
                break;
            }
            m_oMutex.unlock();
        }

        if(l_bEscapeLoop)
        {
            break;
        }

        // update loop values
        l_dAlpha *= l_dCoeffAlpha;
        l_dDiff   = DBL_MAX;
    }

    m_oMutex.lockForWrite();
        m_bDoMorphing = false;                
    m_oMutex.unlock();

    emit endMorphingSignal();
}

void SWMorphingWorker::stopMorphing()
{
    m_oMutex.lockForWrite();
        m_bDoMorphing = false;
    m_oMutex.unlock();
}

void SWMorphingWorker::saveMeshFile(QString sPath)
{
    m_pGLOSNRICP->saveCurrentMeshToObj(sPath);
}
