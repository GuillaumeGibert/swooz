
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
         * \brief Draw the cloud.
         */
//        void drawCloud();

        void drawCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint,
                        QMatrix4x4 &mvpMatrix, cfloat r=-1, cfloat g=-1, cfloat b=-1);

        /**
         * \brief Set the current cloud point to display.
         * \param [in] oCloud          : cloud point to display in the scene,
         */
        void setCloud(swCloud::SWCloud *oCloud);

        /**
         * \brief Set the rigid motion and the current cloud point to display.
         * \param [in] oRigidMotion    : the rigid motion to apply on the cloud
         * \param [in] oCloud          : cloud point to display in the scene
         */
        void setCloudAndRigidMotion(const swCloud::SWRigidMotion &oRigidMotion, swCloud::SWCloud *oCloud);

    private :

        QGLShaderProgram m_oShader;     /**< ... */

        QGLBuffer m_vertexBuffer;       /**< ... */
        QGLBuffer m_indexBuffer;        /**< ... */
        QGLBuffer m_colorBuffer;        /**< ... */

        bool m_bInitCamWithCloudPosition; /**< init the camera with the cloud bbox at the first frame */
        bool m_bApplyRigidMotion;   /**< ... */
        float m_fDefaultOpacity;    /**< ... */

        QMatrix4x4  m_oRigidMotion;	/**< ... */
        QMatrix4x4  m_oMVPMatrix;	/**< ... */

        swCloud::SWCloud *m_pCloud; /**< ... */
};

#endif

