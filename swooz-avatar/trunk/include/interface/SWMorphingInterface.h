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

#include "SWUI_Morphing.h"

namespace Ui {
    class SWUI_Morphing;
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
        SWMorphingInterface(QApplication *parent);

        /**
         * \brief Destructor of SWMorphingInterface
         */
        ~SWMorphingInterface();


        // ############################################# METHODS

    public slots:

        /**
         * @brief openOnlineDocumentation
         */
        void openOnlineDocumentation();

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
         * @brief setTemplateMeshPath
         */
        void setTemplateMeshPath();

        /**
         * @brief setTargetMeshPath
         */
        void setTargetMeshPath();

        /**
         * \brief Reset all parameters with default values
         */
        void resetAllParameters();

        /**
         * @brief enableGLFullScreen
         */
        void enableGLFullScreen();

        /**
         * @brief disableGLFullScreen
         */
        void disableGLFullScreen();

        /**
         * @brief setLandmarksManually
         */
        void setLandmarksManually();

        /**
         * @brief lockInterfaceForLandmarksSetting
         */
        void lockInterfaceForLandmarksSetting();

        /**
         * @brief unlockInterfaceForLandmarksSetting
         */
        void unlockInterfaceForLandmarksSetting();

        /**
         * @brief landmarksSetManuallyEscape
         */
        void landmarksSetManuallyEscape();

        /**
         * @brief setLandmarks3DDisplayTemplate
         */
        void setLandmarks3DDisplayTemplate();

        /**
         * @brief setLandmarks3DDisplayTarget
         */
        void setLandmarks3DDisplayTarget();

        /**
         * @brief landmarksSetManuallyNextPoint
         */
        void landmarksSetManuallyNextPoint();

        /**
         * @brief eraseManuallyLandmarks
         */
        void eraseManuallyLandmarks();

        /**
         * @brief setLandmarksUsage
         */
        void setLandmarksUsage();

        /**
         * @brief setTargetTexture
         */
        void setTargetTexture();

        /**
         * @brief openAboutWindow
         */
        void openAboutWindow();


    signals:

//        /**
//         * \brief Signal for starting the morphing in the worker
//         */
//        void startMorphingSignal();

//        /**
//         * \brief Signal for stoping the morphing in the worker
//         */
//        void stopMorphingSignal();

        /**
         * \brief Signal for save the current source mesh in the worker
         */
        void saveMeshFileSignal(QString);

        void sendInfoTemplateClickedPoints(QString);

        void sendInfoTargetClickedPoints(QString);

        void updateTargetTexture(QString);

    private :


        QString m_absolutePath;         /**< absolute path used for init dialog boxes */

        bool m_isTemplateDefined;       /**< is the template mesh defined ? */
        bool m_isTargetDefined;         /**< ... */
        bool m_isGLFullScreen;          /**< ... */

        bool m_bSetLandmarksManually;
        bool m_bSetLandmarksTemplate;


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
//        QPushButton *m_pPBStart;
//        QPushButton *m_pPBStop;
//        QPushButton *m_pPBReset;
//        QPushButton *m_pPBSaveMesh;
//        QPushButton *m_pPBComputeCorr;
//        QPushButton *m_pPBResetParameters;


//        Ui::SWUI_WParamsDisplay    *m_pUi_WParamsDispl;
//        Ui::SWUI_WMeshCorr         *m_pUi_WMeshCorr;
//        Ui::SWUI_WTargetTransfo    *m_pUi_WTTransf;


        // widgets & ui
        Ui::SWUI_Morphing* m_uiMorphing;   /**< qt main window */


        SWGLOptimalStepNonRigidICP *m_pGLOSNRICP;


        // threads & workers
        SWMorphingWorker  *m_pWMorphing;    /**< morphing worker */
        QThread            m_TMorphing;		/**< morphing thread */

        QWidget *m_pGLContainer;

        QString m_templateMeshClickedPointsInfo;
        QString m_targetMeshClickedPointsInfo;

        QString m_pathTargetTexture;


//        double *test;
};


#endif
