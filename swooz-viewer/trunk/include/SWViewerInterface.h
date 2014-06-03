
/**
 * \file SWViewerInterface.h
 * \brief Defines SWViewerInterface
 * \author Florian Lance
 * \date 02/06/14
 */

#ifndef _SWVIEWERINTERFACE_
#define _SWVIEWERINTERFACE_

#include <QMainWindow>
#include <QThread>

//#include "SWViewerWorker.h"

#include "SWUI_Viewer.h"

#include "interface/QtWidgets/SWGLMeshWidget.h"



namespace Ui {
    class SWUI_Viewer;
}

/**
 * \class SWViewerInterface
 * \brief ...
 * \author Florian Lance
 * \date 02/06/14
 */
class SWViewerInterface : public QMainWindow
{
    Q_OBJECT

    public :


        // ############################################# CONSTRUCTORS / DESTRUCTORS

        /**
         * \brief Constructor of SWViewerInterface
         */
        SWViewerInterface();

        /**
         * \brief Destructor of SWViewerInterface
         */
        ~SWViewerInterface();


        // ############################################# METHODS

    public slots:

        void loadMesh();


    signals:


    private :


        // widgets & ui
        Ui::SWUI_Viewer* m_uiViewer;   /**< qt main window */

        SWGLMeshWidget *m_pGLMesh; /**< ... */

        // threads & workers
//        SWViewerWorker  *m_pWViewer;    /**< viewer worker */
        QThread            m_TViwer;    /**< viwer thread */

};


#endif // SWVIEWERINTERFACE_H
