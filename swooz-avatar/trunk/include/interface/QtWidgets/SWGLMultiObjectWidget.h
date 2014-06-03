
/**
 * \file SWGLMultiObjectWidget.h
 * \brief Defines SWGLMultiObjectWidget class
 *
 *  ...
 *
 * \author Florian Lance
 * \date 03/06/14
 */



#ifndef SWGLMultiObjectWidget_H
#define SWGLMultiObjectWidget_H


#include "mesh/SWMesh.h"

#include "SWGLWidget.h"

#include <QList>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<swCloud::SWCloud> SWCloudPtr;	/**< boost shared pointer for SWCloud */
typedef boost::shared_ptr<swMesh::SWMesh> SWMeshPtr;	/**< boost shared pointer for SWMesh */

class SWGLMultiObjectWidget : public SWGLWidget
{
    Q_OBJECT

    public:

        /**
         * \brief constructor of SWGLMultiObjectWidget
         * \param [in] context              : ...
         * \param [in] parent               : ...
         */
        SWGLMultiObjectWidget(QGLContext *context, QWidget* parent = 0);

        /**
         * \brief destructor of SWGLMultiObjectWidget
         */
        ~SWGLMultiObjectWidget();

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
        void initBuffers();


    public slots:

        /**
         * @brief addCloud
         * @param sPathCloud
         */
        void addCloud(const QString &sPathCloud);

        /**
         * @brief addMesh
         * @param sPathMesh
         */
        void addMesh(const QString &sPathMesh);

        /**
         * @brief drawScene
         */
        void drawScene();

        /**
         * @brief removeCloud
         * @param ui32Index
         */
        void removeCloud(cuint ui32Index);

        /**
         * @brief removeMesh
         * @param ui32Index
         */
        void removeMesh(cuint ui32Index);

        /**
         * @brief setTexture
         * @param ui32Index
         * @param sTexturePath
         */
        void setTexture(cuint ui32Index, const QString &sTexturePath);

        /**
         * @brief applyTexture
         * @param ui32Index
         * @param bApplyTexture
         */
        void applyTexture(cuint ui32Index, const bool bApplyTexture);

        /**
         * @brief setMeshLinesRender
         * @param ui32Index
         * @param bRenderLines
         */
        void setMeshLinesRender(cuint ui32Index, const bool bRenderLines);

    private :

        QGLShaderProgram m_oShaderCloud; /**< ... */
        QGLShaderProgram m_oShaderMesh;  /**< ... */

//        QImage m_oTexture;

        QGLBuffer m_vertexBuffer;   /**< ... */
        QGLBuffer m_indexBuffer;    /**< ... */
        QGLBuffer m_normalBuffer;   /**< ... */
        QGLBuffer m_textureBuffer;  /**< ... */

        QMatrix4x4  m_oMVPMatrix;	/**< ... */

        QList<SWCloudPtr> m_vClouds; /**< ... */

        QList<SWMeshPtr> m_vMeshes;  /**< ... */
        QList<QImage> m_vMeshesTextures; /**< ... */
        QList<bool> m_vBMeshLinesRender; /**< ... */
        QList<bool> m_vBMeshApplyTextures; /**< ... */

        //        bool m_bInitCamWithCloudPosition;
        //        bool m_bBindTexture;            /**< ... */
        //        GLuint m_textureLocation;
        //        GLuint m_texHandle;

};


#endif // SWGLMultiObjectWidget_H
