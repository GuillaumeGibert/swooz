
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
 * \file SWGLCloudWidget.h
 * \brief Defines SWGLCloudWidget
 * \author Florian Lance
 * \date 11/07/13
 */

#ifndef _SWGLCLOUDWIDGET_
#define _SWGLCLOUDWIDGET_

#include "SWGLWidget.h"

#include "cloud/SWCloud.h"

class SWGLCloudWidget : public SWGLWidget
{
    Q_OBJECT

    public:

        /**
         * \brief constructor of SWGLCloudWidget
         * \param [in] context              : ...
         * \param [in] parent               : ...
         * \param [in] sVertexShaderPath    : ...
         * \param [in] sFragmentShaderPath  : ...
         */
        SWGLCloudWidget(QGLContext *context, QWidget* parent = 0);

        /**
         * \brief destructor of SWGLCloudWidget
         */
        ~SWGLCloudWidget();

    protected:

        /**
         * \brief Initialize the OpenGl scene
         */
        virtual void initializeGL();

        /**
         * \brief Draw the OpenGl scene
         */
        virtual void paintGL();


    public slots:

        /**
         * \brief Init buffers.
         */
        void initCloudBuffers();

        /**
         * @brief drawDepthRect
         * @param oShader
         * @param mvpMatrix
         * @param fDepth
         */
        void drawDepthRect(QGLShaderProgram &oShader, QMatrix4x4 &mvpMatrix, cfloat fDepth);

        /**
         * @brief setDepthRect
         * @param dDepth
         */
        void setDepthRect(const double dDepth);

        /**
         * @brief drawCloud
         * @param oShader
         * @param fSizePoint
         * @param mvpMatrix
         */
        void drawCloud(QGLShaderProgram &oShader, cfloat fSizePoint, QMatrix4x4 &mvpMatrix);

        /**
         * \brief Set the current cloud point to display.
         * \param [in] oCloud          : cloud point to display in the scene,
         * \param [in] bResetCamera    :
         */
        void setCloud(swCloud::SWCloud *oCloud, bool bResetCamera = false);


    private :

        QGLShaderProgram m_oShader;     /**< ... */

//        QReadWriteLock m_oCloudMutex;

        QGLBuffer m_vertexBufferCloud;       /**< ... */
        QGLBuffer m_indexBufferCloud;        /**< ... */
        QGLBuffer m_colorBufferCloud;        /**< ... */

        bool m_bInitCamWithCloudPosition; /**< init the camera with the cloud bbox at the first frame */
        bool m_bApplyRigidMotion;   /**< ... */
        float m_fDefaultOpacity;    /**< ... */


        float m_fDepthRect;         /**< .. */

//        QMatrix4x4  m_oMVPMatrix;          /**< model view projection matrix */

//        swCloud::SWCloud *m_pCloud;        /**< cloud pointer*/
//        swCloud::SWCloud m_oCloud;

        swCloud::SWCloud *m_pCloud; /**< ... */

        swCloud::SWCloudBBox m_oCloudBBox; /**< bounded box of the cloud */

        bool m_bNewCloud;
};

#endif

