/**
 * \file SWMorphingInterface.h
 * \brief Defines SWMorphingInterface
 * \author Florian Lance
 * \date 12/11/13
 */

#ifndef _SWMORPHINGINTERFACE_
#define _SWMORPHINGINTERFACE_

#include <QMainWindow>
#include <QThread>

#include "QtWorkers/SWMorphingWorker.h"

#include "SWUI_WMeshCorr.h"
#include "SWUI_WParamsDisplay.h"
#include "SWUI_WTargetTransfo.h"

namespace Ui {
    class SWUI_WMeshCorr;
    class SWUI_WParamsDisplay;
    class SWUI_WTargetTransfo;
}

/**
 * \class SWMorphingInterface
 * \brief ...
 * \author Florian Lance
 * \date 12/11/13
 */
class SWMorphingInterface : public QMainWindow
{
    Q_OBJECT

    public :

        // ############################################# CONSTRUCTORS / DESTRUCTORS

        /**
         * \brief Constructor of SWMorphingInterface
         */
        SWMorphingInterface();

        /**
         * \brief Destructor of SWMorphingInterface
         */
        ~SWMorphingInterface();


        // ############################################# METHODS

    public slots:

        /**
         * \brief Lock critical widgets
         */
        void lockInterface();

        /**
         * \brief unlock critical widgets
         */
        void unlockInterface();

        /**
         * \brief Display dilog window for saving current source mesh
         */
        void saveMeshFile();

        /**
         * \brief Reset all parameters with default values
         */
        void resetAllParameters();

    signals:

        /**
         * \brief Signal for starting the morphing in the worker
         */
        void startMorphingSignal();

        /**
         * \brief Signal for stoping the morphing in the worker
         */
        void stopMorphingSignal();

        /**
         * \brief Signal for save the current source mesh in the worker
         */
        void saveMeshFileSignal(QString);

    private :

        int m_i32RotXDefaultValue;
        int m_i32RotYDefaultValue;
        int m_i32RotZDefaultValue;
        int m_i32TransXDefaultValue;
        int m_i32TransYDefaultValue;
        int m_i32TransZDefaultValue;
        int m_i32ScalingDefaultValue;

        double m_dStartAlphaDefaultValue;
        double m_dMinAlphaDefaultValue;
        double m_dDiffMaxDefaultValue;
        double m_dBetaDefaultValue;
        double m_dGamaDefaultValue;
        double m_dCoeffValueDefaultValue;
        double m_dAngleMaxDefaultValue;

        // widgets
        QPushButton *m_pPBStart;
        QPushButton *m_pPBStop;
        QPushButton *m_pPBReset;
        QPushButton *m_pPBSaveMesh;
        QPushButton *m_pPBComputeCorr;
        QPushButton *m_pPBResetParameters;


        Ui::SWUI_WParamsDisplay    *m_pUi_WParamsDispl;
        Ui::SWUI_WMeshCorr         *m_pUi_WMeshCorr;
        Ui::SWUI_WTargetTransfo    *m_pUi_WTTransf;

        SWGLOptimalStepNonRigidICP *m_pGLOSNRICP;


        // threads & workers
        SWMorphingWorker  *m_pWMorphing;    /**< morphing worker */
        QThread            m_TMorphing;		/**< morphing thread */
};


#endif
