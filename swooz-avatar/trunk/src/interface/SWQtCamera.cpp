
/**
 * \file SWQtCamera.cpp
 * \brief defines SWQtCamera
 * \author Florian Lance
 * \date 11/04/13
 */

#include "interface/SWQtCamera.h"
		
// ############################################# CONSTRUCTORS / DESTRUCTORS

SWQtCamera::SWQtCamera(const QVector3D &oEyePosition, const QVector3D &oLookAt, const QVector3D &oUp, const CameraMode oCameraMode) :
		       m_oEyePosition(oEyePosition), 	    m_oLookAt(oLookAt),        m_oUp(oUp), 
               m_oInitialEyePosition(oEyePosition), m_oInitialLookAt(oLookAt), m_oInitialUp(oUp), m_oCameraMode(oCameraMode)
{	
	m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
	m_oLeft.normalize();
	
	m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
	m_oForeward.normalize();
	
    m_oUp.normalize();
}

SWQtCamera::~SWQtCamera()
{}

void SWQtCamera::set(const QVector3D &oEyePosition, const QVector3D &oLookAt, const QVector3D &oUp)
{
    m_oEyePosition = oEyePosition;
    m_oLookAt = oLookAt;
    m_oUp = oUp;

    m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
    m_oLeft.normalize();

    m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
    m_oForeward.normalize();

    m_oUp.normalize();
}

void SWQtCamera::setInitial(const QVector3D &oEyeInitialPosition, const QVector3D &oInitialLookAt, const QVector3D &oInitialUp)
{
    m_oInitialEyePosition = oEyeInitialPosition;
    m_oInitialLookAt      = oInitialLookAt;
    m_oInitialUp          = oInitialUp;
}

// ############################################# METHODS

QVector3D SWQtCamera::eyePosition() const
{
	return m_oEyePosition;
}
		
QVector3D SWQtCamera::lookAt() const
{
    return m_oLookAt;
}

QVector3D SWQtCamera::forward() const
{
    return m_oForeward;
}
	
QVector3D SWQtCamera::up() const
{
    return m_oUp;
}

SWQtCamera::CameraMode SWQtCamera::cameraMode() const
{
    return m_oCameraMode;
}

void SWQtCamera::setCameraMode(const CameraMode oCameraMode)
{
    m_oCameraMode = oCameraMode;
}

void SWQtCamera::rotate(cfloat fX, cfloat fY, cfloat fZ, cfloat fAmount)
{
    QVector3D l_v(fX, fY, fZ);
    rotateX(fY/l_v.length() * fAmount);
    rotateY(fX/l_v.length() * fAmount);
    rotateZ(fZ/l_v.length() * fAmount);
}
	
void SWQtCamera::rotateX(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oUp, fAmount));
        m_oLookAt   = l_oRotationQ.rotatedVector(m_oLookAt);
        m_oForeward = l_oRotationQ.rotatedVector(m_oForeward);
        m_oLeft     = l_oRotationQ.rotatedVector(m_oLeft);
        m_oUp       = l_oRotationQ.rotatedVector(m_oUp);
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        moveLeft(-fAmount);
    }
}

void SWQtCamera::rotateY(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oLeft, fAmount));
        m_oLookAt   = l_oRotationQ.rotatedVector(m_oLookAt);
        m_oForeward = l_oRotationQ.rotatedVector(m_oForeward);
        m_oLeft     = l_oRotationQ.rotatedVector(m_oLeft);
        m_oUp       = l_oRotationQ.rotatedVector(m_oUp);
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        moveUp(fAmount);
    }
}

void SWQtCamera::rotateZ(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oForeward, fAmount));
        m_oLookAt   = l_oRotationQ.rotatedVector(m_oLookAt);
        m_oForeward = l_oRotationQ.rotatedVector(m_oForeward);
        m_oLeft     = l_oRotationQ.rotatedVector(m_oLeft);
        m_oUp       = l_oRotationQ.rotatedVector(m_oUp);
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oForeward, fAmount));
        m_oForeward = l_oRotationQ.rotatedVector(m_oForeward);
        m_oLeft     = l_oRotationQ.rotatedVector(m_oLeft);
        m_oUp       = l_oRotationQ.rotatedVector(m_oUp);
    }
}

void SWQtCamera::moveForeward(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        m_oEyePosition += m_oForeward * fAmount;
        m_oLookAt      += m_oForeward * fAmount;
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        float l_fLength = (m_oEyePosition - m_oLookAt).length();

        if(fAmount > 0.f && fAmount < 1.f)
        {
            m_oEyePosition += m_oForeward * (fAmount * l_fLength);
        }
    }
}

void SWQtCamera::moveBackward(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        m_oEyePosition -= m_oForeward * fAmount;
        m_oLookAt      -= m_oForeward * fAmount;
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        float l_fLength = (m_oEyePosition - m_oLookAt).length();

        if(fAmount > 0.f && fAmount < 1.f)
        {
            m_oEyePosition -= m_oForeward * (fAmount * l_fLength);
        }
    }
}

void SWQtCamera::moveLeft(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        m_oEyePosition += m_oLeft * fAmount;
        m_oLookAt      += m_oLeft * fAmount;
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        QVector3D l_oNewPos = m_oEyePosition - m_oLookAt;
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oUp, -fAmount));
        m_oEyePosition = l_oRotationQ.rotatedVector(l_oNewPos) + m_oLookAt;

        m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
        m_oForeward.normalize();

        m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
        m_oLeft.normalize();
    }
}

void SWQtCamera::moveRight(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        m_oEyePosition -= m_oLeft * fAmount;
        m_oLookAt      -= m_oLeft * fAmount;
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        QVector3D l_oNewPos = m_oEyePosition - m_oLookAt;
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oUp, +fAmount));
        m_oEyePosition = l_oRotationQ.rotatedVector(l_oNewPos) + m_oLookAt;

        m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
        m_oForeward.normalize();

        m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
        m_oLeft.normalize();
    }
}

void SWQtCamera::moveUp(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        m_oEyePosition += m_oUp * fAmount;
        m_oLookAt      += m_oUp * fAmount;
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        QVector3D l_oNewPos = m_oEyePosition - m_oLookAt;
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oLeft, fAmount));
        m_oEyePosition = l_oRotationQ.rotatedVector(l_oNewPos) + m_oLookAt;

        m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
        m_oForeward.normalize();

        m_oUp = QVector3D::crossProduct(QVector3D(m_oLookAt - m_oEyePosition), m_oLeft);
        m_oUp.normalize();
    }
}

void SWQtCamera::moveDown(cfloat fAmount)
{
    if(m_oCameraMode == FPS_CAMERA)
    {
        m_oEyePosition -= m_oUp * fAmount;
        m_oLookAt      -= m_oUp * fAmount;
    }
    else if(m_oCameraMode == TRACKBALL_CAMERA)
    {
        QVector3D l_oNewPos = m_oEyePosition - m_oLookAt;
        QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oLeft, -fAmount));
        m_oEyePosition = l_oRotationQ.rotatedVector(l_oNewPos) + m_oLookAt;

        m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
        m_oForeward.normalize();

        m_oUp = QVector3D::crossProduct(QVector3D(m_oLookAt - m_oEyePosition), m_oLeft);
        m_oUp.normalize();
    }
}

void SWQtCamera::reset()
{
	m_oUp 	       = m_oInitialUp;
	m_oLookAt      = m_oInitialLookAt;
	m_oEyePosition = m_oInitialEyePosition;	
	
	m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
	m_oLeft.normalize();	
	
	m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
	m_oForeward.normalize();
}
