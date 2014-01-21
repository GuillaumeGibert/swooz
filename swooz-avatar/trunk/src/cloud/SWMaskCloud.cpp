
/**
 * \file SWMaskCloud.cpp
 * \brief defines SWMaskCloud
 * \author Florian Lance
 * \date 06/03/13
 */

#include "cloud/SWMaskCloud.h"

#include <iostream>

using namespace std;
using namespace swCloud;


// ############################################# CONSTRUCTORS / DESTRUCTORS


SWMaskCloud::SWMaskCloud(void) : SWCloud()
{}

SWMaskCloud::SWMaskCloud(const SWCloud &oCloud) : SWCloud(oCloud), m_vI32Mask(vector<int>(oCloud.size(), -1))
{}

SWMaskCloud::SWMaskCloud(const SWCloud &oCloud, const vector<int> &vI32Mask) : SWCloud(oCloud)
{
    if(oCloud.size() == vI32Mask.size())
    {
        m_vI32Mask = vI32Mask;
    }
    else
    {
        cerr << "Error SWMaskCloud SWMaskCloud() : Input cloud and mask array must have the save size (number of points). " << endl;
    }
}

SWMaskCloud::~SWMaskCloud(void)
{}

// ############################################# METHODS


void SWMaskCloud::setMask(const vector<int> &vI32Mask)
{
    if(size() == vI32Mask.size())
    {
        m_vI32Mask = vI32Mask;
    }
    else
    {
        cerr << "Error SWMaskCloud setMask() : mask array must have the save size than the cloud (number of points). " << endl;
    }
}

int SWMaskCloud::mask(cuint ui32IdPoint) const
{
    if(ui32IdPoint < size())
    {
        return m_vI32Mask[ui32IdPoint];
    }
    else
    {
        cerr << "Error SWMaskCloud mask() : input idPoint is not valid. " << endl;
        return -1;
    }
}

void SWMaskCloud::updateMaskColors() // TODO : remove (old)
{
    for(uint ii = 0; ii < size(); ++ii)
    {
        if(m_vI32Mask[ii] > 0)
        {
            color(0)[ii] = 0;
            color(1)[ii] = 255;
            color(2)[ii] = 0;
        }
        else
        {
            color(0)[ii] = 255;
            color(1)[ii] = 0;
            color(2)[ii] = 0;
        }
    }
}



//SWMaskCloud::SWMaskCloud(cuint ui32NumberOfPoint, float *aCoords, uint8 *aUi8Colors, int *aI32Mask) : SWCloud(ui32NumberOfPoint, aCoords, aUi8Colors), m_aI32Mask(aI32Mask)
//{}

//SWMaskCloud::SWMaskCloud(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ, const std::vector<int> &vI32Mask) : SWCloud(vPX, vPY, vPZ)
//{
//	if(vPX.size() == vI32Mask.size())
//	{
//		m_aI32Mask = new int[m_ui32NumberOfPoints];
		
//		for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
//		{
//			m_aI32Mask[ii] = vI32Mask[ii];
//		}
//	}
//	else
//	{
//		cerr << "SWMaskCloud vPX, vPY, vPZ, vI32Mask constructor -> bad parameters, array don't have all the same size or are null. " << endl;
//		m_aI32Mask = NULL;
//	}
//}

//SWMaskCloud::SWMaskCloud(const std::vector<float> &vPX, const std::vector<float> &vPY, const std::vector<float> &vPZ,
//			 const std::vector<uint8> &vR,  const std::vector<uint8> &vG,  const std::vector<uint8> &vB,  const std::vector<int> &vI32Mask)
//			 : SWCloud(vPX, vPY, vPZ, vR, vG, vB)
//{
//	if(vPX.size() == vI32Mask.size())
//	{
//		m_aI32Mask = new int[m_ui32NumberOfPoints];
		
//		for(uint ii = 0; ii < m_ui32NumberOfPoints; ++ii)
//		{
//			m_aI32Mask[ii] = vI32Mask[ii];
//		}
//	}
//	else
//	{
//		cerr << "SWMaskCloud vPX, vPY, vPZ, vI32Mask constructor -> bad parameters, array don't have all the same size or are null. " << endl;
//		m_aI32Mask = NULL;
//	}
//}


//SWMaskCloud::~SWMaskCloud(void)
//{
//	erase();

//	delete[] m_aI32Mask;
//	m_aI32Mask = NULL;
//}



//int *SWMaskCloud::mask() const
//{
//	return &m_aI32Mask[0];
//}

//void SWMaskCloud::setMask(int *aI32Mask)
//{
//	delete[] m_aI32Mask; // delete current data
	
//	// init new data
//	m_aI32Mask = aI32Mask;
//}

//SWMaskCloud& SWMaskCloud::operator+=(const SWMaskCloud &oCloud)
//{
//	uint l_uI32PrevArraySize = m_ui32ArraySize;
//	uint l_uI32PrevSize 	 = size();
	
//	SWCloud::operator+=(oCloud);
	
//	uint l_uI32DiffArraySize = m_ui32ArraySize - l_uI32PrevArraySize;
	 
//	if(l_uI32PrevArraySize == m_ui32ArraySize)
//	{
//		upSizeMask(l_uI32DiffArraySize/3);
//	}
	
//	int *l_aI32Buffer = new int[l_uI32PrevSize + oCloud.size()];
//	memcpy(l_aI32Buffer, m_aI32Mask, l_uI32PrevSize * sizeof(int) );
//	memcpy(l_aI32Buffer + l_uI32PrevSize, oCloud.m_aI32Mask, oCloud.size() * sizeof(int) );
//	memcpy(m_aI32Mask, l_aI32Buffer, (l_uI32PrevSize + oCloud.size()) * sizeof(int) );
	
//	delete[] l_aI32Buffer;
	
//	return *this;
//}

//void SWMaskCloud::upSizeMask(cuint ui32SizeToAdd)
//{
//	ulong l_ulNewSize;

//	if(ui32SizeToAdd == 0)
//	{
//		l_ulNewSize = size() * 2;
//	}
//	else
//	{
//		l_ulNewSize = size() + ui32SizeToAdd;
//	}
	
//	int *l_aI32NewMask = new int[l_ulNewSize];

//	memcpy(l_aI32NewMask, m_aI32Mask, size() * sizeof(int));
	
//	delete[] m_aI32Mask;

//	m_aI32Mask = l_aI32NewMask;
//}


