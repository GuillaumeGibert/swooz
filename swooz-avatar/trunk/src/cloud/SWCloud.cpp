
/**
 * \file SWCloud.cpp
 * \brief defines SWCloud
 * \author Florian Lance
 * \date 06/12/12
 */

#include "cloud/SWCloud.h"
#include "SWExceptions.h"

#include <iostream>
#include <fstream>

#include <time.h>
#include <iterator>
#include <sstream>

#include "geometryUtility.h"

using namespace std;
using namespace swCloud;
using namespace swExcept;

int SWCloud::m_i32NumberOfCreatedClouds   = 0;			
int SWCloud::m_i32NumberOfDestroyedClouds = 0;

#define PI 3.14159265 /**< pi constant definition */

// ############################################# CONSTRUCTORS / DESTRUCTORS - SWRigidMotion

SWRigidMotion::SWRigidMotion()
{    
    for(uint ii = 0; ii < 9; ++ii)
    {
        m_aFRotation[ii] = 0.f;
        if(ii < 3)
        {
            m_aFTranslation[ii] = 0.f;
        }
    }

    m_aFRotation[0] = 1.f;
    m_aFRotation[4] = 1.f;
    m_aFRotation[8] = 1.f;

    computeRotationAnglesWithRotationMatrix();
    computeQuaternionsWithRotationMatrix();
}

SWRigidMotion::SWRigidMotion(cfloat *aFRotation, cfloat *aFTranslation, bool bRotMat)
{
    if(bRotMat)
    {
        for(uint ii = 0; ii < 9; ++ii)
        {
            m_aFRotation[ii] = aFRotation[ii];

            if(ii < 3)
            {
                m_aFTranslation[ii] = aFTranslation[ii];
            }
        }

        computeRotationAnglesWithRotationMatrix();
    }
    else
    {
        for(uint ii = 0; ii < 3; ++ii)
        {
            m_aFRotAngles[ii] = aFRotation[ii];
            m_aFTranslation[ii] = aFTranslation[ii];
        }

        computeRotationMatrixWithRotationAngles();
    }

    computeQuaternionsWithRotationMatrix();
}

//SWRigidMotion::SWRigidMotion(cfloat fRotation, vector<float> v3FAxe) // TODO : test
//{
//    swUtil::normalize(v3FAxe);

//    float c = cos(fRotation), s = sin(fRotation);
//    float ux = v3FAxe[0], uy = v3FAxe[1], uz = v3FAxe[2];

//    m_aFRotation[0] = ux * ux + (1 - ux * ux) * c;
//    m_aFRotation[1] = ux * uy + (1 - c) - uz * s;
//    m_aFRotation[2] = ux * uz * (1 - c) + uy * s;

//    m_aFRotation[3] = ux * uy * (1 - c) + uz * s;
//    m_aFRotation[4] = uy * uy + (1 - uy * uy) * c;
//    m_aFRotation[5] = uy * uz * (1 - c) - ux * s;

//    m_aFRotation[6] = ux * uz * (1 - c) - uy * s;
//    m_aFRotation[7] = uy * uz * (1 - c) + ux * s;
//    m_aFRotation[8] = uz * uz + (1 - uz * uz) * c;

//    m_aFTranslation[0] = 0.f;
//    m_aFTranslation[1] = 0.f;
//    m_aFTranslation[2] = 0.f;
//}

SWRigidMotion::SWRigidMotion(cfloat fAngle, vector<float> v3FAxe)
{
    float sina = sin(fAngle / 2);
    float cosa = cos(fAngle / 2);

    std::vector<float> l_aFQuarternions(4);
    l_aFQuarternions[0] = v3FAxe[0] * sina;
    l_aFQuarternions[1] = v3FAxe[1] * sina;
    l_aFQuarternions[2] = v3FAxe[2] * sina;
    l_aFQuarternions[3] = cosa;


    swUtil::normalize(l_aFQuarternions);

    for(int ii = 0; ii < 4; ++ii)
    {
        if(ii < 3)
        {
            m_aFTranslation[ii] = 0;
        }

        m_aFQuaternions[ii] = l_aFQuarternions[ii];
    }

    computeRotationMatrixWithQuaternions();
    computeRotationAnglesWithRotationMatrix();
}



//SWRigidMotion::SWRigidMotion(cfloat fLati, cfloat fLong, cfloat fAngle,  cbool bTest)
//{
//    float sina = sin(fAngle / 2);
//    float cosa = cos(fAngle /2);

//    float sinlat = sin(fLati);
//    float coslat = cos(fLati);

//    float sinlong = sin(fLong);
//    float coslong = cos(fLong);

//    m_aFQuaternions[0] = sina * coslat * sinlong;
//    m_aFQuaternions[1] = sina * sinlat;
//    m_aFQuaternions[2] = sina * sinlat * coslong;
//    m_aFQuaternions[3] = cosa;

//    computeRotationMatrixWithQuaternions();
//    computeRotationAnglesWithRotationMatrix();
//}


SWRigidMotion::SWRigidMotion(cfloat fRotationX, cfloat fRotationY, cfloat fRotationZ)
{
    for(uint ii = 0; ii < 9; ++ii)
    {
        m_aFRotation[ii] = 0.f;

        if(ii < 3)
        {
            m_aFTranslation[ii] = 0.f;
        }
    }

    m_aFRotAngles[0] = fRotationX;
    m_aFRotAngles[1] = fRotationY;
    m_aFRotAngles[2] = fRotationZ;

    computeRotationMatrixWithRotationAngles();
    computeQuaternionsWithRotationMatrix();
}

SWRigidMotion::SWRigidMotion(const SWRigidMotion &oRigidMotion)
{
    for(uint ii = 0; ii < 9; ++ii)
    {
        m_aFRotation[ii] = oRigidMotion.m_aFRotation[ii];

        if(ii< 4)
        {
            if(ii < 3)
            {
                m_aFTranslation[ii] = oRigidMotion.m_aFTranslation[ii];
                m_aFRotAngles[ii]   = oRigidMotion.m_aFRotAngles[ii];
            }

            m_aFQuaternions[ii] = oRigidMotion.m_aFQuaternions[ii];
        }
    }
}

SWRigidMotion::~SWRigidMotion()
{}


// ############################################# METHODS - SWRigidMotion

void SWRigidMotion::computeRotationMatrixWithQuaternions()
{
    float xx = m_aFQuaternions[0] * m_aFQuaternions[0];
    float xy = m_aFQuaternions[0] * m_aFQuaternions[1];
    float xz = m_aFQuaternions[0] * m_aFQuaternions[2];
    float xw = m_aFQuaternions[0] * m_aFQuaternions[3];

    float yy = m_aFQuaternions[1] * m_aFQuaternions[1];
    float yz = m_aFQuaternions[1] * m_aFQuaternions[2];
    float yw = m_aFQuaternions[1] * m_aFQuaternions[3];

    float zz = m_aFQuaternions[2] * m_aFQuaternions[2];
    float zw = m_aFQuaternions[2] * m_aFQuaternions[3];


    m_aFRotation[0] = 1 - 2 * ( yy + zz );
    m_aFRotation[1] =     2 * ( xy - zw );
    m_aFRotation[2] =     2 * ( xz + yw );

    m_aFRotation[3] =     2 * ( xy + zw );
    m_aFRotation[4] = 1 - 2 * ( xx + zz );
    m_aFRotation[5] =     2 * ( yz - xw );

    m_aFRotation[6] =     2 * ( xz - yw );
    m_aFRotation[7] =     2 * ( yz + xw );
    m_aFRotation[8] = 1 - 2 * ( xx + yy );
}


void SWRigidMotion::computeRotationAnglesWithRotationMatrix()
{
    float C,D,RAD = 180.f / (float)PI;
    float trX, trY;

    m_aFRotAngles[1]  = D = -asin(m_aFRotation[2]);
    C		  = cos(D);
    m_aFRotAngles[1] *= RAD;

    if(fabs(C) > 0.005f)  /* Gimbal lock ? */
    {
        // No
        trX =  m_aFRotation[8] / C;
        trY = -m_aFRotation[5] / C;

        m_aFRotAngles[0] = atan2(trY, trX) * RAD;

        trX =  m_aFRotation[0] / C;
        trY = -m_aFRotation[1] / C;

        m_aFRotAngles[2] = atan2(trY, trX) * RAD;
    }
    else
    {
        // Yes
        m_aFRotAngles[0] = 0;

        trX = m_aFRotation[4];
        trY = m_aFRotation[3];

        m_aFRotAngles[2] = atan2(trY, trX) * RAD;
    }        
}

void SWRigidMotion::computeRotationMatrixWithRotationAngles()
{
    float A = cos((float)PI * m_aFRotAngles[0] / 180.f);
    float B = sin((float)PI * m_aFRotAngles[0] / 180.f);
    float C = cos((float)PI * m_aFRotAngles[1] / 180.f);
    float D = sin((float)PI * m_aFRotAngles[1] / 180.f);
    float E = cos((float)PI * m_aFRotAngles[2] / 180.f);
    float F = sin((float)PI * m_aFRotAngles[2] / 180.f);

    float AD = A * D;
    float BD = B * D;

    m_aFRotation[0] =  C * E;
    m_aFRotation[1] = -C * F;
    m_aFRotation[2] = -D;

    m_aFRotation[3] = -BD * E + A * F;
    m_aFRotation[4] =  BD * F + A * E;
    m_aFRotation[5] = -B  * C;

    m_aFRotation[6] =  AD * E + B * F;
    m_aFRotation[7] = -AD * F + B * E;
    m_aFRotation[8] =  A  * C;
}

void SWRigidMotion::computeQuaternionsWithRotationMatrix()
{
    float l_fTrace = m_aFRotation[0] + m_aFRotation[4] + m_aFRotation[8] + 1;
    float l_fS;

    if(l_fTrace > 0)
    {
        l_fS = 0.5f / sqrt(l_fTrace);

        m_aFQuaternions[0] = (m_aFRotation[7] - m_aFRotation[5])*l_fS;
        m_aFQuaternions[1] = (m_aFRotation[2] - m_aFRotation[6])*l_fS;
        m_aFQuaternions[2] = (m_aFRotation[3] - m_aFRotation[1])*l_fS;
        m_aFQuaternions[3] = 0.25f / l_fS;
    }
    else
    {
        if(m_aFRotation[0] > m_aFRotation[4] && m_aFRotation[0] >  m_aFRotation[8])
        {
            l_fS = sqrt(1 + m_aFRotation[0] - m_aFRotation[4] - m_aFRotation[8]*2);
            m_aFQuaternions[0] = 0.25f * l_fS;
            m_aFQuaternions[1] = (m_aFRotation[1] + m_aFRotation[3]) / l_fS;
            m_aFQuaternions[2] = (m_aFRotation[2] + m_aFRotation[6]) / l_fS;
            m_aFQuaternions[3] = (m_aFRotation[5] - m_aFRotation[7]) / l_fS;
        }
        else if(m_aFRotation[4] > m_aFRotation[0] && m_aFRotation[4] > m_aFRotation[8])
        {
            l_fS = sqrt(1 - m_aFRotation[0] + m_aFRotation[4] - m_aFRotation[8]*2);
            m_aFQuaternions[0] = (m_aFRotation[1] + m_aFRotation[3]) / l_fS;
            m_aFQuaternions[1] = 0.25f * l_fS;
            m_aFQuaternions[2] = (m_aFRotation[5] + m_aFRotation[7]) / l_fS;
            m_aFQuaternions[3] = (m_aFRotation[2] - m_aFRotation[6]) / l_fS;
        }
        else
        {
            l_fS = sqrt(1 - m_aFRotation[0] - m_aFRotation[4] + m_aFRotation[8]*2);
            m_aFQuaternions[0] = (m_aFRotation[2] + m_aFRotation[6]) / l_fS;
            m_aFQuaternions[1] = (m_aFRotation[5] + m_aFRotation[7]) / l_fS;
            m_aFQuaternions[2] = 0.25f * l_fS;
            m_aFQuaternions[3] = (m_aFRotation[1] - m_aFRotation[3]) / l_fS;
        }
    }
}

void SWRigidMotion::set(cfloat *aFRotation, cfloat *aFTranslation)
{
    for(uint ii = 0; ii < 9; ++ii)
    {
        m_aFRotation[ii] = aFRotation[ii];

        if(ii < 3)
        {
            m_aFTranslation[ii] = aFTranslation[ii];
        }
    }

    computeRotationAnglesWithRotationMatrix();
    computeQuaternionsWithRotationMatrix();
}

void SWRigidMotion::display()
{
    std::string l_sCurrValue, l_sToDiplay = "R : (";

    for(uint ii = 0; ii < 3; ++ii)
    {
        std::ostringstream l_os;
        l_os << m_aFRotAngles[ii];
        l_sCurrValue = l_os.str();
        l_sCurrValue.append(20 - l_sCurrValue.size(), '.');
        l_sToDiplay += l_sCurrValue;

        if(ii != 2)
            l_sToDiplay += ",";
    }

    l_sToDiplay += ") T : (";

    for(uint ii = 0; ii < 3; ++ii)
    {
        std::ostringstream l_os;
        l_os << m_aFTranslation[ii];
        l_sCurrValue = l_os.str();
        l_sCurrValue.append(20 - l_sCurrValue.size(), '.');
        l_sToDiplay += l_sCurrValue;

        if(ii != 2)
            l_sToDiplay += ",";
    }

    l_sToDiplay += ")";
    std::cout << l_sToDiplay <<  std::endl;
}

void SWRigidMotion::displayRotMatrix()
{
    std::cout << "Rot matrix : " << std::endl;
    for(int ii = 0; ii < 9; ++ii)
    {
        if(ii%3 == 0 && ii != 0)
        {
            std::cout << std::endl;
        }
        std::cout << m_aFRotation[ii] << " ";
    }
}


// ############################################# CONSTRUCTORS / DESTRUCTORS - SWCloud

SWCloud::SWCloud() : m_ui32NumberOfPoints(0), m_ui32ArraySize(0), m_aFCoords(NULL), m_aUi8Colors(NULL)
{
    ++m_i32NumberOfCreatedClouds;
}

SWCloud::SWCloud(cuint ui32NumberOfPoint, float *aCoords, uint8 *aUi8Colors) : 
	m_ui32NumberOfPoints(ui32NumberOfPoint), m_ui32ArraySize(3*ui32NumberOfPoint), m_aFCoords(aCoords), m_aUi8Colors(aUi8Colors)
{
	++m_i32NumberOfCreatedClouds;    
}

SWCloud::SWCloud(const std::string &sPathObjFile) : m_ui32NumberOfPoints(0), m_ui32ArraySize(0), m_aFCoords(NULL), m_aUi8Colors(NULL)
{
    ++m_i32NumberOfCreatedClouds;
    loadObj(sPathObjFile);
}

SWCloud::SWCloud(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ) :
                 m_ui32NumberOfPoints(0), m_ui32ArraySize(0), m_aFCoords(NULL), m_aUi8Colors(NULL)
{
	++m_i32NumberOfCreatedClouds;		
	
	if(vPX.size() > 0 && vPX.size() == vPY.size() && vPY.size() == vPZ.size())
	{
        m_ui32NumberOfPoints = static_cast<uint>(vPX.size());
		m_ui32ArraySize      = m_ui32NumberOfPoints * 3;
		
		m_aFCoords   = new float[m_ui32ArraySize];
		m_aUi8Colors = new uint8[m_ui32ArraySize];
		
		for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
		{
			m_aFCoords[0*m_ui32NumberOfPoints + ii] = vPX[ii];
			m_aFCoords[1*m_ui32NumberOfPoints + ii] = vPY[ii];
			m_aFCoords[2*m_ui32NumberOfPoints + ii] = vPZ[ii];

			m_aUi8Colors[0*m_ui32NumberOfPoints + ii] = 255;
			m_aUi8Colors[1*m_ui32NumberOfPoints + ii] = 0;
			m_aUi8Colors[2*m_ui32NumberOfPoints + ii] = 0;
		}
	}
    else if(vPX.size() == vPY.size() && vPY.size() == vPZ.size())
    {
        std::cout << "SWCloud vPX, vPY, vPZ constructor -> the cloud is null. " << std::endl;
        m_aFCoords   	     = NULL;
        m_aUi8Colors         = NULL;
        m_ui32NumberOfPoints = 0;
        m_ui32ArraySize	     = 0;
    }
	else
	{
        std::cerr << "SWCloud vPX, vPY, vPZ constructor -> bad parameters, array don't have all the same size " << std::endl;
		m_aFCoords   	     = NULL;
		m_aUi8Colors         = NULL;
		m_ui32NumberOfPoints = 0;
		m_ui32ArraySize	     = 0;
		
		throw cloudBadDataError();
	}
}


SWCloud::SWCloud(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ, 
         const std::vector<uint8> &vR,  const std::vector<uint8> &vG,  const std::vector<uint8> &vB) :
         m_ui32NumberOfPoints(0), m_ui32ArraySize(0), m_aFCoords(NULL), m_aUi8Colors(NULL)
{
	++m_i32NumberOfCreatedClouds;	
	
	if(vPX.size() > 0 && vPX.size() == vPY.size() && vPY.size() == vPZ.size() && vPZ.size() == vR.size() && vR.size() == vG.size() && vG.size() == vB.size())
	{
        m_ui32NumberOfPoints = static_cast<uint>(vPX.size());
		m_ui32ArraySize      = m_ui32NumberOfPoints * 3;
		
		m_aFCoords   = new float[m_ui32ArraySize];
		m_aUi8Colors = new uint8[m_ui32ArraySize];
		
		for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
		{
			m_aFCoords[0*m_ui32NumberOfPoints + ii] = vPX[ii];
			m_aFCoords[1*m_ui32NumberOfPoints + ii] = vPY[ii];
			m_aFCoords[2*m_ui32NumberOfPoints + ii] = vPZ[ii];
			
			m_aUi8Colors[0*m_ui32NumberOfPoints + ii] = vR[ii];
			m_aUi8Colors[1*m_ui32NumberOfPoints + ii] = vG[ii];
			m_aUi8Colors[2*m_ui32NumberOfPoints + ii] = vB[ii];
		}		
	}
    else if(vPX.size() == vPY.size() && vPY.size() == vPZ.size() && vPZ.size() == vR.size() && vR.size() == vG.size() && vG.size() == vB.size())
    {
        std::cout << "SWCloud vPX, vPY, vPZ, vR vG vB constructor -> the cloud is null. " << std::endl;
        m_aFCoords   	     = NULL;
        m_aUi8Colors         = NULL;
        m_ui32NumberOfPoints = 0;
        m_ui32ArraySize	     = 0;
    }
	else
	{
        std::cerr << "SWCloud vPX, vPY, vPZ vR vG vB constructor -> bad parameters, array don't have all the same size. " << std::endl;
		
		m_aFCoords   	     = NULL;
		m_aUi8Colors 	     = NULL;
		m_ui32NumberOfPoints = 0;
		m_ui32ArraySize	     = 0;

		throw cloudBadDataError();
	}
}

SWCloud::SWCloud(cfloat fPX, cfloat fPY, cfloat fPZ, cuint8 ui8R, cuint8 ui8G, cuint8 ui8B) :
                 m_ui32NumberOfPoints(0), m_ui32ArraySize(0), m_aFCoords(NULL), m_aUi8Colors(NULL)
{
	++m_i32NumberOfCreatedClouds;
	
	m_ui32NumberOfPoints = 27;
	m_ui32ArraySize      = m_ui32NumberOfPoints*3;
	
	m_aFCoords   = new float[m_ui32ArraySize];
	m_aUi8Colors = new uint8[m_ui32ArraySize];	

	float l_fOffsetX = 0.0005f;
	float l_fOffsetY = 0.0005f;
	float l_fOffsetZ = 0.0005f;
	
	for(uint ii = 0; ii < 27; ++ii)
	{
		m_aFCoords[0*m_ui32NumberOfPoints + ii] = fPX - l_fOffsetX + (ii/9)   * l_fOffsetX;
		m_aFCoords[1*m_ui32NumberOfPoints + ii] = fPY - l_fOffsetY + (ii%9/3) * l_fOffsetY;
		m_aFCoords[2*m_ui32NumberOfPoints + ii] = fPZ - l_fOffsetZ + (ii%3)   * l_fOffsetZ;

		m_aUi8Colors[0*m_ui32NumberOfPoints + ii] = ui8R;
		m_aUi8Colors[1*m_ui32NumberOfPoints + ii] = ui8G;
		m_aUi8Colors[2*m_ui32NumberOfPoints + ii] = ui8B;		
    }
}

SWCloud::SWCloud(const SWCloud &oCloud) : m_ui32NumberOfPoints(0), m_ui32ArraySize(0), m_aFCoords(NULL), m_aUi8Colors(NULL)
{
    ++m_i32NumberOfCreatedClouds;
    copy(oCloud);
}

SWCloud &SWCloud::operator=(const SWCloud &oCloud)
{
    ++m_i32NumberOfCreatedClouds;
    copy(oCloud);
    return *this;
}

SWCloud::~SWCloud(void)
{	
	++m_i32NumberOfDestroyedClouds;
	erase(); // delete current data
}

// ############################################# METHODS - SWCloud


SWCloud& SWCloud::operator+=(const std::vector<float> &oPoint) // TODO : it can't be working in this state
{
    if(oPoint.size() == 3)
	{
		for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
		{
			m_aFCoords[0*m_ui32NumberOfPoints + ii] += oPoint[0];
			m_aFCoords[1*m_ui32NumberOfPoints + ii] += oPoint[1];
            m_aFCoords[2*m_ui32NumberOfPoints + ii] += oPoint[2];
		}
	}
	else
	{
        cerr << "SWCloud += vector<float> -> bad parameter, the size of the vector must be 3 " << endl;
		
		throw cloudBadDataError();
	}
	
	return *this;
}


SWCloud& SWCloud::operator+=(const SWCloud &oCloud)
{	
	if(oCloud.size() > 0)
	{	
		if(m_ui32ArraySize < 3 * size() + 3*oCloud.size())
		{
			upSize(3 * size() + 3*oCloud.size() - m_ui32ArraySize);
		}

		float *l_aFBuffer = new float[2*size()];
		memcpy(l_aFBuffer, m_aFCoords + size(), 2 * size() * sizeof(float) );
		
		memcpy(m_aFCoords + size(), oCloud.coord(0), oCloud.size() * sizeof(float) );
		memcpy(m_aFCoords + size() + oCloud.size(), l_aFBuffer, size() * sizeof(float) );
		memcpy(m_aFCoords + 2 * size() + oCloud.size(), oCloud.coord(1), oCloud.size() * sizeof(float) );
		memcpy(m_aFCoords + 2 * size() + 2 * oCloud.size(), l_aFBuffer + size(),  size() * sizeof(float) );
		memcpy(m_aFCoords + 3 * size() + 2 * oCloud.size(),  oCloud.coord(2), oCloud.size() * sizeof(float) );

		delete[] l_aFBuffer;
		
		
		uint8 *l_aUi8Buffer = new uint8[2*size()];
		memcpy(l_aUi8Buffer, m_aUi8Colors + size(), 2 * size() * sizeof(uint8) );
		
		memcpy(m_aUi8Colors + size(), oCloud.color(0), oCloud.size() * sizeof(uint8) );
		memcpy(m_aUi8Colors + size() + oCloud.size(), l_aUi8Buffer, size() * sizeof(uint8) );
		memcpy(m_aUi8Colors + 2 * size() + oCloud.size(), oCloud.color(1), oCloud.size() * sizeof(uint8) );
		memcpy(m_aUi8Colors + 2 * size() + 2 * oCloud.size(), l_aUi8Buffer + size(),  size() * sizeof(uint8) );
		memcpy(m_aUi8Colors + 3 * size() + 2 * oCloud.size(),  oCloud.color(2), oCloud.size() * sizeof(uint8) );	
		
		delete[] l_aUi8Buffer;
		
		m_ui32NumberOfPoints += oCloud.size();
	}
	
    return *this;
}

SWCloud &SWCloud::operator *=(cfloat fScaleValue)
{
    for(uint ii = 0; ii < size(); ++ii)
    {
        coord(0)[ii] *= fScaleValue;
        coord(1)[ii] *= fScaleValue;
        coord(2)[ii] *= fScaleValue;
    }

    return *this;
}


bool SWCloud::loadObj(const string &sPathObjFile)
{
    bool l_bEndFile = false;
    string l_sType = "";

    vector<float> l_vFX, l_vFY, l_vFZ;
    vector<uint8> l_vUI8R, l_vUI8G, l_vUI8B;

    ifstream  l_oFileStream(sPathObjFile);

    if (l_oFileStream.is_open())
    {                                
        while(!l_bEndFile)
        {
            l_oFileStream >> l_sType;

            if(l_sType.size() > 0)
            {
                if(l_sType.at(0) == '#' || l_sType == "vn" || l_sType == "vt" || l_sType == "f")
                {
                    string l_sLine;
                    getline(l_oFileStream, l_sLine);
                    l_sType = "";
                }
                else if(l_sType == "v")
                {
                    int l_i32Pos = static_cast<int>(l_oFileStream.tellg());

                    string l_sLine;
                    getline(l_oFileStream, l_sLine);

                    // count white spaces
                    std::vector<uint> l_vUI32SpacePos;
                    for(uint ii = 0; ii < l_sLine.size(); ++ii)
                    {
                        if(l_sLine[ii] == ' ')
                        {
                            l_vUI32SpacePos.push_back(ii);
                        }
                    }

                    if(l_vUI32SpacePos.size() != 3 && l_vUI32SpacePos.size() != 6)
                    {
                        std::cerr << "Obj file not valid (SWCloud::loadObj) " << std::endl;
                        return false;
                    }

                    l_oFileStream.seekg(l_i32Pos);

                    // add vertices
                    float l_fX, l_fY, l_fZ;                    
                    l_oFileStream >> l_fX;
                    l_oFileStream >> l_fY;
                    l_oFileStream >> l_fZ;

                    l_vFX.push_back(l_fX);
                    l_vFY.push_back(l_fY);
                    l_vFZ.push_back(l_fZ);

                    // add colors
                    float l_fR, l_fG, l_fB;
                    if(l_vUI32SpacePos.size() == 6)
                    {
                        l_oFileStream >> l_fR;
                        l_oFileStream >> l_fG;
                        l_oFileStream >> l_fB;

                        l_vUI8R.push_back(static_cast<uint8>(l_fR * 255));
                        l_vUI8G.push_back(static_cast<uint8>(l_fG * 255));
                        l_vUI8B.push_back(static_cast<uint8>(l_fB * 255));
                    }

                    l_sType = "";
                }
            }
            else
            {
                l_oFileStream.close();
                l_bEndFile = true;
            }
        }
    }
    else
    {
        cerr << "Can't open obj file (SWCloud::loadObj). " << endl;
        return false;
    }

    if(l_vUI8B.size() == l_vFX.size())
    {
//        std::cout << " aa " << l_vUI8R.size() <<" " << l_vUI8R[10] << " " < << std::endl;
//        for(int ii = 0; ii < l_vUI8R.size(); ++ii)
//            std::cout << static_cast<int>(l_vUI8R[ii]) << " " << static_cast<int>(l_vUI8G[ii]) << " " << static_cast<int>(l_vUI8B[ii]) << " | ";
        set(l_vFX, l_vFY, l_vFZ, l_vUI8R, l_vUI8G, l_vUI8B);
    }
    else
    {
        set(l_vFX, l_vFY, l_vFZ);
    }

    return true;
}

uint SWCloud::size() const
{
	return m_ui32NumberOfPoints;
}

uint SWCloud::arraySize() const
{
    return m_ui32ArraySize;
}

void SWCloud::point(float *a3FXYZ, cuint ui32IndexPoint) const
{
    a3FXYZ[0] = coord(0)[ui32IndexPoint];
    a3FXYZ[1] = coord(1)[ui32IndexPoint];
    a3FXYZ[2] = coord(2)[ui32IndexPoint];
}

void SWCloud::upSize(cuint ui32SizeToAdd)
{
	ulong l_ulNewSize;

	if(ui32SizeToAdd == 0)
	{
		l_ulNewSize = m_ui32ArraySize * 2;
	}
	else
	{
		l_ulNewSize = m_ui32ArraySize + ui32SizeToAdd;
	}
	
	float* l_aFNewCoords = new float[l_ulNewSize];
	uint8* l_aUi8NewColors = new uint8[l_ulNewSize];

	memcpy( l_aFNewCoords,   m_aFCoords,   m_ui32ArraySize * sizeof(float) );
	memcpy( l_aUi8NewColors, m_aUi8Colors, m_ui32ArraySize * sizeof(uint8) );

	m_ui32ArraySize = l_ulNewSize;
	
	delete[] m_aFCoords;
	delete[] m_aUi8Colors;

	m_aFCoords   = l_aFNewCoords;
	m_aUi8Colors = l_aUi8NewColors;
}

float *SWCloud::coord(cuint ui32IdCord) const
{
	if(ui32IdCord >= 0 && ui32IdCord < 3)
	{
		return &m_aFCoords[ui32IdCord * m_ui32NumberOfPoints];
	}
	
	return &m_aFCoords[0];
}


uint8 *SWCloud::color(cuint ui32IdColor) const
{
	if(ui32IdColor >= 0 && ui32IdColor < 3)
	{
		return &m_aUi8Colors[ui32IdColor * m_ui32NumberOfPoints];
	}
	
	return &m_aUi8Colors[0];	
}

void SWCloud::set(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ)
{
	if(vPX.size() == vPY.size() && vPY.size() == vPZ.size())
	{			
		erase(); // delete current data
		
        m_ui32NumberOfPoints = static_cast<uint>(vPX.size());
		m_ui32ArraySize      = m_ui32NumberOfPoints * 3;
		
		m_aFCoords   = new float[m_ui32ArraySize];
		m_aUi8Colors = new uint8[m_ui32ArraySize];
		
		for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
		{
			m_aFCoords[0*m_ui32NumberOfPoints + ii] = vPX[ii];
			m_aFCoords[1*m_ui32NumberOfPoints + ii] = vPY[ii];
			m_aFCoords[2*m_ui32NumberOfPoints + ii] = vPZ[ii];
			
			m_aUi8Colors[0*m_ui32NumberOfPoints + ii] = 255;
			m_aUi8Colors[1*m_ui32NumberOfPoints + ii] = 0;
			m_aUi8Colors[2*m_ui32NumberOfPoints + ii] = 0;
		}
	}
	else
	{
		cerr << "SWCloud set vPX vPY vPZ -> bad parameter, the size of the vector must be the same" << endl;		
		throw cloudBadDataError();		
	}
}


void SWCloud::set(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ,
                  const std::vector<uint8> &vR, const std::vector<uint8> &vG, const std::vector<uint8> &vB)
{
    if(vPX.size() == vPY.size() && vPY.size() == vPZ.size())
    {
        erase(); // delete current data

        m_ui32NumberOfPoints = static_cast<uint>(vPX.size());
        m_ui32ArraySize      = m_ui32NumberOfPoints * 3;

        m_aFCoords   = new float[m_ui32ArraySize];
        m_aUi8Colors = new uint8[m_ui32ArraySize];

        for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
        {
            m_aFCoords[0*m_ui32NumberOfPoints + ii] = vPX[ii];
            m_aFCoords[1*m_ui32NumberOfPoints + ii] = vPY[ii];
            m_aFCoords[2*m_ui32NumberOfPoints + ii] = vPZ[ii];

            m_aUi8Colors[0*m_ui32NumberOfPoints + ii] = vR[ii];
            m_aUi8Colors[1*m_ui32NumberOfPoints + ii] = vG[ii];
            m_aUi8Colors[2*m_ui32NumberOfPoints + ii] = vB[ii];
        }
    }
    else
    {
        cerr << "SWCloud set vPX vPY vPZ -> bad parameter, the size of the vector must be the same" << endl;
        throw cloudBadDataError();
    }
}

void SWCloud::set(cuint ui32NumberOfPoints, float *aFCoords, uint8 *aUi8Colors)
{	
	erase(); // delete current data
	
	// init new data
	m_ui32NumberOfPoints = ui32NumberOfPoints;
	m_ui32ArraySize      = 3*m_ui32NumberOfPoints;
	
	m_aFCoords   = aFCoords;
	m_aUi8Colors = aUi8Colors;
}

void SWCloud::setUnicolor(cuint8 ui8R, cuint8 ui8G, cuint8 ui8B)
{
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		m_aUi8Colors[0*m_ui32NumberOfPoints + ii] = ui8R;
		m_aUi8Colors[1*m_ui32NumberOfPoints + ii] = ui8G;
		m_aUi8Colors[2*m_ui32NumberOfPoints + ii] = ui8B;
	}
}

void SWCloud::copy(const SWCloud &oCloud)
{
	erase(); // delete current data
	
	// init new data
	m_ui32NumberOfPoints = oCloud.size();
	m_ui32ArraySize = 3*m_ui32NumberOfPoints;
	
    m_aFCoords    = new float[3*m_ui32NumberOfPoints];
    m_aUi8Colors  = new uint8[3*m_ui32NumberOfPoints];
	
	float *l_uifX = &m_aFCoords[m_ui32NumberOfPoints*0];
	float *l_uifY = &m_aFCoords[m_ui32NumberOfPoints*1];
	float *l_uifZ = &m_aFCoords[m_ui32NumberOfPoints*2];
	
	uint8 *l_ui8R = &m_aUi8Colors[m_ui32NumberOfPoints*0];
	uint8 *l_ui8G = &m_aUi8Colors[m_ui32NumberOfPoints*1];
	uint8 *l_ui8B = &m_aUi8Colors[m_ui32NumberOfPoints*2];	

	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		l_uifX[ii] = oCloud.coord(0)[ii];
		l_uifY[ii] = oCloud.coord(1)[ii];
		l_uifZ[ii] = oCloud.coord(2)[ii];
		
		l_ui8R[ii] = oCloud.color(0)[ii];
		l_ui8G[ii] = oCloud.color(1)[ii];
		l_ui8B[ii] = oCloud.color(2)[ii];
    }
}


void SWCloud::copy(cuint ui32NumberOfPoints, cfloat *aCoords, cuint8 *aUi8Colors)
{
	erase(); // delete current data
	
	// init new data
	m_ui32NumberOfPoints = ui32NumberOfPoints;
	m_ui32ArraySize = 3*m_ui32NumberOfPoints;
	
	m_aFCoords =   new float[3*m_ui32NumberOfPoints];
	m_aUi8Colors = new uint8[3*m_ui32NumberOfPoints];
	
	float *l_uifX = &m_aFCoords[m_ui32NumberOfPoints*0];
	float *l_uifY = &m_aFCoords[m_ui32NumberOfPoints*1];
	float *l_uifZ = &m_aFCoords[m_ui32NumberOfPoints*2];
	
	uint8 *l_ui8R = &m_aUi8Colors[m_ui32NumberOfPoints*0];
	uint8 *l_ui8G = &m_aUi8Colors[m_ui32NumberOfPoints*1];
	uint8 *l_ui8B = &m_aUi8Colors[m_ui32NumberOfPoints*2];
	
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		l_uifX[ii] = aCoords[m_ui32NumberOfPoints*0 + ii];
		l_uifY[ii] = aCoords[m_ui32NumberOfPoints*1 + ii];
		l_uifZ[ii] = aCoords[m_ui32NumberOfPoints*2 + ii];
		
		l_ui8R[ii] = aUi8Colors[m_ui32NumberOfPoints*0 + ii];
		l_ui8G[ii] = aUi8Colors[m_ui32NumberOfPoints*1 + ii];
		l_ui8B[ii] = aUi8Colors[m_ui32NumberOfPoints*2 + ii];
	}
}

void SWCloud::copy(cuint ui32NumberOfPoints, const float *aCoords, cuint8 ui8ColorR, cuint8 ui8ColorG, cuint8 ui8ColorB)
{
	erase(); // delete current data
	
	// init new data
	m_aFCoords =   new float[3*ui32NumberOfPoints];
	m_aUi8Colors = new uint8[3*ui32NumberOfPoints];
	
	m_ui32NumberOfPoints = ui32NumberOfPoints;
	m_ui32ArraySize      = 3*m_ui32NumberOfPoints;
	
	float *l_uifX = &m_aFCoords[m_ui32NumberOfPoints*0];
	float *l_uifY = &m_aFCoords[m_ui32NumberOfPoints*1];
	float *l_uifZ = &m_aFCoords[m_ui32NumberOfPoints*2];
	
	uint8 *l_ui8R = &m_aUi8Colors[m_ui32NumberOfPoints*0];
	uint8 *l_ui8G = &m_aUi8Colors[m_ui32NumberOfPoints*1];
	uint8 *l_ui8B = &m_aUi8Colors[m_ui32NumberOfPoints*2];
	
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		l_uifX[ii] = aCoords[m_ui32NumberOfPoints*0 + ii];
		l_uifY[ii] = aCoords[m_ui32NumberOfPoints*1 + ii];
		l_uifZ[ii] = aCoords[m_ui32NumberOfPoints*2 + ii];
		
		l_ui8R[ii] = ui8ColorR;
		l_ui8G[ii] = ui8ColorG;
		l_ui8B[ii] = ui8ColorB;
	}
}

void SWCloud::erase()
{
    deleteAndNullifyArray(m_aFCoords);
    deleteAndNullifyArray(m_aUi8Colors);

	m_ui32NumberOfPoints = 0;
	m_ui32ArraySize      = 0;
}


bool SWCloud::reduce(float fRandomSamplingPercentage, float fMinDistBeforeReduction)
{
    if(fRandomSamplingPercentage < 0.f || fMinDistBeforeReduction < 0)
    {
        cerr << "Error reduce SWCloud : bad parameters ." << endl;
        return false;
    }

    if(fRandomSamplingPercentage > 1.f)
    {
        // do nothing
        return true;
    }
	
	float l_fMinDist = FLT_MAX;

	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		if(coord(2)[ii] < l_fMinDist)
		{			
			l_fMinDist = coord(2)[ii];
		}
	}
	
	#ifndef WIN32
		srand48((long)time(NULL));
	#else
		// srand((long)time(NULL));
	#endif

    int l_i32NumberOfRandomlySampledPoints = 0;

	int *l_aI32Flag = new int[m_ui32NumberOfPoints];
	
	// randomly select points
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		#ifndef WIN32

			if((coord(2)[ii] < l_fMinDist + fMinDistBeforeReduction) || (drand48() < fRandomSamplingPercentage))
			{
				l_aI32Flag[ii] = 1;
				l_i32NumberOfRandomlySampledPoints++;
			}
			else
			{
				l_aI32Flag[ii] = 0;
			}
		#else
			if((coord(2)[ii] < l_fMinDist + fMinDistBeforeReduction) || (rand()/(double)RAND_MAX < fRandomSamplingPercentage))
			{
				l_aI32Flag[ii] = 1;
				l_i32NumberOfRandomlySampledPoints++;
			}
			else
			{
				l_aI32Flag[ii] = 0;
			}
		#endif
	}
	
	// init new data
	float *l_aFNewCoords   = new float[l_i32NumberOfRandomlySampledPoints*3];
	uint8 *l_aUi8NewColors = new uint8[l_i32NumberOfRandomlySampledPoints*3];
	
	uint l_ui32AddedPoints = 0;
	
	// add selected points
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		if(l_aI32Flag[ii])
		{
			l_aFNewCoords[l_i32NumberOfRandomlySampledPoints * 0 + l_ui32AddedPoints] = coord(0)[ii];
			l_aFNewCoords[l_i32NumberOfRandomlySampledPoints * 1 + l_ui32AddedPoints] = coord(1)[ii];
			l_aFNewCoords[l_i32NumberOfRandomlySampledPoints * 2 + l_ui32AddedPoints] = coord(2)[ii];
			
			l_aUi8NewColors[l_i32NumberOfRandomlySampledPoints * 0 + l_ui32AddedPoints] = color(0)[ii];
			l_aUi8NewColors[l_i32NumberOfRandomlySampledPoints * 1 + l_ui32AddedPoints] = color(1)[ii];
			l_aUi8NewColors[l_i32NumberOfRandomlySampledPoints * 2 + l_ui32AddedPoints] = color(2)[ii];
			
			l_ui32AddedPoints++;
		}
	}

	// delete data
	delete[] l_aI32Flag;
	delete[] m_aFCoords;
	delete[] m_aUi8Colors;
	
	// assign new data
	m_aFCoords = l_aFNewCoords;
	m_aUi8Colors = l_aUi8NewColors;
	m_ui32NumberOfPoints = l_i32NumberOfRandomlySampledPoints;
	m_ui32ArraySize      = 3*m_ui32NumberOfPoints;
	
	return true;
}

bool SWCloud::transform(cfloat *m_aFRotationMatrix, cfloat *m_aFTranslationMatrix)
{
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
        float l_fNewX,l_fNewY,l_fNewZ;

        l_fNewX =   m_aFRotationMatrix[0] * coord(0)[ii] +
                    m_aFRotationMatrix[1] * coord(1)[ii] +
                    m_aFRotationMatrix[2] * coord(2)[ii] + m_aFTranslationMatrix[0];
		
        l_fNewY =   m_aFRotationMatrix[3] * coord(0)[ii] +
                    m_aFRotationMatrix[4] * coord(1)[ii] +
                    m_aFRotationMatrix[5] * coord(2)[ii] + m_aFTranslationMatrix[1];
		
        l_fNewZ =   m_aFRotationMatrix[6] * coord(0)[ii] +
                    m_aFRotationMatrix[7] * coord(1)[ii] +
                    m_aFRotationMatrix[8] * coord(2)[ii] + m_aFTranslationMatrix[2];

        m_aFCoords[m_ui32NumberOfPoints*0 +ii] = l_fNewX;
        m_aFCoords[m_ui32NumberOfPoints*1 +ii] = l_fNewY;
        m_aFCoords[m_ui32NumberOfPoints*2 +ii] = l_fNewZ;
	}
	
	return true;
}


float SWCloud::squareDistancePoint(const SWCloud &oCloud, cuint ui32IndiceP)
{
	float l_fX = oCloud.coord(0)[ui32IndiceP];
	float l_fY = oCloud.coord(1)[ui32IndiceP];
	float l_fZ = oCloud.coord(2)[ui32IndiceP];
	
	float l_fCurrentDist, l_fMinDist = FLT_MAX;
	float l_fDX, l_fDY, l_fDZ;
	
	for(uint ii = 0; ii < size(); ++ii)
	{
		l_fDX = coord(0)[ii] - l_fX;
		l_fDY = coord(1)[ii] - l_fY;
		l_fDZ = coord(2)[ii] - l_fZ;
		
		l_fCurrentDist = l_fDX*l_fDX + l_fDY*l_fDY + l_fDZ*l_fDZ;
		
		if(l_fCurrentDist < l_fMinDist)
		{
			l_fMinDist = l_fCurrentDist;
		}
	}	
	
	return l_fMinDist;
}

float SWCloud::squareDistanceCloud(const SWCloud &oCloud, cbool bReduce, cfloat ui32CoeffReduce)
{
	SWCloud l_oCloud;
	l_oCloud.copy(oCloud);
	
	float l_squareDistance = 0;
	
	if(bReduce)
	{
		l_oCloud.reduce(ui32CoeffReduce);
	}
	
	for(uint ii = 0; ii < l_oCloud.size(); ++ii)
	{
		l_squareDistance += squareDistancePoint(l_oCloud,ii);
	}
	
	return l_squareDistance/l_oCloud.size();
}

std::vector<float> SWCloud::moveToOrigine()
{
    std::vector<float> l_v3fMeanVector = meanPoint();

    float *l_uifX = &m_aFCoords[m_ui32NumberOfPoints*0];
    float *l_uifY = &m_aFCoords[m_ui32NumberOfPoints*1];
    float *l_uifZ = &m_aFCoords[m_ui32NumberOfPoints*2];

    for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
    {
        l_uifX[ii] -= l_v3fMeanVector[0];
        l_uifY[ii] -= l_v3fMeanVector[1];
        l_uifZ[ii] -= l_v3fMeanVector[2];
    }

    return l_v3fMeanVector;
}

std::vector<float> SWCloud::meanPoint() const
{
    if(m_ui32NumberOfPoints == 0)
    {
        return std::vector<float>(3,0.f);
    }

    float l_fMeanX = 0, l_fMeanY = 0, l_fMeanZ = 0;

    float *l_uifX = &m_aFCoords[m_ui32NumberOfPoints*0];
    float *l_uifY = &m_aFCoords[m_ui32NumberOfPoints*1];
    float *l_uifZ = &m_aFCoords[m_ui32NumberOfPoints*2];

    for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
    {
        l_fMeanX += l_uifX[ii];
        l_fMeanY += l_uifY[ii];
        l_fMeanZ += l_uifZ[ii];
    }

    l_fMeanX /= m_ui32NumberOfPoints;
    l_fMeanY /= m_ui32NumberOfPoints;
    l_fMeanZ /= m_ui32NumberOfPoints;    

    std::vector<float> l_v3fMeanVector;
    l_v3fMeanVector.push_back(l_fMeanX);
    l_v3fMeanVector.push_back(l_fMeanY);
    l_v3fMeanVector.push_back(l_fMeanZ);

    return l_v3fMeanVector;
}

bool SWCloud::retrieveCloudPart(SWCloud &oCloudPart, cuint32 ui32BeginIndexPoint, cuint32 ui32EndIndexPoint)
{
	uint l_ui32IndexBegin, l_ui32IndexEnd, ui32NumberOfPoints;
	
	if(ui32EndIndexPoint == 0)
	{
		l_ui32IndexBegin = ui32BeginIndexPoint;
		l_ui32IndexEnd   = size();
	}
	else if(ui32BeginIndexPoint < ui32EndIndexPoint)
	{
		l_ui32IndexBegin = ui32BeginIndexPoint;
		l_ui32IndexEnd   = ui32EndIndexPoint;
	}
	else
	{
		return false;
	}
	
	ui32NumberOfPoints = (l_ui32IndexEnd - l_ui32IndexBegin);
	
	// init new data
	float *l_aFNewCoords   = new float[ui32NumberOfPoints*3];
	uint8 *l_aUi8NewColors = new uint8[ui32NumberOfPoints*3];	
	
	// add selected points
	for(uint ii = l_ui32IndexBegin, jj = 0; ii < l_ui32IndexEnd; ++ii, ++jj)
	{
		l_aFNewCoords[ui32NumberOfPoints * 0 + jj] = coord(0)[ii];
		l_aFNewCoords[ui32NumberOfPoints * 1 + jj] = coord(1)[ii];
		l_aFNewCoords[ui32NumberOfPoints * 2 + jj] = coord(2)[ii];
		
		l_aUi8NewColors[ui32NumberOfPoints * 0 + jj] = color(0)[ii];
		l_aUi8NewColors[ui32NumberOfPoints * 1 + jj] = color(1)[ii];
		l_aUi8NewColors[ui32NumberOfPoints * 2 + jj] = color(2)[ii];
	}	

	oCloudPart.set(ui32NumberOfPoints, l_aFNewCoords, l_aUi8NewColors);
	
    return true;
}

void SWCloud::keepOnlyPointInsideBBox(const SWCloudBBox &oCloudBBox)
{
    std::vector<float> l_vX, l_vY, l_vZ;
    std::vector<uint8> l_vR, l_vG, l_vB;

    for(uint ii = 0; ii < size(); ++ii)
    {
        if(oCloudBBox.isInside(coord(0)[ii], coord(1)[ii], coord(2)[ii]))
        {
            l_vX.push_back(coord(0)[ii]);
            l_vY.push_back(coord(1)[ii]);
            l_vZ.push_back(coord(2)[ii]);

            l_vR.push_back(color(0)[ii]);
            l_vG.push_back(color(1)[ii]);
            l_vB.push_back(color(2)[ii]);
        }
    }

    erase();

    SWCloud l_oCutCloud(l_vX , l_vY, l_vZ, l_vR, l_vG, l_vB);
    copy(l_oCutCloud);
}

void SWCloud::lowerBBoxPoint(float &fMinX, float &fMinY, float &fMinZ) const
{
    if(size() == 0)
    {
        fMinX = 0.f;
        fMinY = 0.f;
        fMinZ = 0.f;
    }

	fMinX = FLT_MAX;
	fMinY = FLT_MAX;
	fMinZ = FLT_MAX;
	
	for(uint ii = 0; ii < size(); ++ii)
	{
		if(coord(0)[ii] < fMinX)
		{
			fMinX = coord(0)[ii];
		}
		if(coord(1)[ii] < fMinY)
		{
			fMinY = coord(1)[ii];
		}
		if(coord(2)[ii] < fMinZ)
		{
			fMinZ = coord(2)[ii];
		}
	}
}


void SWCloud::upperBBoxPoint(float &fMaxX, float &fMaxY, float &fMaxZ) const
{
    if(size() == 0)
    {
        fMaxX = 0.f;
        fMaxY = 0.f;
        fMaxZ = 0.f;
    }

	fMaxX = -FLT_MAX;
	fMaxY = -FLT_MAX;
	fMaxZ = -FLT_MAX;
	
	for(uint ii = 0; ii < size(); ++ii)
	{
		if(coord(0)[ii] > fMaxX)
		{
			fMaxX = coord(0)[ii];
		}
		if(coord(1)[ii] > fMaxY)
		{
			fMaxY = coord(1)[ii];
		}
		if(coord(2)[ii] > fMaxZ)
		{
			fMaxZ = coord(2)[ii];
		}
	}
}


SWCloudBBox SWCloud::bBox() const
{
	SWCloudBBox l_oBBox;
	
	lowerBBoxPoint(l_oBBox.m_fMinX, l_oBBox.m_fMinY, l_oBBox.m_fMinZ);
	upperBBoxPoint(l_oBBox.m_fMaxX, l_oBBox.m_fMaxY, l_oBBox.m_fMaxZ);
	
	return l_oBBox;
}

void SWCloud::cloudBBox(const SWCloudBBox &oBBox, SWCloud &oCloudBBox, cuint i32RidgeNumberOfPoints) 
{
	oCloudBBox.erase(); // delete current data
		
	vector<float> l_vFX, l_vFY, l_vFZ;
	
	// min X -> max X
	float l_fDistX = (oBBox.m_fMaxX - oBBox.m_fMinX)*(1.f/i32RidgeNumberOfPoints);
	float l_fDistY = (oBBox.m_fMaxY - oBBox.m_fMinY)*(1.f/i32RidgeNumberOfPoints);
	float l_fDistZ = (oBBox.m_fMaxZ - oBBox.m_fMinZ)*(1.f/i32RidgeNumberOfPoints);
	
	for(uint ii = 0; ii < i32RidgeNumberOfPoints; ++ii)
	{
		// minX -> maxX
		l_vFX.push_back(oBBox.m_fMinX + ii*l_fDistX);
		l_vFY.push_back(oBBox.m_fMinY);
		l_vFZ.push_back(oBBox.m_fMinZ);
		
		l_vFX.push_back(oBBox.m_fMinX + ii*l_fDistX);
		l_vFY.push_back(oBBox.m_fMaxY);
		l_vFZ.push_back(oBBox.m_fMinZ);

		l_vFX.push_back(oBBox.m_fMinX + ii*l_fDistX);
		l_vFY.push_back(oBBox.m_fMaxY);
		l_vFZ.push_back(oBBox.m_fMaxZ);

		l_vFX.push_back(oBBox.m_fMinX + ii*l_fDistX);
		l_vFY.push_back(oBBox.m_fMinY);
		l_vFZ.push_back(oBBox.m_fMaxZ);			
		
		// minY -> maxY
		l_vFX.push_back(oBBox.m_fMinX);
		l_vFY.push_back(oBBox.m_fMinY + ii*l_fDistY);
		l_vFZ.push_back(oBBox.m_fMinZ);
		
		l_vFX.push_back(oBBox.m_fMaxX);
		l_vFY.push_back(oBBox.m_fMinY + ii*l_fDistY);
		l_vFZ.push_back(oBBox.m_fMinZ);

		l_vFX.push_back(oBBox.m_fMaxX);
		l_vFY.push_back(oBBox.m_fMinY + ii*l_fDistY);
		l_vFZ.push_back(oBBox.m_fMaxZ);

		l_vFX.push_back(oBBox.m_fMinX);
		l_vFY.push_back(oBBox.m_fMinY + ii*l_fDistY);
		l_vFZ.push_back(oBBox.m_fMaxZ);

		// minZ -> maxZ
		l_vFX.push_back(oBBox.m_fMinX);
		l_vFY.push_back(oBBox.m_fMinY);
		l_vFZ.push_back(oBBox.m_fMinZ + ii*l_fDistZ);
		
		l_vFX.push_back(oBBox.m_fMaxX);
		l_vFY.push_back(oBBox.m_fMinY);
		l_vFZ.push_back(oBBox.m_fMinZ + ii*l_fDistZ);

		l_vFX.push_back(oBBox.m_fMaxX);
		l_vFY.push_back(oBBox.m_fMaxY);
		l_vFZ.push_back(oBBox.m_fMinZ + ii*l_fDistZ);

		l_vFX.push_back(oBBox.m_fMinX);
		l_vFY.push_back(oBBox.m_fMaxY);
		l_vFZ.push_back(oBBox.m_fMinZ + ii*l_fDistZ);
	}
	
	for(uint ii = 0; ii < i32RidgeNumberOfPoints; ++ii)
	{
		// axe Z, middlePoint
		l_vFX.push_back(oBBox.m_fMinX + (oBBox.m_fMaxX - oBBox.m_fMinX)/2.f);
		l_vFY.push_back(oBBox.m_fMinY + (oBBox.m_fMaxY - oBBox.m_fMinY)/2.f);
		l_vFZ.push_back(oBBox.m_fMinZ + (oBBox.m_fMaxZ - oBBox.m_fMinZ)/2.f + ii*(min(l_fDistX, min(l_fDistZ, l_fDistY))/4.f));

		// axe Y, middlePoint
		l_vFX.push_back(oBBox.m_fMinX + (oBBox.m_fMaxX - oBBox.m_fMinX)/2.f);
		l_vFY.push_back(oBBox.m_fMinY + (oBBox.m_fMaxY - oBBox.m_fMinY)/2.f + ii*(min(l_fDistX, min(l_fDistZ, l_fDistY))/4.f));
		l_vFZ.push_back(oBBox.m_fMinZ + (oBBox.m_fMaxZ - oBBox.m_fMinZ)/2.f);		
		
		// axe X, middlePoint
		l_vFX.push_back(oBBox.m_fMinX + (oBBox.m_fMaxX - oBBox.m_fMinX)/2.f + ii*(min(l_fDistX, min(l_fDistZ, l_fDistY))/4.f));
		l_vFY.push_back(oBBox.m_fMinY + (oBBox.m_fMaxY - oBBox.m_fMinY)/2.f);
		l_vFZ.push_back(oBBox.m_fMinZ + (oBBox.m_fMaxZ - oBBox.m_fMinZ)/2.f);				
	}
	
	oCloudBBox.set(l_vFX, l_vFY, l_vFZ);	
}

void SWCloud::cloudBBox(SWCloud &oCloudBBox, cuint i32RidgeNumberOfPoints) 
{	
	oCloudBBox.erase(); // delete current data
	
	SWCloudBBox l_oBBox = bBox(); // compute bbox
		
	cloudBBox(l_oBBox, oCloudBBox, i32RidgeNumberOfPoints);
}

void SWCloud::bBox2DFilter(const SWCloudBBox &oBBox)
{
	vector<float> l_vX, l_vY, l_vZ;
	vector<uint8> l_vR, l_vG, l_vB;
	
	for(uint ii = 0; ii < size(); ++ii)
	{
		if(oBBox.isInside(coord(0)[ii], coord(1)[ii]))
		{
			l_vX.push_back(coord(0)[ii]);
			l_vY.push_back(coord(1)[ii]);
			l_vZ.push_back(coord(2)[ii]);
			
			l_vR.push_back(color(0)[ii]);
			l_vG.push_back(color(1)[ii]);
			l_vB.push_back(color(2)[ii]);			
		}
	}
	
	SWCloud *l_oFilteredCloud = new SWCloud(l_vX, l_vY, l_vZ, l_vR, l_vG, l_vB);
	
	// delete data
	delete[] m_aFCoords;
	delete[] m_aUi8Colors;
	
	// assign new data
	m_aFCoords   = l_oFilteredCloud->coord(0);
	m_aUi8Colors = l_oFilteredCloud->color(0);
	m_ui32NumberOfPoints = l_oFilteredCloud->size();	
    m_ui32ArraySize      = 3 * m_ui32NumberOfPoints;
}

float *SWCloud::vertexBuffer() const
{
    if(size() == 0)
    {
        return NULL;
    }

	float *l_aFVertex = new float[m_ui32ArraySize];
	
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		l_aFVertex[ii*3]     = coord(0)[ii];
		l_aFVertex[ii*3 + 1] = coord(1)[ii];
        l_aFVertex[ii*3 + 2] = coord(2)[ii];
	}

	return l_aFVertex;
}

uint32 *SWCloud::indexBuffer() const
{
    if(size() == 0)
    {
        return NULL;
    }

	uint32 *l_aUI32Index = new uint32[m_ui32NumberOfPoints];
	
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		l_aUI32Index[ii] = ii;
    }
	
	return l_aUI32Index;
}

float *SWCloud::colorBuffer() const
{
    if(size() == 0)
    {
        return NULL;
    }

	float *l_aFColor = new float[m_ui32ArraySize];
	
	for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
	{
		l_aFColor[ii*3]     = color(0)[ii] / 255.f;
		l_aFColor[ii*3 + 1] = color(1)[ii] / 255.f;
        l_aFColor[ii*3 + 2] = color(2)[ii] / 255.f;
    }
	
	return l_aFColor;
}


int SWCloud::idNearestPoint(const std::vector<float> &oPt, cfloat fDistMin) const
{        
    int l_i32IdNearestPoint = 0;
    float l_fMin = FLT_MAX;

    for(uint ii = 0; ii < size(); ++ii)
    {
        float l_fCurrSquareDist = (oPt[0]-coord(0)[ii])*(oPt[0]-coord(0)[ii])+
                                  (oPt[1]-coord(1)[ii])*(oPt[1]-coord(1)[ii])+
                                  (oPt[2]-coord(2)[ii])*(oPt[2]-coord(2)[ii]);
        if(l_fCurrSquareDist < l_fMin)
        {
            bool l_bUpdate = true;

            if(fDistMin != 0.f)
            {
                l_bUpdate = (sqrt(l_fCurrSquareDist) > fDistMin);
            }

            if(l_bUpdate)
            {
                l_fMin = l_fCurrSquareDist;
                l_i32IdNearestPoint = ii;
            }
        }
    }

    return l_i32IdNearestPoint;
}


//void SWCloud::adaptCloud(const SWCloud &oTarget, cint i32NearPointsNb, cfloat fDeviation)
//{
//    // fill nearest id point array
////    vector<int> l_vI32CorrId;

////    for(uint ii = 0; ii < size(); ++ii)
////    {
////        l_vI32CorrId.push_back(idNearestPoint(ii, oTarget));
////    }

////    for(uint ii = 0; ii < l_vI32CorrId.size(); ++ii)
////    {
////        coord(0)[ii] = oTarget.coord(0)[l_vI32CorrId[ii]];
////        coord(1)[ii] = oTarget.coord(1)[l_vI32CorrId[ii]];
////        coord(2)[ii] = oTarget.coord(2)[l_vI32CorrId[ii]];
////    }

//    cout << "Adapt cloud : " << i32NearPointsNb << " " << fDeviation << endl;

//    for(uint ii = 0; ii < size(); ++ii)
//    {
//        vector<float> l_v3FCurrPoint;
//        if(meanNearestPosition(l_v3FCurrPoint, ii, oTarget, i32NearPointsNb, fDeviation) > 0)
//        {
//            coord(0)[ii] = l_v3FCurrPoint[0];
//            coord(1)[ii] = l_v3FCurrPoint[1];
//            coord(2)[ii] = l_v3FCurrPoint[2];
//        }
//    }
//}


//int SWCloud::idNearestPoint(cint i32IdSourcePoint, const SWCloud &oTarget, float fDistMin) const
//{

////    idNearestPoint();


//    int l_i32IdMin = -1;
//    float l_fDistMax = FLT_MAX;

//    for(uint ii = 0; ii < oTarget.size(); ++ii)
//    {
//        float l_fCurrSquareDist =
//                (oTarget.coord(0)[ii] - coord(0)[i32IdSourcePoint]) * (oTarget.coord(0)[ii] - coord(0)[i32IdSourcePoint]) +
//                (oTarget.coord(1)[ii] - coord(1)[i32IdSourcePoint]) * (oTarget.coord(1)[ii] - coord(1)[i32IdSourcePoint]) +
//                (oTarget.coord(2)[ii] - coord(2)[i32IdSourcePoint]) * (oTarget.coord(2)[ii] - coord(2)[i32IdSourcePoint]);

//        if(l_fCurrSquareDist < l_fDistMax && (sqrt(l_fCurrSquareDist) > fDistMin))
//        {
//            l_fDistMax   = l_fCurrSquareDist;
//            l_i32IdMin = ii;
//        }
//    }

//    return l_i32IdMin;
//}

//int SWCloud::meanNearestPosition(vector<float> &oMeanPosition, cint i32IdSourcePoint, const SWCloud &oTarget, int i32NearPointsNumber, cfloat fDistMax) const
//{
//    float l_fDistMin = 0.f;

//    int l_i32NumPoint = 0;
//    oMeanPosition.clear();
//    oMeanPosition.push_back(0.f); oMeanPosition.push_back(0.f); oMeanPosition.push_back(0.f);

//    while(l_i32NumPoint < i32NearPointsNumber)
//    {

//        int l_i32TId = idNearestPoint(i32IdSourcePoint, oTarget, l_fDistMin);

//        float l_fDiffX = (coord(0)[i32IdSourcePoint] - oTarget.coord(0)[l_i32TId]);
//        float l_fDiffY = (coord(1)[i32IdSourcePoint] - oTarget.coord(1)[l_i32TId]);
//        float l_fDiffZ = (coord(2)[i32IdSourcePoint] - oTarget.coord(2)[l_i32TId]);

//        l_fDistMin = sqrt((l_fDiffX * l_fDiffX) + (l_fDiffY * l_fDiffY) + (l_fDiffZ * l_fDiffZ));

//        if(l_fDistMin <  fDistMax)
//        {
//            oMeanPosition[0] += oTarget.coord(0)[l_i32TId];
//            oMeanPosition[1] += oTarget.coord(1)[l_i32TId];
//            oMeanPosition[2] += oTarget.coord(2)[l_i32TId];
//            ++l_i32NumPoint;
//        }
//        else
//        {
//            i32NearPointsNumber = l_i32NumPoint;
//        }
//    }

//    oMeanPosition[0] /= l_i32NumPoint;
//    oMeanPosition[1] /= l_i32NumPoint;
//    oMeanPosition[2] /= l_i32NumPoint;

//    return l_i32NumPoint;

//    // DEBUG
////    cout << "mean : " << oMeanPosition[0] << " " << oMeanPosition[1] << " " << oMeanPosition[2] <<  " " << l_i32NumPoint << endl;
//}



//bool SWCloud::isNearbyPointsIn2DPlan(std::vector<float> &v3FPoint, cfloat fOffsetX, float fOffsetY)
//{
//    if(v3FPoint.size() < 2)
//    {
//        return false;
//    }

//    for(uint ii = 0; ii < size(); ++ii)
//    {
//        if(sqrt((coord(0)[ii] - v3FPoint[0]) * (coord(0)[ii] - v3FPoint[0])) < fOffsetX  &&
//           sqrt((coord(1)[ii] - v3FPoint[1]) * (coord(1)[ii] - v3FPoint[1])) < fOffsetY)
//        {
//            return true;
//        }
//    }

//    return false;
//}

//float SWCloud::computeMeanZPositionOfNearbyPointsIn2DPlan(vector<float> &v3FPoint, cfloat fOffsetX, float fOffsetY)
//{
//    int l_i32NumberOfPoints = 0;
//    float l_fZPos = 0;

//    for(uint ii = 0; ii < size(); ++ii)
//    {
//        if(sqrt((coord(0)[ii] - v3FPoint[0]) * (coord(0)[ii] - v3FPoint[0])) < fOffsetX  &&
//           sqrt((coord(1)[ii] - v3FPoint[1]) * (coord(1)[ii] - v3FPoint[1])) < fOffsetY)
//        {
//            l_fZPos += coord(2)[ii];
//            l_i32NumberOfPoints++;
//        }
//    }

//    return l_fZPos/l_i32NumberOfPoints;
//}
