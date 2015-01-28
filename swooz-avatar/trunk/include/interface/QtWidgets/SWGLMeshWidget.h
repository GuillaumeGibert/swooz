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
* \file SWGLMeshWidget.h
* \brief Defines SWGLMeshWidget
* \author Florian Lance
* \date 11/07/13
*/

#ifndef _SWGLMESHWIDGET_
#define _SWGLMESHWIDGET_

#include "mesh/SWMesh.h"

#include "SWGLWidget.h"



class SWGLMeshWidget : public SWGLWidget
{
    Q_OBJECT

    public:

        /**
        * \brief constructor of SWGLMeshWidget
        * \param [in] context : ...
        * \param [in] parent : ...
        * \param [in] sVertexShaderPath : ...
        * \param [in] sFragmentShaderPath : ...
        */
        SWGLMeshWidget(QGLContext *context, QWidget* parent = 0, const QString &sVertexShaderPath = "", const QString &sFragmentShaderPath = "");

        /**
        * \brief destructor of SWGLMeshWidget
        */
        ~SWGLMeshWidget();

    protected:

        /**
        * \brief Initialize the OpenGl scene
        */
        virtual void initializeGL();

        /**
        * \brief Draw the OpenGl scene
        */
        virtual void paintGL();

        /**
        * \brief Init buffers.
        */
        void initMeshBuffers();


    public slots:

        /**
        * \brief Draw the mesh.
        */
        void drawMesh();

        /**
        * \brief Set the mesh to draw
        * \param [in] pMesh : pointer to the mesh to draw
        * @param [in] bResetCamera : ...
        */
        void setMesh(swMesh::SWMesh *pMesh, bool bResetCamera);

        /**
        * \brief Set the texture for the mesh.
        * \param [in] sTexturePath : path of the texture.
        */
        void setTexture(const QString &sTexturePath);

        /**
        * @brief setTexture
        * @param oTexture
        */
        void setTexture(const QImage &oTexture);

        /**
        * @brief applyTexture
        * @param bApplyTexture
        */
        void applyTexture(const bool bApplyTexture);

        /**
        * @brief SWGLMeshWidget::setMeshLinesRender
        * @param bRenderLines
        */
        void setMeshLinesRender(const bool bRenderLines = true);

    private :

        bool m_bNewMesh;

        QString m_sVertexShaderPath; /**< ... */
        QString m_sFragmentShaderPath; /**< ... */

        bool m_bLinesRender; /**< ... */
        bool m_bApplyTexture; /**< ... */
        bool m_bBindTexture; /**< ... */

        QGLShaderProgram m_oShaderMesh; /**< ... */
        QGLShaderProgram m_oShaderLines; /**< ... */

        QImage m_oTexture;

        QGLBuffer m_vertexBufferMesh; /**< ... */
        QGLBuffer m_indexBufferMesh; /**< ... */
        QGLBuffer m_normalBufferMesh; /**< ... */
        QGLBuffer m_textureBufferMesh; /**< ... */

        GLuint m_textureLocation;
        GLuint m_texHandle;

//        QMatrix4x4 m_oMVPMatrix;	/**< ... */

        swMesh::SWMesh *m_pMesh; /**< ... */
};

#endif
