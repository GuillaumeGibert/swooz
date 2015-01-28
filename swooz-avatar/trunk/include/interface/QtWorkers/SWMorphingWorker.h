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
