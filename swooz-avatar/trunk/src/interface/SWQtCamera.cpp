
/**
 * \file SWQtCamera.cpp
 * \brief defines SWQtCamera
 * \author Florian Lance
 * \date 11/04/13
 */

#include "interface/SWQtCamera.h"
		
// ############################################# CONSTRUCTORS / DESTRUCTORS

SWQtCamera::SWQtCamera(const QVector3D &oEyePosition, const QVector3D &oLookAt, const QVector3D &oUp) : 
		       m_oEyePosition(oEyePosition), 	    m_oLookAt(oLookAt),        m_oUp(oUp), 
		       m_oInitialEyePosition(oEyePosition), m_oInitialLookAt(oLookAt), m_oInitialUp(oUp)
{	
	m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
	m_oLeft.normalize();
	
	m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
	m_oForeward.normalize();
	
    m_oUp.normalize();
}

SWQtCamera::~SWQtCamera()
{}

// ############################################# METHODS

QVector3D SWQtCamera::eyePosition() const
{
	return m_oEyePosition;
}
		
QVector3D SWQtCamera::viewDirection() const
{
	return m_oLookAt;
}
	
QVector3D SWQtCamera::up() const
{
	return m_oUp;
}

void SWQtCamera::rotate(cfloat fX, cfloat fY, cfloat fZ, cfloat fAmount)
{
    QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(QVector3D(fX, fY, fZ), fAmount));

    m_oLookAt   = l_oRotationQ.rotatedVector(m_oLookAt);
    m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
    m_oForeward.normalize();

    m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
    m_oLeft.normalize();
}
	
void SWQtCamera::rotateX(cfloat fAmount)
{
	QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oUp, fAmount));
	
	m_oLookAt   = l_oRotationQ.rotatedVector(m_oLookAt);
    m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
    m_oForeward.normalize();

    m_oLeft = QVector3D::crossProduct(m_oUp, QVector3D(m_oLookAt - m_oEyePosition));
    m_oLeft.normalize();
}

void SWQtCamera::rotateY(cfloat fAmount)
{
	QQuaternion l_oRotationQ(QQuaternion::fromAxisAndAngle(m_oLeft, fAmount));

	m_oLookAt   = l_oRotationQ.rotatedVector(m_oLookAt);
    m_oForeward = QVector3D(m_oLookAt - m_oEyePosition);
    m_oForeward.normalize();
	
    m_oUp = QVector3D::crossProduct(QVector3D(m_oLookAt - m_oEyePosition), m_oLeft);
    m_oUp.normalize();
}

void SWQtCamera::moveForeward(cfloat fAmount)
{
	m_oEyePosition += m_oForeward * fAmount;
	m_oLookAt      += m_oForeward * fAmount;
}

void SWQtCamera::moveBackward(cfloat fAmount)
{
	m_oEyePosition -= m_oForeward * fAmount;
	m_oLookAt      -= m_oForeward * fAmount;
}

void SWQtCamera::moveLeft(cfloat fAmount)
{
	m_oEyePosition += m_oLeft * fAmount;
	m_oLookAt      += m_oLeft * fAmount;
}

void SWQtCamera::moveRight(cfloat fAmount)
{
	m_oEyePosition -= m_oLeft * fAmount;
	m_oLookAt      -= m_oLeft * fAmount;
}

void SWQtCamera::moveUp(cfloat fAmount)
{
    m_oEyePosition += m_oUp * fAmount;
    m_oLookAt      += m_oUp * fAmount;
}

void SWQtCamera::moveDown(cfloat fAmount)
{
    m_oEyePosition -= m_oUp * fAmount;
    m_oLookAt      -= m_oUp * fAmount;
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
