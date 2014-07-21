#ifndef _SWGLOPTIMALSTEPNONRIGIDICP_
#define _SWGLOPTIMALSTEPNONRIGIDICP_

/**
 * \file SWGLOptimalStepNonRigidICP.h
 * \brief Defines SWGLOptimalStepNonRigidICP
 * \author Florian Lance
 * \date 26/09/13
 */


//#include <QtGui>

#include "SWGLWidget.h"
#include "mesh/SWOptimalStepNonRigidICP.h"
#include "mesh/SWMesh.h"
#include "boost/shared_ptr.hpp"

#include "interface/SWGLUtility.h"


typedef boost::shared_ptr<swMesh::SWMesh> SWMeshPtr; /**< boost shared pointer for SWMesh */
typedef boost::shared_ptr<swMesh::SWOptimalStepNonRigidICP> SWOptimalStepNonRigidICPPtr; /**< boost shared pointer for SWOptimalStepNonRigidICP */


struct SWGLBufferList
{
    QGLBuffer m_vertexBuffer;
    QGLBuffer m_indexBuffer;
    QGLBuffer m_colorBuffer;
    QGLBuffer m_normalBuffer;
    QGLBuffer m_textureBuffer;
    bool m_bUpdate;
};


/**
 * \class SWGLOptimalStepNonRigidICP
 * \brief Sub class of SWGLWidget, can be used to control the OSNRICP morphing and to display data in a widget.
 * \author Florian Lance
 * \date 26/09/13
 */
class SWGLOptimalStepNonRigidICP : public SWGLWidget
{
    Q_OBJECT

    public:

        /**
         * \brief constructor of SWGLOptimalStepNonRigidICP
         * \param [in] context              : opengl context
         * \param [in] parent               : parent widget
         */
        SWGLOptimalStepNonRigidICP::SWGLOptimalStepNonRigidICP(QGLContext *context, QWidget* parent);

        /**
         * \brief destructor of SWOptimalStepNonRigidICP
         */
        ~SWGLOptimalStepNonRigidICP();

        /**
         * \brief Return start alpha (mutex protection)
         * \return start alpha value
         */
        double startAlpha() const;

        /**
         * \brief Return min alpha (mutex protection)
         * \return min alpha value
         */
        double minAlpha() const;

        /**
         * \brief Return diff max (mutex protection)
         * \return diff max value
         */
        double diffMax() const;

        /**
         * \brief Return coeff alpha (mutex protection)
         * \return coeff alpha value
         */
        double coeffAlpha() const;


        /**
         * @brief bufferUpdate
         */
        void bufferUpdate();


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
         * \brief Save the current source mesh to an obj file
         * \param [in] sPath : path of the obj to be saved
         */
        void saveCurrentMeshToObj(const QString &sPath);

        /**
         * \brief Set cloud source display boolean value
         * \param [in] bVal : value
         */
        void setCloudSDisplay(bool bVal);

        /**
         * \brief Set cloud target display boolean value
         * \param [in] bVal : value
         */
        void setCloudTDisplay(bool bVal);

        /**
         * \brief Set mesh source display boolean value
         * \param [in] bVal : value
         */
        void setMeshSDisplay(bool bVal);

        /**
         * \brief Set mesh target display boolean value
         * \param [in] bVal : value
         */
        void setMeshTDisplay(bool bVal);

        /**
         * \brief Set Use landmarks boolean value
         * \param [in] bVal : value
         */
        void setUseLandMarks(bool bVal);

        /**
         * \brief Set vertices normals source display boolean value
         * \param [in] bVal : value
         */
        void setVerticesNormalsSDisplay(bool bVal);

        /**
         * \brief Set vertices normals target display boolean value
         * \param [in] bVal : value
         */
        void setVerticesNormalsTDisplay(bool bVal);

       /**
        * \brief Set triangles normals source display boolean value
        * \param [in] bVal : value
        */
        void setTrianglesNormalsSDisplay(bool bVal);

        /**
         * \brief Set triangles normals target display boolean value
         * \param [in] bVal : value
         */
        void setTrianglesNormalsTDisplay(bool bVal);

        /**
         * \brief Set landmarks display boolean value
         * \param [in] bVal : value
         */
        void setLandMarksDisplay(bool bVal);

        /**
         * \brief Set correspondance lines display boolean value
         * \param [in] bVal : value
         */
        void setCorrDisplay(bool bVal);

        /**
         * \brief Set fill source mesh display boolean value
         * \param [in] bVal : value
         */
        void setFillSDisplay(bool bVal);

        /**
         * \brief Set fill target mesh display boolean value
         * \param [in] bVal : value
         */
        void setFillTDisplay(bool bVal);

        /**
         * \brief Set the source mesh obj path file
         * \param [in] sPathSource : path of the obj file
         */
        void setSourceMesh(const QString &sPathSource);

        /**
         * \brief Set the target mesh obj path file
         * \param [in] sPathTarget : path of the obj file
         */
        void setTargetMesh(const QString &sPathTarget);


        /**
         * \brief Set angle max between correspondance vertices
         * \param [in] dVal : angle value
         */
        void setAngleMax(double dVal);

        /**
         * \brief Set x rotation to the target and apply the transformation
         * \param [in] i32Val : x rotation value of the slider
         */
        void setRotTargetX(int i32Val);

        /**
         * \brief Set y rotation to the target and apply the transformation
         * \param [in] i32Val : y rotation value of the slider
         */
        void setRotTargetY(int i32Val);

        /**
         * \brief Set z rotation to the target and apply the transformation
         * \param [in] i32Val : z rotation value of the slider
         */
        void setRotTargetZ(int i32Val);

        /**
         * \brief Set x translation to the target and apply the transformation
         * \param [in] i32Val : x translation value of the slider
         */
        void setTransTargetX(int i32Val);

        /**
         * \brief Set y translation to the target and apply the transformation
         * \param [in] i32Val : y translation value of the slider
         */
        void setTransTargetY(int i32Val);

        /**
         * \brief Set z translation to the target and apply the transformation
         * \param [in] i32Val : z translation value of the slider
         */
        void setTransTargetZ(int i32Val);

        /**
         * \brief Set scaling coeff to the target and apply the transformation
         * \param [in] i32Val : scalin value of the slider
         */
        void setTargetScaling(int i32Val);

        /**
         * \brief Set start alpha (mutex protection)
         * \param dVal : value
         */
        void setStartAlpha(double dVal);

        /**
         * \brief Set min alpha (mutex protection)
         * \param dVal : value
         */
        void setMinAlpha(double dVal);

        /**
         * \brief Set diff max (mutex protection)
         * \param dVal : value
         */
        void setDiffMax(double dVal);

        /**
         * \brief Set beta (mutex protection)
         * \param dVal : value
         */
        void setBeta(double dVal);

        /**
         * \brief Set gama (mutex protection)
         * \param dVal : value
         */
        void setGama(double dVal);

        /**
         * \brief Set coeff alpha (mutex protection)
         * \param dVal : value
         */
        void setCoeffAlpha(double dVal);

        /**
         * \brief Compute distance weights and correspondances between source and target.
         */
        void computeDistWAndCorr();

        /**
         * \brief Init the current morphing step  (mutex protection)
         *  Update normals, correspondances, weights and copy working data.
         */
        void initResolve();

        /**
         * \brief Do a morphing iteration with the input alpha value
         * \param [in] dAlpha : current alpha
         */
        double morph(cdouble dAlpha);

        /**
         * \brief Reset morphing.
         */
        void resetMorphing();

        /**
         * \brief Reset morphing and init new meshes with the currents paths
         */
        void resetMorphingWithNewMeshes();

        /**
         * \brief Reset morphing and init corr stasm files with the currents paths
         */
        void resetMorphingWithNewCorr();

        /**
         * \brief Refresh widget opengl display
         */
        void refreshDisplay(bool bBufferUpdate = true);

    signals :

        /**
         * @brief sendSourceMeshInfos
         */
        void sendSourceMeshInfos(QString);

        /**
         * @brief sendTargetMeshInfos
         */
        void sendTargetMeshInfos(QString);

    private :

        void initBufferList(SWGLBufferList &oBuffer);

        /**
         * \brief Align source and target using the nose. Apply also a basic scaling to the target if necessary
         * \param [in,out] oSourceMesh : source mesh
         * \param [in,out] oTargetMesh : target mesh
         */
        void alignWithNose(swMesh::SWMesh &oSourceMesh, swMesh::SWMesh &oTargetMesh);

        /**
         * \brief Apply all the transformations (translations,rotations,scaling) to the target.
         * \param [in] bUpdateDisplay : refresh the opengl display
         */
        void transformTarget(cbool bUpdateDisplay = true);

        /**
         * \brief Draw the opengl scene, call all drawing functions.
         */
        void drawScene();

        void drawCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint,
                        QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b);

        void drawSourceCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint, QMatrix4x4 &mvpMatrix);

        void drawMeshLines(QGLShaderProgram &oShader, SWGLBufferList &oBuffers, swMesh::SWMesh &oMesh, QMatrix4x4 &mvpMatrix,
                           cfloat r = -1, cfloat g = -1, cfloat b = -1, cfloat fOpacity = 1.f);

        void drawCorrLines(QGLShaderProgram &oShader, const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget,
                           const std::vector<uint> &vU, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b);

        void drawLandMarksCorr(QGLShaderProgram &oShader, const std::map<uint,uint> mLandMarkCorr, const swCloud::SWCloud &oSource,
                               const swCloud::SWCloud &oTarget, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b);

        void drawLandMarksPoints(QGLShaderProgram &oShader, const std::map<uint,uint> mLandMarkCorr,
                                 const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget, QMatrix4x4 &mvpMatrix);

        void drawMeshVerticesNormals(QGLShaderProgram &oShader, SWGLBufferList &oBuffers, const swMesh::SWMesh &oMesh, QMatrix4x4 &mvpMatrix,
                                     cfloat r = -1, cfloat g = -1, cfloat b = -1);

        void drawMeshTrianglesNormals(QGLShaderProgram &oShader, SWGLBufferList &oBuffers, const swMesh::SWMesh &oMesh, QMatrix4x4 &mvpMatrix,
                                      cfloat r = -1, cfloat g = -1, cfloat b = -1);

        void drawMeshTriangles(QGLShaderProgram &oShader, SWGLBufferList &oBuffers, swMesh::SWMesh &oMesh, QMatrix4x4 &mvpMatrix,
                                  QVector3D &v3DLAmbiant, cfloat fOpacity = 1.f);

        /**
         * @brief getInfoMesh
         * @param oMesh
         * @return
         */
        QString getInfoMesh(const swMesh::SWMesh &oMesh);

    private :

        bool m_bPointsSDisplay;             /**< display the points of the source ? */
        bool m_bPointsTDisplay;             /**< display the points of the target ? */
        bool m_bMeshSDisplay;               /**< display the mesh of the source ? */
        bool m_bMeshTDisplay;               /**< display the mesh of the target ? */
        bool m_bCorrDisplay;                /**< display the correspondence links between source and target vertices ?  */
        bool m_bFillS;                      /**< fill the source mesh */
        bool m_bFillT;                      /**< fill the target mesh */
        bool m_bDisplayLandMarks;           /**< display landmarks ? */
        bool m_bVerticesNormalsSDisplay;    /**< display the vertices normales of the source ? */
        bool m_bTrianglesNormalsSDisplay;   /**< display the triangles normales of the source ? */
        bool m_bVerticesNormalsTDisplay;    /**< display the vertices normales of the target ? */
        bool m_bTrianglesNormalsTDisplay;   /**< display the triangles normales of the target ? */

        bool m_bUseLandMarks;               /**< use landmarks with the morphing ? */

        float m_fXRotTarget;                /**< x rotation target */
        float m_fYRotTarget;                /**< y rotation target */
        float m_fZRotTarget;                /**< z rotation target */
        float m_fXTransTarget;              /**< x translation target */
        float m_fYTransTarget;              /**< y translation target  */
        float m_fZTransTarget;              /**< z translation target  */
        float m_fTargetScaling;             /**< scaling target */

        float m_fDefaultOpacity;            /**< default opengl opacity value */
        float m_fOpacitySourceMeshLines;    /**< opengl opacity value sent to fragment shader for the mesh lines source */
        float m_fOpacityTargetMeshLines;    /**< opengl opacity value sent to fragment shader for the mesh lines target */
        float m_fOpacitySourceMesh;         /**< opengl opacity for the source mesh */
        float m_fOpacityTargetMesh;         /**< opengl opacity fot the target mesh*/

        double m_dStartAlpha;               /**< start alpha */
        double m_dMinAlpha;                 /**< minimum alpha  */
        double m_dDiffMax;                  /**< diff max bewteen two morph, if lower the next alpha value can be used */
        double m_dBeta;                     /**< beta : landmarks  value */
        double m_dGama;                     /**< gama : neighbours value */
        double m_dCoeffAlpha;               /**< alpha coeff to be used */

        std::string m_sPathSourceMesh;      /**< mesh source obj file path */
        std::string m_sPathTargetMesh;      /**< mesh target obj file path */

        QString m_sPathStasmTarget;
        QString m_sPathStasmSource;

        QMatrix4x4 m_oMVPMatrix;            /**< mvp matrix for opengl scene */

        QGLBuffer m_vertexBuffer;           /**< opengl vertex buffer */
        QGLBuffer m_indexBuffer;            /**< opengl index buffer */
        QGLBuffer m_colorBuffer;            /**< opengl color buffer */
        QGLBuffer m_normalBuffer;           /**< opengl normal buffer */
        QGLBuffer m_textureBuffer;           /**< opengl normal buffer */

        SWGLBufferList m_templateCloudBuffer;
        SWGLBufferList m_templateMeshBuffer;
        SWGLBufferList m_templateMeshLinesBuffer;
        SWGLBufferList m_templateVerticesNormalesBuffer;
        SWGLBufferList m_templateTrianglesNormalesBuffer;

        SWGLBufferList m_targetCloudBuffer;
        SWGLBufferList m_targetMeshBuffer;
        SWGLBufferList m_targetMeshLinesBuffer;
        SWGLBufferList m_targetVerticesNormalesBuffer;
        SWGLBufferList m_targetTrianglesNormalesBuffer;

        QGLShaderProgram m_oShaderPoints;   /**< shader program to render points */
        QGLShaderProgram m_oShaderLines;    /**< shader program to render lines */
        QGLShaderProgram m_oShaderTriangles;/**< shader program to render triangles */

        QMutex *m_pParamMutex;              /**< mutex for OSNRICP parameters */

        QReadWriteLock *m_pSourceMeshMutex; /**< mutex for source mesh */
        QReadWriteLock *m_pTargetMeshMutex; /**< mutex for target mesh */
        QReadWriteLock *m_pUMutex;          /**< mutex for OSNRICP U correspondance array */
        QReadWriteLock *m_pWMutex;          /**< mutex for OSNRICP W weight array */

        SWMeshPtr m_pSourceMesh;            /**< source mesh pointer */
        SWMeshPtr m_pTargetMesh;            /**< target mesh pointer */

        SWOptimalStepNonRigidICPPtr m_pOSNRICP; /**< Optimal step non rigid ICP pointer */
};

#endif

