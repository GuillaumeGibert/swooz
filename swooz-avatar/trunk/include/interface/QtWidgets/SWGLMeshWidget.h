
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
         * \param [in] context              : ...
         * \param [in] parent               : ...
         * \param [in] sVertexShaderPath    : ...
         * \param [in] sFragmentShaderPath  : ...
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
         * \param [in] oMesh    : mesh to draw
         */
        void setMesh(swMesh::SWMesh &oMesh);

        /**
         * \brief Set the texture for the mesh.
         * \param [in] sTexturePath : path of the texture.
         */
        void setTexture(const QString &sTexturePath);

    private :

        QString m_sVertexShaderPath;    /**< ... */
        QString m_sFragmentShaderPath;  /**< ... */

        QGLShaderProgram m_oShaderMesh;         /**< ... */
        QGLShaderProgram m_oShaderLines;    /**< ... */

        QGLBuffer m_vertexBuffer;   /**< ... */
        QGLBuffer m_indexBuffer;    /**< ... */
        QGLBuffer m_normalBuffer;   /**< ... */
        QGLBuffer m_textureBuffer;  /**< ... */

        GLuint m_textureLocation;

        QMatrix4x4  m_oMVPMatrix;	/**< ... */

        swMesh::SWMesh *m_pMesh;            /**< ... */
};

#endif

