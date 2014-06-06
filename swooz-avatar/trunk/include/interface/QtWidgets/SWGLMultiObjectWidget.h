
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


enum GLObjectDisplayMode
{
    GLO_ORIGINAL_COLOR,GLO_UNI_COLOR,GLO_TEXTURE
};

struct SWGLObjectParameters
{        
    bool m_bCloud;

    bool m_bVisible;    
    bool m_bDisplayLines;

    GLObjectDisplayMode displayMode;

    double m_dScaling;

    QVector3D m_vUnicolor;
    QVector3D m_vTranslation;
    QVector3D m_vRotation;

    QString m_sTexturePath;

};

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

        /**
         * @brief setCloudParameters
         * @param ui32Index
         * @param oParams
         */
        void setCloudParameters(cuint ui32Index, const SWGLObjectParameters &oParams);

        /**
         * @brief setMeshParameters
         * @param ui32Index
         * @param oParams
         */
        void setMeshParameters(cuint ui32Index, const SWGLObjectParameters &oParams);

        /**
         * @brief cloudParameters
         * @param ui32Index
         * @param oParams
         */
        void cloudParameters(cuint ui32Index,SWGLObjectParameters &oParams);

        /**
         * @brief meshParameters
         * @param ui32Index
         * @param oParams
         */
        void meshParameters(cuint ui32Index,SWGLObjectParameters &oParams);

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

        void drawClouds();

        void drawMeshes();

    private :

        QGLShaderProgram m_oShaderCloud; /**< ... */
        QGLShaderProgram m_oShaderMesh;  /**< ... */

        QGLBuffer m_vertexBuffer;   /**< ... */
        QGLBuffer m_indexBuffer;    /**< ... */
        QGLBuffer m_normalBuffer;   /**< ... */
        QGLBuffer m_textureBuffer;  /**< ... */
        QGLBuffer m_colorBuffer;    /**< ... */

        QMatrix4x4  m_oMVPMatrix;	/**< ... */

        QList<SWCloudPtr> m_vClouds; /**< ... */
        QList<SWMeshPtr> m_vMeshes;  /**< ... */
        QList<SWGLObjectParameters> m_vCloudsParameters; /**< ... */
        QList<SWGLObjectParameters> m_vMeshesParameters; /**< ... */

//        QList<QImage> m_vMeshesTextures; /**< ... */
//        QList<bool> m_vBMeshLinesRender; /**< ... */
//        QList<bool> m_vBMeshApplyTextures; /**< ... */

        //        bool m_bInitCamWithCloudPosition;
        //        bool m_bBindTexture;            /**< ... */
        //        GLuint m_textureLocation;
        //        GLuint m_texHandle;

        QReadWriteLock m_oParamMutex; /**< ... */

};


#endif // SWGLMultiObjectWidget_H
