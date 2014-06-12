
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
         */
        void setCloud(swCloud::SWCloud *oCloud);


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

        QMatrix4x4  m_oMVPMatrix;          /**< model view projection matrix */

//        swCloud::SWCloud *m_pCloud;        /**< cloud pointer*/
        swCloud::SWCloud m_oCloud;
        swCloud::SWCloudBBox m_oCloudBBox; /**< bounded box of the cloud */

        bool m_bNewCloud;
};

#endif

