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
 * \file SWGLMultiObjectWidget.cpp
 * \brief Defines SWGLMultiObjectWidget
 * \author Florian Lance
 * \date 03/06/14
 */

#include "interface/QtWidgets/SWGLMultiObjectWidget.h"
#include <iostream>

#include "moc_SWGLMultiObjectWidget.cpp"

#include "interface/SWGLUtility.h"

#include <QGLFunctions>

SWGLMultiObjectWidget::SWGLMultiObjectWidget(QGLContext *context, QWidget* parent) :
    SWGLWidget(context, parent)
{
    m_sendAnimations = true;
}

SWGLMultiObjectWidget::~SWGLMultiObjectWidget()
{}

void SWGLMultiObjectWidget::setCloudParameters(cuint ui32Index, const SWGLObjectParameters &oParams)
{
    m_vCloudsParameters[ui32Index]->m_parametersMutex.lockForWrite();
        m_vCloudsParameters[ui32Index]->m_bCloud = oParams.m_bCloud;
        m_vCloudsParameters[ui32Index]->m_bVisible = oParams.m_bVisible;
        m_vCloudsParameters[ui32Index]->m_bDisplayLines = oParams.m_bDisplayLines;
        m_vCloudsParameters[ui32Index]->displayMode = oParams.displayMode;
        m_vCloudsParameters[ui32Index]->m_dScaling = oParams.m_dScaling;
        m_vCloudsParameters[ui32Index]->m_vUnicolor = oParams.m_vUnicolor;
        m_vCloudsParameters[ui32Index]->m_vTranslation = oParams.m_vTranslation;
        m_vCloudsParameters[ui32Index]->m_vRotation = oParams.m_vRotation;
        m_vCloudsParameters[ui32Index]->m_sTexturePath = oParams.m_sTexturePath;
    m_vCloudsParameters[ui32Index]->m_parametersMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::setMeshParameters(cuint ui32Index, const SWGLObjectParameters &oParams)
{
    m_vMeshesParameters[ui32Index]->m_parametersMutex.lockForWrite();
        m_vMeshesParameters[ui32Index]->m_bCloud = oParams.m_bCloud;
        m_vMeshesParameters[ui32Index]->m_bVisible = oParams.m_bVisible;
        m_vMeshesParameters[ui32Index]->m_bDisplayLines = oParams.m_bDisplayLines;
        m_vMeshesParameters[ui32Index]->displayMode = oParams.displayMode;
        m_vMeshesParameters[ui32Index]->m_dScaling = oParams.m_dScaling;
        m_vMeshesParameters[ui32Index]->m_vUnicolor = oParams.m_vUnicolor;
        m_vMeshesParameters[ui32Index]->m_vTranslation = oParams.m_vTranslation;
        m_vMeshesParameters[ui32Index]->m_vRotation = oParams.m_vRotation;
        m_vMeshesParameters[ui32Index]->m_vSourceLight = oParams.m_vSourceLight;
        m_vMeshesParameters[ui32Index]->m_vAmbiantLight = oParams.m_vAmbiantLight;
        m_vMeshesParameters[ui32Index]->m_vDiffusLight = oParams.m_vDiffusLight;
        m_vMeshesParameters[ui32Index]->m_vSpecularLight = oParams.m_vSpecularLight;
        m_vMeshesParameters[ui32Index]->m_dAmbiantK = oParams.m_dAmbiantK;
        m_vMeshesParameters[ui32Index]->m_dDiffusK = oParams.m_dDiffusK;
        m_vMeshesParameters[ui32Index]->m_dSpecularK = oParams.m_dSpecularK;
        m_vMeshesParameters[ui32Index]->m_dSpecularP = oParams.m_dSpecularP;

        if(m_vMeshesParameters[ui32Index]->m_sTexturePath != oParams.m_sTexturePath)
        {
            m_vMeshesParameters[ui32Index]->m_sTexturePath = oParams.m_sTexturePath;
            m_vMeshesParameters[ui32Index]->m_oTexture = QImage(m_vMeshesParameters[ui32Index]->m_sTexturePath);
            m_vMeshesParameters[ui32Index]->m_textureLocation = bindTexture(m_vMeshesParameters[ui32Index]->m_oTexture);
        }

    m_vMeshesParameters[ui32Index]->m_parametersMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::cloudParameters(cuint ui32Index, SWGLObjectParameters &oParams)
{
    m_vCloudsParameters[ui32Index]->m_parametersMutex.lockForRead();
        oParams.m_bCloud        = m_vCloudsParameters[ui32Index]->m_bCloud;
        oParams.m_bVisible      = m_vCloudsParameters[ui32Index]->m_bVisible;
        oParams.m_bDisplayLines = m_vCloudsParameters[ui32Index]->m_bDisplayLines;
        oParams.displayMode     = m_vCloudsParameters[ui32Index]->displayMode;
        oParams.m_dScaling      = m_vCloudsParameters[ui32Index]->m_dScaling;
        oParams.m_vUnicolor     = m_vCloudsParameters[ui32Index]->m_vUnicolor;
        oParams.m_vTranslation  = m_vCloudsParameters[ui32Index]->m_vTranslation;
        oParams.m_vRotation     = m_vCloudsParameters[ui32Index]->m_vRotation;
        oParams.m_sTexturePath  = m_vCloudsParameters[ui32Index]->m_sTexturePath;
    m_vCloudsParameters[ui32Index]->m_parametersMutex.unlock();
}

void SWGLMultiObjectWidget::meshParameters(cuint ui32Index, SWGLObjectParameters &oParams)
{
    m_vMeshesParameters[ui32Index]->m_parametersMutex.lockForRead();
        oParams.m_bCloud        = m_vMeshesParameters[ui32Index]->m_bCloud;
        oParams.m_bVisible      = m_vMeshesParameters[ui32Index]->m_bVisible;
        oParams.m_bDisplayLines = m_vMeshesParameters[ui32Index]->m_bDisplayLines;
        oParams.displayMode     = m_vMeshesParameters[ui32Index]->displayMode;
        oParams.m_dScaling      = m_vMeshesParameters[ui32Index]->m_dScaling;
        oParams.m_vUnicolor     = m_vMeshesParameters[ui32Index]->m_vUnicolor;
        oParams.m_vTranslation  = m_vMeshesParameters[ui32Index]->m_vTranslation;
        oParams.m_vRotation     = m_vMeshesParameters[ui32Index]->m_vRotation;
        oParams.m_sTexturePath  = m_vMeshesParameters[ui32Index]->m_sTexturePath;
        oParams.m_vSourceLight  = m_vMeshesParameters[ui32Index]->m_vSourceLight;        
        oParams.m_vAmbiantLight =m_vMeshesParameters[ui32Index]->m_vAmbiantLight;
        oParams.m_vDiffusLight = m_vMeshesParameters[ui32Index]->m_vDiffusLight;
        oParams.m_vSpecularLight = m_vMeshesParameters[ui32Index]->m_vSpecularLight;
        oParams.m_dAmbiantK = m_vMeshesParameters[ui32Index]->m_dAmbiantK;
        oParams.m_dDiffusK = m_vMeshesParameters[ui32Index]->m_dDiffusK;
        oParams.m_dSpecularK = m_vMeshesParameters[ui32Index]->m_dSpecularK;
        oParams.m_dSpecularP = m_vMeshesParameters[ui32Index]->m_dSpecularP;
    m_vMeshesParameters[ui32Index]->m_parametersMutex.unlock();
}

void SWGLMultiObjectWidget::initializeGL()
{
    // set perspective
        m_rZNear = 0.01;
        m_rZFar  = 10000.0;
        m_rFOV   = 40.0;

    // set background
        qglClearColor(QColor(49, 53, 70));

    initShaders("../data/shaders/cloudViewer.vert", "../data/shaders/cloudViewer.frag", m_oShaderCloud);
    initShaders("../data/shaders/meshViewer.vert",  "../data/shaders/meshViewer.frag",  m_oShaderMesh);
    initShaders("../data/shaders/cubeMap.vert",  "../data/shaders/cubeMap.frag",  m_oShaderCubeMap);


    initTextures();


    initVertexBuffer(m_vertexBufferCubeMap);
    initVertexBuffer(m_textureBufferCubeMap);
    initIndexBuffer(m_indexBufferCubeMap);

    // enable depth buffer
        glEnable(GL_DEPTH_TEST);
}

void SWGLMultiObjectWidget::paintGL()
{
    // set the size point
        glPointSize(m_glFSizePoint);

    // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // calculate model view transformation
        QMatrix4x4 l_oViewMatrix;
        l_oViewMatrix.setToIdentity();

    // set camera vue
        l_oViewMatrix.lookAt( m_pCamera->eyePosition(), m_pCamera->lookAt(), m_pCamera->up());

    // comput MVP matrix
        QMatrix4x4 l_oModelMatrix;
        l_oModelMatrix.setToIdentity();
        m_oMVPMatrix = l_oModelMatrix  * m_oProjectionMatrix * l_oViewMatrix;

        drawScene();
}

void SWGLMultiObjectWidget::addCloud(const QString &sPathCloud)
{    
    makeCurrent();

    SWCloudPtr l_pCloud = SWCloudPtr(new swCloud::SWCloud());
    l_pCloud->loadObj(sPathCloud.toUtf8().constData());

    swCloud::SWCloudBBox l_oBBox = l_pCloud->bBox();
    if(l_oBBox.diagLength() > 100.f)
    {
        (*l_pCloud) *= 0.001f;
    }

    // parameters
        SWGLObjectParametersPtr l_pCloudParam = SWGLObjectParametersPtr(new SWGLObjectParameters);
        l_pCloudParam->m_bCloud = true;
        l_pCloudParam->m_bVisible = true;
        l_pCloudParam->m_bDisplayLines   = false;
        l_pCloudParam->displayMode = GLO_ORIGINAL_COLOR;
        l_pCloudParam->m_dScaling = 1.;
        l_pCloudParam->m_vRotation = QVector3D(0.,0.,0.);
        l_pCloudParam->m_vRotation = QVector3D(0.,0.,0.);
        l_pCloudParam->m_sTexturePath = QString("...");
        l_pCloudParam->m_vUnicolor = QVector3D(255.,0.,0.);        
        // animation
        l_pCloudParam->m_animationStarted = false;
        l_pCloudParam->m_animationOffsetsX = QVector<float>(l_pCloud->size(),0.f);
        l_pCloudParam->m_animationOffsetsY = QVector<float>(l_pCloud->size(),0.f);
        l_pCloudParam->m_animationOffsetsZ = QVector<float>(l_pCloud->size(),0.f);

    // infos
        SWGLObjectInfos l_cloudInfos;
        l_cloudInfos.m_points = l_pCloud->size();
        l_cloudInfos.m_triangles = 0;

    // init buffers
        QGLBufferPtr l_indexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_vertexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_colorBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_normalBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_textureBuffer = QGLBufferPtr(new QGLBuffer());
        initIndexBuffer(*l_indexBuffer);
        initVertexBuffer(*l_vertexBuffer);
        initVertexBuffer(*l_normalBuffer);
        initVertexBuffer(*l_textureBuffer);
        initVertexBuffer(*l_colorBuffer);

    // set usage pattern
        l_indexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_normalBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_textureBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_colorBuffer->setUsagePattern(QGLBuffer::StaticDraw);

    m_pListCloudsMutex.lockForWrite();
        m_vClouds.push_back(l_pCloud);
        m_vCloudsParameters.push_back(l_pCloudParam);
        m_cloudsInfos.push_back(l_cloudInfos);
        m_vCloudsVertexBuffer.push_back(l_vertexBuffer);
        m_vCloudsIndexBuffer.push_back(l_indexBuffer);
        m_vCloudsNormalBuffer.push_back(l_normalBuffer);
        m_vCloudsTextureBuffer.push_back(l_textureBuffer);
        m_vCloudsColorBuffer.push_back(l_colorBuffer);
        m_vCloudsBufferToUpdate.push_back(true);
    m_pListCloudsMutex.unlock();

    setCameraItem(true, m_vClouds.size()-1);
}

void SWGLMultiObjectWidget::addMesh(const QString &sPathMesh)
{
    makeCurrent();

    SWMeshPtr l_pMesh = SWMeshPtr(new swMesh::SWMesh(sPathMesh.toUtf8().constData()));

    swCloud::SWCloudBBox l_oBBox = l_pMesh->cloud()->bBox();
    if(l_oBBox.diagLength() > 100.f)
    {
        (*l_pMesh->cloud()) *= 0.001f;
    }

    // parameters
        SWGLObjectParametersPtr l_pMeshesParam = SWGLObjectParametersPtr(new SWGLObjectParameters);

        // compute light position
        std::vector<float> l_v3FMeanPoint = l_pMesh->cloud()->meanPoint();
        l_pMeshesParam->m_vSourceLight = QVector3D(l_v3FMeanPoint[0], l_v3FMeanPoint[1], l_v3FMeanPoint[2] - 1.0);

        l_pMeshesParam->m_bCloud = false;
        l_pMeshesParam->m_bVisible = true;
        l_pMeshesParam->m_bDisplayLines   = false;
        l_pMeshesParam->displayMode = GLO_ORIGINAL_COLOR;
        l_pMeshesParam->m_dScaling = 1.;
        l_pMeshesParam->m_vRotation = QVector3D(0.,0.,0.);
        l_pMeshesParam->m_vRotation = QVector3D(0.,0.,0.);
        l_pMeshesParam->m_sTexturePath = QString("...");
        l_pMeshesParam->m_vUnicolor = QVector3D(255.,0.,0.);
        l_pMeshesParam->m_vAmbiantLight  = QVector3D(0.3,0.3,0.3);
        l_pMeshesParam->m_vDiffusLight   = QVector3D(1.0,1.0,1.0);
        l_pMeshesParam->m_vSpecularLight = QVector3D(0.5,0.50,0.50);
        l_pMeshesParam->m_dAmbiantK = 1.;
        l_pMeshesParam->m_dDiffusK = 0.5;
        l_pMeshesParam->m_dSpecularK = 1.;
        l_pMeshesParam->m_dSpecularP = 10.;
        // animation
        l_pMeshesParam->m_animationStarted = false;
        l_pMeshesParam->m_animationOffsetsX = QVector<float>(l_pMesh->pointsNumber(),0.f);
        l_pMeshesParam->m_animationOffsetsY = QVector<float>(l_pMesh->pointsNumber(),0.f);
        l_pMeshesParam->m_animationOffsetsZ = QVector<float>(l_pMesh->pointsNumber(),0.f);

    // infos
        SWGLObjectInfos l_meshesInfos;
        l_meshesInfos.m_points = l_pMesh->pointsNumber();
        l_meshesInfos.m_triangles = l_pMesh->trianglesNumber();

    // init buffers
        QGLBufferPtr l_indexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_vertexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_colorBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_normalBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_textureBuffer = QGLBufferPtr(new QGLBuffer());
        initIndexBuffer(*l_indexBuffer);
        initVertexBuffer(*l_vertexBuffer);
        initVertexBuffer(*l_normalBuffer);
        initVertexBuffer(*l_textureBuffer);
        initVertexBuffer(*l_colorBuffer);

    // set usage pattern
        l_indexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_normalBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_textureBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_colorBuffer->setUsagePattern(QGLBuffer::StaticDraw);

    m_pListMeshesMutex.lockForWrite();
        m_vMeshes.push_back(l_pMesh);       
        m_vMeshesParameters.push_back(l_pMeshesParam);
        m_meshesInfos.push_back(l_meshesInfos);
        m_vMeshesVertexBuffer.push_back(l_vertexBuffer);
        m_vMeshesIndexBuffer.push_back(l_indexBuffer);
        m_vMeshesNormalBuffer.push_back(l_normalBuffer);
        m_vMeshesTextureBuffer.push_back(l_textureBuffer);
        m_vMeshesColorBuffer.push_back(l_colorBuffer);
        m_vMeshesBufferToUpdate.push_back(true);
    m_pListMeshesMutex.unlock();


    setCameraItem(true, m_vMeshes.size()-1);
}

void SWGLMultiObjectWidget::removeCloud(cuint ui32Index)
{
    makeCurrent();

    m_pListCloudsMutex.lockForWrite();

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    if(ui32Index < static_cast<uint>(m_vClouds.size()))
    {        
        float l_aFNULL[1];
        uint32 l_aUI32NUL[1];
        allocateBuffer(*m_vCloudsVertexBuffer[ui32Index],  l_aFNULL, 1 * sizeof(float) );
        allocateBuffer(*m_vCloudsIndexBuffer[ui32Index],  l_aFNULL, 1 * sizeof(float) );
        allocateBuffer(*m_vCloudsTextureBuffer[ui32Index],  l_aFNULL, 1 * sizeof(float) );
        allocateBuffer(*m_vCloudsColorBuffer[ui32Index],  l_aFNULL, 1 * sizeof(float) );
        allocateBuffer(*m_vCloudsColorBuffer[ui32Index],  l_aUI32NUL, 1 * sizeof(GLuint) );

        m_vBuffersToDelete.push_back(m_vCloudsVertexBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsIndexBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsTextureBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsNormalBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsColorBuffer[ui32Index]);

        m_vCloudsVertexBuffer.removeAt(ui32Index);
        m_vCloudsNormalBuffer.removeAt(ui32Index);
        m_vCloudsIndexBuffer.removeAt(ui32Index);
        m_vCloudsTextureBuffer.removeAt(ui32Index);
        m_vCloudsColorBuffer.removeAt(ui32Index);
        m_vCloudsBufferToUpdate.removeAt(ui32Index);

        m_vClouds.removeAt(ui32Index);
        m_vCloudsParameters.removeAt(ui32Index);
        m_cloudsInfos.removeAt(ui32Index);

        m_pListCloudsMutex.unlock();
    }
    else
    {
        m_pListCloudsMutex.unlock();
        std::cerr << "-ERROR : bad index removeCloud " << std::endl;
        return;
    }

    updateGL();
}

void SWGLMultiObjectWidget::removeMesh(cuint ui32Index)
{    
    makeCurrent();

    m_pListMeshesMutex.lockForWrite();

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    if(ui32Index < static_cast<uint>(m_vMeshes.size()))
    {
        float l_aFNULL[1];
        uint32 l_aUI32NUL[1];
        allocateBuffer(*m_vMeshesVertexBuffer[ui32Index],  l_aFNULL, 1 * sizeof(float) );
        allocateBuffer(*m_vMeshesTextureBuffer[ui32Index],  l_aFNULL, 1 * sizeof(float) );
        allocateBuffer(*m_vMeshesColorBuffer[ui32Index],  l_aFNULL, 1* sizeof(float) );
        allocateBuffer(*m_vMeshesNormalBuffer[ui32Index],  l_aFNULL, 1* sizeof(float) );
        allocateBuffer(*m_vMeshesIndexBuffer[ui32Index],  l_aUI32NUL, 1 * sizeof(GLuint) );

        m_vBuffersToDelete.push_back(m_vMeshesVertexBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesTextureBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesColorBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesNormalBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesIndexBuffer[ui32Index]);

        m_vMeshesVertexBuffer.removeAt(ui32Index);
        m_vMeshesNormalBuffer.removeAt(ui32Index);
        m_vMeshesIndexBuffer.removeAt(ui32Index);
        m_vMeshesTextureBuffer.removeAt(ui32Index);
        m_vMeshesColorBuffer.removeAt(ui32Index);
        m_vMeshesBufferToUpdate.removeAt(ui32Index);

        m_vMeshes.removeAt(ui32Index);
        m_vMeshesParameters.removeAt(ui32Index);
        m_meshesInfos.removeAt(ui32Index);

        m_pListMeshesMutex.unlock();
    }
    else
    {
        m_pListMeshesMutex.unlock();
        std::cerr << "-ERROR : bad index removeMesh " << std::endl;
        return;
    }

    updateGL();
}

void SWGLMultiObjectWidget::setCameraItem(cbool bIsCloudItem, cint i32IndexItem)
{
    std::vector<float> l_v3FMiddlePoint(3,0.f);
    float l_distObject = 0.f;
    QVector3D l_vTranslation;


    swCloud::SWCloudBBox l_bbox;

    if(bIsCloudItem && (i32IndexItem < m_vClouds.size()))
    {
        l_bbox = m_vClouds[i32IndexItem]->bBox();
        l_vTranslation = m_vCloudsParameters[i32IndexItem]->m_vTranslation;
    }
    else if(i32IndexItem < m_vMeshes.size())
    {
        l_bbox = m_vMeshes[i32IndexItem]->cloud()->bBox();
        l_vTranslation = m_vMeshesParameters[i32IndexItem]->m_vTranslation;
    }
    else
    {
        return;
    }

    l_v3FMiddlePoint[0] = (l_bbox.m_fMaxX + l_bbox.m_fMinX)*0.5f;
    l_v3FMiddlePoint[1] = (l_bbox.m_fMaxY + l_bbox.m_fMinY)*0.5f;
    l_v3FMiddlePoint[2] = (l_bbox.m_fMaxZ + l_bbox.m_fMinZ)*0.5f;
    l_distObject = l_bbox.diagLength();
    qDebug() << "bbox: " << l_v3FMiddlePoint[0] << " "<< l_v3FMiddlePoint[1] << " " << l_v3FMiddlePoint[2] << " " << l_distObject;

    QVector3D l_oPos(l_v3FMiddlePoint[0], l_v3FMiddlePoint[1], l_v3FMiddlePoint[2]+l_distObject);
    l_oPos += l_vTranslation;
    QVector3D l_oLookAt = l_oPos;
    l_oLookAt.setZ(-1000.0);
    QVector3D l_oUp(0.0,1.0,0.0);
    setCamera(l_oPos, l_oLookAt, l_oUp);
}


void SWGLMultiObjectWidget::beginAnimation(bool isCloudItem, int indexItem)
{
    if(isCloudItem)
    {
        if(indexItem < static_cast<int>(m_vCloudsParameters.size()))
        {
            m_vCloudsParameters[indexItem]->m_animationMutex.lockForWrite();
                m_vCloudsParameters[indexItem]->m_animationStarted = true;
            m_vCloudsParameters[indexItem]->m_animationMutex.unlock();
        }
    }
    else
    {
        if(indexItem < static_cast<int>(m_vMeshesParameters.size()))
        {
            m_vMeshesParameters[indexItem]->m_animationMutex.lockForWrite();
                m_vMeshesParameters[indexItem]->m_animationStarted = true;
            m_vMeshesParameters[indexItem]->m_animationMutex.unlock();
        }
    }
}

void SWGLMultiObjectWidget::stopAnimation(cbool isCloudItem, cuint indexItem)
{
    if(isCloudItem)
    {
        if(indexItem < static_cast<uint>(m_vCloudsParameters.size()))
        {
            m_vCloudsParameters[indexItem]->m_animationMutex.lockForWrite();
                m_vCloudsParameters[indexItem]->m_animationStarted = false;
            m_vCloudsParameters[indexItem]->m_animationMutex.unlock();
        }
    }
    else
    {
        if(indexItem < static_cast<uint>(m_vMeshesParameters.size()))
        {
            m_vMeshesParameters[indexItem]->m_animationMutex.lockForWrite();
                m_vMeshesParameters[indexItem]->m_animationStarted = false;
            m_vMeshesParameters[indexItem]->m_animationMutex.unlock();
        }
    }
}


void SWGLMultiObjectWidget::setAnimationOffset(SWAnimationSendDataPtr animationSendData)
{
    int indexItem = animationSendData->m_index;

    if(animationSendData->m_isCloud)
    {
        if(indexItem < static_cast<int>(m_vCloudsParameters.size()))
        {
            m_vCloudsParameters[indexItem]->m_animationMutex.lockForWrite();
                m_vCloudsParameters[indexItem]->m_animationOffsetsX = animationSendData->m_animationOffsetsX;
                m_vCloudsParameters[indexItem]->m_animationOffsetsY = animationSendData->m_animationOffsetsY;
                m_vCloudsParameters[indexItem]->m_animationOffsetsZ = animationSendData->m_animationOffsetsZ;
                m_vCloudsParameters[indexItem]->m_animationRigidMotion = animationSendData->m_animationRigidMotion;
//                m_vCloudsParameters[indexItem]->m_animationIndexRotTrans = animationSendData->m_;
            m_vCloudsParameters[indexItem]->m_animationMutex.unlock();
        }
    }
    else
    {
        if(indexItem < static_cast<int>(m_vMeshesParameters.size()))
        {
            m_vMeshesParameters[indexItem]->m_animationMutex.lockForWrite();
                m_vMeshesParameters[indexItem]->m_animationOffsetsX = animationSendData->m_animationOffsetsX;
                m_vMeshesParameters[indexItem]->m_animationOffsetsY = animationSendData->m_animationOffsetsY;
                m_vMeshesParameters[indexItem]->m_animationOffsetsZ = animationSendData->m_animationOffsetsZ;
                m_vMeshesParameters[indexItem]->m_animationRigidMotion = animationSendData->m_animationRigidMotion;
//                m_vMeshesParameters[indexItem]->m_animationIndexRotTrans = indexRotTrans;
            m_vMeshesParameters[indexItem]->m_animationMutex.unlock();
        }
    }
}

void SWGLMultiObjectWidget::drawClouds()
{
    // bind shader for clouds
    m_oShaderCloud.bind();
        checkGlError(true);

    // retrieve meshes number
    int l_i32CloudsNumber = m_vClouds.size();

    // display clouds
        for(int ii = 0; ii < l_i32CloudsNumber; ++ii)
        {                       
            // release buffers
                QGLBuffer::release(QGLBuffer::VertexBuffer);
                QGLBuffer::release(QGLBuffer::IndexBuffer);

            // retrieve data
                m_vCloudsParameters[ii]->m_parametersMutex.lockForRead();
                    bool l_bVisible = m_vCloudsParameters[ii]->m_bVisible;
                    float l_fScaling = static_cast<float>(m_vCloudsParameters[ii]->m_dScaling);
                    QVector3D l_vTranslation = m_vCloudsParameters[ii]->m_vTranslation;
                    QVector3D l_vRotation = m_vCloudsParameters[ii]->m_vRotation;
                    QVector3D l_vUnicolor = m_vCloudsParameters[ii]->m_vUnicolor;
                    GLObjectDisplayMode l_oDisplayMode = m_vCloudsParameters[ii]->displayMode;
                m_vCloudsParameters[ii]->m_parametersMutex.unlock();

            // check visibility
                if(!l_bVisible)
                {
                    continue;
                }

            // apply transformations
                std::vector<float> l_v3FMeanPoint = m_vClouds[ii]->meanPoint();
                QVector3D l_v3FTranslationToCenter(-l_v3FMeanPoint[0], -l_v3FMeanPoint[1], -l_v3FMeanPoint[2]);
                swCloud::SWRigidMotion l_oTransfo(l_vRotation.x(), l_vRotation.y(), l_vRotation.z());
                l_oTransfo.m_aFTranslation[0] = l_vTranslation.x();
                l_oTransfo.m_aFTranslation[1] = l_vTranslation.y();
                l_oTransfo.m_aFTranslation[2] = l_vTranslation.z();

                QMatrix4x4 l_oTransformation(l_oTransfo.m_aFRotation[0], l_oTransfo.m_aFRotation[1], l_oTransfo.m_aFRotation[2],l_oTransfo.m_aFTranslation[0],
                                             l_oTransfo.m_aFRotation[3], l_oTransfo.m_aFRotation[4], l_oTransfo.m_aFRotation[5],l_oTransfo.m_aFTranslation[1],
                                             l_oTransfo.m_aFRotation[6], l_oTransfo.m_aFRotation[7], l_oTransfo.m_aFRotation[8],l_oTransfo.m_aFTranslation[2],
                                             0.0, 0.0, 0.0, 1.0);

            // uniform
                m_oShaderCloud.setUniformValue("scaling", l_fScaling);
                m_oShaderCloud.setUniformValue("translationToCenter", l_v3FTranslationToCenter);
                m_oShaderCloud.setUniformValue("applyTransformation", true);
                m_oShaderCloud.setUniformValue("displayMode", l_oDisplayMode);
                m_oShaderCloud.setUniformValue("uniColor", l_vUnicolor.x()/255., l_vUnicolor.y()/255., l_vUnicolor.z()/255.);
                m_oShaderCloud.setUniformValue("mvpMatrix", m_oMVPMatrix);
                m_oShaderCloud.setUniformValue("transformation", l_oTransformation);

            if(m_vCloudsBufferToUpdate[ii])
            {
                // allocate buffers
                    float  *l_aFVertexBuffer   = m_vClouds[ii]->vertexBuffer();
                    float  *l_aFColorBuffer    = m_vClouds[ii]->colorBuffer();
                    uint32 *l_aUI32IndexBuffer = m_vClouds[ii]->indexBuffer();
                        allocateBuffer(*m_vCloudsVertexBuffer[ii], l_aFVertexBuffer,   m_vClouds[ii]->size() *  3 * sizeof(float) );
                        allocateBuffer(*m_vCloudsIndexBuffer[ii], l_aUI32IndexBuffer, m_vClouds[ii]->size() * sizeof(GLuint) );
                        allocateBuffer(*m_vCloudsColorBuffer[ii], l_aFColorBuffer,    m_vClouds[ii]->size() *  3 * sizeof(float));
                    deleteAndNullifyArray(l_aFVertexBuffer);
                    deleteAndNullifyArray(l_aUI32IndexBuffer);
                    deleteAndNullifyArray(l_aFColorBuffer);

                m_vCloudsBufferToUpdate[ii] = false;
            }


            // draw
                drawBufferWithColor(*m_vCloudsIndexBuffer[ii], *m_vCloudsVertexBuffer[ii],
                                    *m_vCloudsColorBuffer[ii], m_oShaderCloud, GL_POINTS);
        }

    m_oShaderCloud.release();
        checkGlError(true);
}

void SWGLMultiObjectWidget::drawMeshes()
{   
    // bind shader for meshes
    m_oShaderMesh.bind();
        checkGlError(true);

    // retrieve meshes number
    int l_i32MeshesNumber = m_vMeshes.size();

    // display meshes
        for(int ii = 0; ii < l_i32MeshesNumber; ++ii)
        {
            // release buffers
                QGLBuffer::release(QGLBuffer::VertexBuffer);
                QGLBuffer::release(QGLBuffer::IndexBuffer);

            // retrieve data
                m_vMeshesParameters[ii]->m_parametersMutex.lockForRead();
                    bool l_bVisible = m_vMeshesParameters[ii]->m_bVisible;
                    bool l_bDisplayLines = m_vMeshesParameters[ii]->m_bDisplayLines;
                    QVector3D l_vTranslation = m_vMeshesParameters[ii]->m_vTranslation;
                    QVector3D l_vRotation = m_vMeshesParameters[ii]->m_vRotation;
                    QVector3D l_vUnicolor = m_vMeshesParameters[ii]->m_vUnicolor;
                    QVector3D l_vSourceLight = m_vMeshesParameters[ii]->m_vSourceLight;
                    float l_fAmbiantK = static_cast<float>(m_vMeshesParameters[ii]->m_dAmbiantK);
                    float l_fDiffusK= static_cast<float>(m_vMeshesParameters[ii]->m_dDiffusK);
                    float l_fSpecularK = static_cast<float>(m_vMeshesParameters[ii]->m_dSpecularK);
                    float l_fSpecularP = static_cast<float>(m_vMeshesParameters[ii]->m_dSpecularP);
                    float l_fScaling = static_cast<float>(m_vMeshesParameters[ii]->m_dScaling);
                    QVector3D l_vAmbiantLight = m_vMeshesParameters[ii]->m_vAmbiantLight;
                    QVector3D l_vDiffusLight = m_vMeshesParameters[ii]->m_vDiffusLight;
                    QVector3D l_vSpecularLight = m_vMeshesParameters[ii]->m_vSpecularLight;
                    GLObjectDisplayMode l_oDisplayMode = m_vMeshesParameters[ii]->displayMode;
                    bool l_animationStarted = m_vMeshesParameters[ii]->m_animationStarted;
                m_vMeshesParameters[ii]->m_parametersMutex.unlock();

                m_vMeshesParameters[ii]->m_animationMutex.lockForRead();
                    QVector<float> l_animationOffsetsX = m_vMeshesParameters[ii]->m_animationOffsetsX;
                    QVector<float> l_animationOffsetsY = m_vMeshesParameters[ii]->m_animationOffsetsY;
                    QVector<float> l_animationOffsetsZ = m_vMeshesParameters[ii]->m_animationOffsetsZ;
                    QVector<float> l_animationRigidMotion = m_vMeshesParameters[ii]->m_animationRigidMotion;
                m_vMeshesParameters[ii]->m_animationMutex.unlock();


            // check visibility
                if(!l_bVisible)
                {
                    continue;
                }

            // set polygon mode
                if(l_bDisplayLines)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }


                // apply transformations
                    std::vector<float> l_v3FMeanPoint = m_vMeshes[ii]->cloud()->meanPoint();
                    QVector3D l_v3FTranslationToCenter(-l_v3FMeanPoint[0], -l_v3FMeanPoint[1], -l_v3FMeanPoint[2]);
                    swCloud::SWRigidMotion l_oTransfo(l_vRotation.x(), l_vRotation.y(), l_vRotation.z());
                    l_oTransfo.m_aFTranslation[0] = l_vTranslation.x();
                    l_oTransfo.m_aFTranslation[1] = l_vTranslation.y();
                    l_oTransfo.m_aFTranslation[2] = l_vTranslation.z();

                    QMatrix4x4 l_oTransformation(l_oTransfo.m_aFRotation[0], l_oTransfo.m_aFRotation[1], l_oTransfo.m_aFRotation[2],l_oTransfo.m_aFTranslation[0],
                                                 l_oTransfo.m_aFRotation[3], l_oTransfo.m_aFRotation[4], l_oTransfo.m_aFRotation[5],l_oTransfo.m_aFTranslation[1],
                                                 l_oTransfo.m_aFRotation[6], l_oTransfo.m_aFRotation[7], l_oTransfo.m_aFRotation[8],l_oTransfo.m_aFTranslation[2],
                                                 0.0, 0.0, 0.0, 1.0);

                // uniform
                    // camera
                        m_oShaderMesh.setUniformValue("viewDirection", -m_pCamera->lookAt());
                    // transformations
                        m_oShaderMesh.setUniformValue("scaling", l_fScaling);
                        m_oShaderMesh.setUniformValue("translationToCenter", l_v3FTranslationToCenter);
                        m_oShaderMesh.setUniformValue("applyTransformation", true);
                        m_oShaderMesh.setUniformValue("transformation", l_oTransformation);
                        m_oShaderMesh.setUniformValue("mvpMatrix", m_oMVPMatrix);
                    // color / texture
                        m_oShaderMesh.setUniformValue("displayMode", l_oDisplayMode);
                        m_oShaderMesh.setUniformValue("uniColor", l_vUnicolor.x()/255., l_vUnicolor.y()/255., l_vUnicolor.z()/255.);
                    // lights
                        m_oShaderMesh.setUniformValue("lSourcePos" , l_vSourceLight);
                        m_oShaderMesh.setUniformValue("kAmbiant" , l_fAmbiantK);
                        m_oShaderMesh.setUniformValue("kDiffus" , l_fDiffusK);
                        m_oShaderMesh.setUniformValue("kSpecular" , l_fSpecularK);
                        m_oShaderMesh.setUniformValue("pSpecular" , l_fSpecularP);
                        m_oShaderMesh.setUniformValue("lAmbiant" , l_vAmbiantLight);
                        m_oShaderMesh.setUniformValue("lDiffus" , l_vDiffusLight);
                        m_oShaderMesh.setUniformValue("lSpecular" , l_vSpecularLight);

            if(m_vMeshesBufferToUpdate[ii])
            {
                // allocate buffers
                    float  *l_aFVertexBuffer = NULL;

                    if(l_animationStarted)
                    {
                        swCloud::SWCloud l_cloudAnim;
                        l_cloudAnim.copy(*m_vMeshes[ii]->cloud());


                        std::vector<float> l_off(3,0.f);
                        l_off[0] = -l_cloudAnim.coord(0)[352];
                        l_off[1] = -l_cloudAnim.coord(1)[352];
                        l_off[2] = -l_cloudAnim.coord(2)[352];

                        l_cloudAnim += l_off;

//                        qDebug() << "verif :" << l_cloudAnim.coord(0)[352] << " " << l_cloudAnim.coord(1)[352] << " " << l_cloudAnim.coord(2)[352];

                        for(uint jj = 0; jj < l_cloudAnim.size(); ++jj)
                        {
                            l_cloudAnim.coord(0)[jj] += l_animationOffsetsX[jj];
                            l_cloudAnim.coord(1)[jj] += l_animationOffsetsY[jj];
                            l_cloudAnim.coord(2)[jj] += l_animationOffsetsZ[jj];
                        }

                        swCloud::SWRigidMotion l_rigidMotion((180.f/3.14)*l_animationRigidMotion[3],(180.f/3.14)*l_animationRigidMotion[4],(-180.f/3.14)*l_animationRigidMotion[5]);
                        l_rigidMotion.m_aFTranslation[0] = l_animationRigidMotion[0]/40.f;
                        l_rigidMotion.m_aFTranslation[1] = l_animationRigidMotion[1]/40.f;
                        l_rigidMotion.m_aFTranslation[2] = -l_animationRigidMotion[2]/40.f;
                        l_cloudAnim.transform(l_rigidMotion.m_aFRotation, l_rigidMotion.m_aFTranslation);

                        l_off[0] = -l_off[0];
                        l_off[1] = -l_off[1];
                        l_off[2] = -l_off[2];

                        l_cloudAnim += l_off;

//                        qDebug() << "verif :" << l_cloudAnim.coord(0)[352] << " " << l_cloudAnim.coord(1)[352] << " " << l_cloudAnim.coord(2)[352];

                        l_aFVertexBuffer = l_cloudAnim.vertexBuffer();
                    }
                    else
                    {
                        l_aFVertexBuffer = m_vMeshes[ii]->vertexBuffer();
                    }

                    float  *l_aFColorBuffer    = m_vMeshes[ii]->cloud()->colorBuffer();
                    uint32 *l_aUI32IndexBuffer = m_vMeshes[ii]->indexVertexTriangleBuffer();
                    float  *l_aFNormalBuffer   = m_vMeshes[ii]->normalBuffer();
                    float  *l_aFTextureBuffer  = m_vMeshes[ii]->textureBuffer();
                        allocateBuffer(*m_vMeshesVertexBuffer[ii],  l_aFVertexBuffer,     m_vMeshes[ii]->pointsNumber() *  3 * sizeof(float) );
                        allocateBuffer(*m_vMeshesIndexBuffer[ii],   l_aUI32IndexBuffer,   m_vMeshes[ii]->trianglesNumber() * 3* sizeof(GLuint) );
                        allocateBuffer(*m_vMeshesColorBuffer[ii],   l_aFColorBuffer,      m_vMeshes[ii]->pointsNumber() *  3 * sizeof(float));
                        allocateBuffer(*m_vMeshesNormalBuffer[ii],  l_aFNormalBuffer,     m_vMeshes[ii]->pointsNumber() *  3 * sizeof(float));
                        allocateBuffer(*m_vMeshesTextureBuffer[ii], l_aFTextureBuffer,    m_vMeshes[ii]->pointsNumber() *  2 * sizeof(float) );
                    deleteAndNullifyArray(l_aFVertexBuffer);
                    deleteAndNullifyArray(l_aUI32IndexBuffer);
                    deleteAndNullifyArray(l_aFColorBuffer);
                    deleteAndNullifyArray(l_aFNormalBuffer);
                    deleteAndNullifyArray(l_aFTextureBuffer);

                m_vMeshesBufferToUpdate[ii] = false;
            }
            else if(l_animationStarted)
            {
                float  *l_aFVertexBuffer   = NULL;

                swCloud::SWCloud l_cloudAnim;
                l_cloudAnim.copy(*m_vMeshes[ii]->cloud());


                std::vector<float> l_off(3,0.f);
                l_off[0] = -l_cloudAnim.coord(0)[352];
                l_off[1] = -l_cloudAnim.coord(1)[352];
                l_off[2] = -l_cloudAnim.coord(2)[352];

                l_cloudAnim += l_off;

                for(uint jj = 0; jj < l_cloudAnim.size(); ++jj)
                {
                    l_cloudAnim.coord(0)[jj] += l_animationOffsetsX[jj];
                    l_cloudAnim.coord(1)[jj] += l_animationOffsetsY[jj];
                    l_cloudAnim.coord(2)[jj] += l_animationOffsetsZ[jj];
                }

                swCloud::SWRigidMotion l_rigidMotion((180.f/3.14)*l_animationRigidMotion[3],(180.f/3.14)*l_animationRigidMotion[4],(-180.f/3.14)*l_animationRigidMotion[5]);
                l_rigidMotion.m_aFTranslation[0] = l_animationRigidMotion[0]/40.f;
                l_rigidMotion.m_aFTranslation[1] = l_animationRigidMotion[1]/40.f;
                l_rigidMotion.m_aFTranslation[2] = -l_animationRigidMotion[2]/40.f;
                l_cloudAnim.transform(l_rigidMotion.m_aFRotation, l_rigidMotion.m_aFTranslation);

                l_off[0] = -l_off[0];
                l_off[1] = -l_off[1];
                l_off[2] = -l_off[2];

                l_cloudAnim += l_off;


                l_aFVertexBuffer = l_cloudAnim.vertexBuffer();

                allocateBuffer(*m_vMeshesVertexBuffer[ii],  l_aFVertexBuffer,     m_vMeshes[ii]->pointsNumber() *  3 * sizeof(float) );
                deleteAndNullifyArray(l_aFVertexBuffer);
            }

            // draw
                    if(l_oDisplayMode == GLO_ORIGINAL_COLOR)
                    {
                        drawBufferWithColor(*m_vMeshesIndexBuffer[ii], *m_vMeshesVertexBuffer[ii],
                                            *m_vMeshesColorBuffer[ii], *m_vMeshesNormalBuffer[ii], m_oShaderMesh, GL_TRIANGLES);
                    }
                    else if(l_oDisplayMode == GLO_TEXTURE)
                    {
                        glEnable(GL_TEXTURE_2D);

                        // bind texture
                            glBindTexture(GL_TEXTURE_2D, m_vMeshesParameters[ii]->m_textureLocation);

                        drawBufferWithTexture(*m_vMeshesIndexBuffer[ii], *m_vMeshesVertexBuffer[ii],
                                              *m_vMeshesTextureBuffer[ii],*m_vMeshesNormalBuffer[ii], m_oShaderMesh, GL_TRIANGLES);

                        glDisable(GL_TEXTURE_2D);
                    }
                    else if(l_oDisplayMode == GLO_UNI_COLOR)
                    {
                        drawBuffer(*m_vMeshesIndexBuffer[ii], *m_vMeshesVertexBuffer[ii],
                                   *m_vMeshesNormalBuffer[ii], m_oShaderMesh, GL_TRIANGLES);
                    }
        }

        m_oShaderMesh.release();
}

void SWGLMultiObjectWidget::drawScene()
{
    drawAxes(m_oShaderCloud, m_oMVPMatrix, 0.02f);
    drawCubeMap(m_oShaderCloud, m_oMVPMatrix);

    m_pListCloudsMutex.lockForRead();
        drawClouds();
    m_pListCloudsMutex.unlock();

    m_pListMeshesMutex.lockForRead();
        drawMeshes();
    m_pListMeshesMutex.unlock();
}



