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
 * \file FaceLab.cpp
 * \author Simon Robert
 * \date 03-05-2012
 * \brief The file containing the FaceLAB module template
 */

#include "devices/faceLab/FaceLab.h"

using namespace std;
using namespace sm::eod;
using namespace sm::eod::io;
using namespace sm::eod::utils;
using namespace FaceLabTracker;


CFaceLab::CFaceLab(int port)
{
	m_iPortNumber = port;
	m_bListening = false;
	m_lLastUpdate = 0;
}


bool CFaceLab::init()
{
	startListening();
	return true;
}


bool CFaceLab::close()
{
	//Closing the listening loop
	m_bListening = false;

	return true;
}


HeadGazeData CFaceLab::grabData()
{
	return toStructure(*m_pHeadGazeEngineData);
}

void CFaceLab::listen()
{
	try 
    {
        DatagramSocket l_oInputSocket(m_iPortNumber);
		printf("\n>INFO: Listening on %s\n", l_oInputSocket.getLocalAddress().toString().c_str());
		
		//The listening attribute allows spontaneous stopping
		while (m_bListening)
		{
			handleDatagram(l_oInputSocket);
		}
		
		l_oInputSocket.disconnectSocket();
		m_pListeningThread->join();
    }
    catch (SocketError &e)
    {
        printf("\n-ERROR: Socket Error: %s\n", e.what());
    }
    catch (SerializableError &e)
    {
        printf("\n-ERROR: Serialization Error: %s\n", e.what());
    }
    catch (std::exception &e)
    {
        printf("\n-ERROR: Exception: %s\n", e.what());
    }
}


void CFaceLab::handleEngineOutputData(EngineOutputData& gazeData,
									  const InetAddress& from)
{
	m_pHeadGazeEngineData = gazeData.clone();
	m_lLastUpdate = m_pHeadGazeEngineData->frameNum()+1;
}

void CFaceLab::convFaceCoordinateToPoint3D(const FaceCoordinate &fc, Point3D &p)
{
	p.x = fc.x;
	p.y = fc.y;
	p.z = fc.z;
}


HeadGazeData CFaceLab::toStructure(EngineOutputData gazeData)
{
	HeadGazeData l_oMyData;

	//head translation
	l_oMyData.headTranslation.x = 
		gazeData.headOutputData()->headPosition().at(X_AXIS);
	l_oMyData.headTranslation.y = 
		gazeData.headOutputData()->headPosition().at(Y_AXIS);
	l_oMyData.headTranslation.z = 
		gazeData.headOutputData()->headPosition().at(Z_AXIS);

	//head rotation
	l_oMyData.headRotation.x = 
		gazeData.headOutputData()->headRotation().at(X_AXIS);
	l_oMyData.headRotation.y = 
		gazeData.headOutputData()->headRotation().at(Y_AXIS);
	l_oMyData.headRotation.z = 
		gazeData.headOutputData()->headRotation().at(Z_AXIS);

	//left eye
	l_oMyData.leftEyeValidity = 
		gazeData.eyeOutputData()->gazeOutputData()->gazeQualityLevel(LEFT_EYE);
	l_oMyData.leftPupilDiameter = 
		gazeData.eyeOutputData()->pupilOutputData()->pupilDiameter(LEFT_EYE);
	l_oMyData.leftEyeClosure = 
		gazeData.eyeOutputData()->eyeClosureOutputData()->eyeClosure(LEFT_EYE);

	//left gaze
	l_oMyData.leftGazeRotation.x = 
		gazeData.eyeOutputData()->gazeOutputData()->gazeRotation(LEFT_EYE).at(X_AXIS);
	l_oMyData.leftGazeRotation.y = 
		gazeData.eyeOutputData()->gazeOutputData()->gazeRotation(LEFT_EYE).at(Y_AXIS);

	//right eye
	l_oMyData.rightEyeValidity = 
		gazeData.eyeOutputData()->gazeOutputData()->gazeQualityLevel(RIGHT_EYE);
	l_oMyData.rightPupilDiameter = 
		gazeData.eyeOutputData()->pupilOutputData()->pupilDiameter(RIGHT_EYE);
	l_oMyData.rightEyeClosure = 
		gazeData.eyeOutputData()->eyeClosureOutputData()->eyeClosure(RIGHT_EYE);
	
	//right gaze
	l_oMyData.rightGazeRotation.x = 
		gazeData.eyeOutputData()->gazeOutputData()->gazeRotation(RIGHT_EYE).at(X_AXIS);
	l_oMyData.rightGazeRotation.y = 
		gazeData.eyeOutputData()->gazeOutputData()->gazeRotation(RIGHT_EYE).at(Y_AXIS);
	
	//face
	//	inner lip 
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(200).fc, l_oMyData.innerLip0);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(201).fc, l_oMyData.innerLip1);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(202).fc, l_oMyData.innerLip2);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(203).fc, l_oMyData.innerLip3);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(204).fc, l_oMyData.innerLip4);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(205).fc, l_oMyData.innerLip5);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(206).fc, l_oMyData.innerLip6);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(207).fc, l_oMyData.innerLip7);
	//	outer lip
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(100).fc, l_oMyData.outerLip0);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(101).fc, l_oMyData.outerLip1);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(102).fc, l_oMyData.outerLip2);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(103).fc, l_oMyData.outerLip3);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(104).fc, l_oMyData.outerLip4);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(105).fc, l_oMyData.outerLip5);
	//	mouth corner
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(5).fc, l_oMyData.leftMouthCorner);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(4).fc, l_oMyData.rightMouthCorner);	
	// 	left eyebrow
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(400).fc, l_oMyData.leftEyebrow0);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(401).fc, l_oMyData.leftEyebrow1);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(402).fc, l_oMyData.leftEyebrow2);
	// 	right eyebrow
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(300).fc, l_oMyData.rightEyebrow0);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(301).fc, l_oMyData.rightEyebrow1);
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(302).fc, l_oMyData.rightEyebrow2);
	//	nose center
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(500).fc, l_oMyData.noseCenter);
	//	left eye center
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(700).fc, l_oMyData.leftEyeCenter);
	// 	right eye center
	convFaceCoordinateToPoint3D(gazeData.faceOutputData()->faceLandmarks().at(600).fc, l_oMyData.rightEyeCenter);
	
	return l_oMyData;
}

void CFaceLab::handleDatagram(DatagramSocket& inputSocket)
{
    std::vector<uint8> l_aBuffer;
    InetAddress l_oFrom;

    inputSocket.receiveDatagram(l_aBuffer,l_oFrom);
    int l_iPos = 0;

    // Extract all the objects from the buffer
    while (l_iPos < (int)l_aBuffer.size()) 
    {
        // newObject will advance the pos for
        SerializablePtr l_pSerializable(SerializableFactory::newObject(l_aBuffer, l_iPos));
        // Check for an EngineOutputData object in the buffer
        if ((l_pSerializable != NULL) && (l_pSerializable->objectID() == EngineOutputData::ENGINE_OUTPUT_DATA_ID))
        {
            handleEngineOutputData(static_cast<EngineOutputData&>(*l_pSerializable), l_oFrom);
        }
        else 
        {
            printf("\n-ERROR: Unrecognised packet received, header id: %d\n", l_aBuffer[0]);
        }
    }
}

void CFaceLab::startListening()
{
	m_bListening = true;
	m_pListeningThread = new boost::thread(boost::bind(&CFaceLab::listen, this));
}

uint64 CFaceLab::getLastUpdate()
{
	return m_lLastUpdate;
}
