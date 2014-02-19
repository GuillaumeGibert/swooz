

/**
 * \file SWMorphingWorker.h
 * \brief Defines SWMorphingWorker
 * \author Florian Lance
 * \date 19/11/13
 */

#ifndef _SWMORPHINGWORKER_
#define _SWMORPHINGWORKER_


#include <QObject>
#include <QReadWriteLock>

#include "interface/QtWidgets/SWGLOptimalStepNonRigidICP.h"

/**
 * \class SWMorphingWorker
 * \brief  ...
 * \author Florian Lance
 * \date 19/11/13
 */
class SWMorphingWorker : public QObject
{
    Q_OBJECT

    public :

        /**
         * \brief constructor of SWMorphingWorker
         * \param [in] pGLOSNRICP : ...
         */
        SWMorphingWorker(SWGLOptimalStepNonRigidICP *pGLOSNRICP);

        /**
         * \brief destructor of SWMorphingWorker
         */
        ~SWMorphingWorker();


    public slots:

        /**
         * \brief ...
         */
        void startMorphing();

        /**
         * \brief ...
         */
        void stopMorphing();

        /**
         * \brief ...
         */
        void saveMeshFile(QString sPath);

    signals:

        /**
         * \brief Signal to call updateGL in the morphing 3D scene
         */
        void updateSceneDisplaySignal();

        /**
         * \brief Signal for indicating the beginning of a morphing
         */
        void startMorphingSignal();

        /**
         * \brief Signal for indicating the end of the current morphing
         */
        void endMorphingSignal();


    private :

        bool m_bDoMorphing;                       /**< ... */

        QReadWriteLock m_oMutex;                  /**< ... */
        SWGLOptimalStepNonRigidICP *m_pGLOSNRICP; /**< ... */
};


#endif
