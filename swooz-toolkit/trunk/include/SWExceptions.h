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
 * \file SWExceptions.h
 * \brief defines exceptions used in swooz
 * \author Florian Lance
 * \date 01/03/13
 */

#ifndef _SWEXCEPTIONS_
#define _SWEXCEPTIONS_

#include <iostream>

// ######################################################################## main exceptions

namespace swExcept
{
    /**
     * \class opencvError
     */
    class opencvError : public std::exception { public : virtual const char * what(void) const throw (){ return "Opencv error. "; }};

    /**
     * \class stasmError
     */
    class stasmError : public std::exception { public : virtual const char * what(void) const throw () { return "Stasm error. "; }};

    /**
     * \class cloudError
     */
    class cloudError : public std::exception { public : virtual const char * what(void) const throw () { return "Cloud module error. ";}};

    /**
     * \class deviceError
     */
    class deviceError : public std::exception { public : virtual const char * what(void) const throw () { return "Device module error. ";}};

    /**
     * \class openglError
     */
    class openglError : public std::exception { public : virtual const char * what(void) const throw () { return "Device module error. ";}};

    /**
     * \class faceShiftError
     */
    class faceShiftError : public std::exception { public : virtual const char * what(void) const throw () { return "FaceShift module error. ";}};


    // ######################################################################## sub exceptions


    // ############################### opencvError

    /**
     * \class haarCascadeError
     */
    class haarCascadeError : public opencvError { public : virtual const char * what(void) const throw () { return "Haar cascade error. "; }};

    /**
     * \class opticalFlowError
     */
    class opticalFlowError : public opencvError { public: virtual const char * what(void) const throw () { return "Optical flow error. ";}};

    /**
     * \class fileStorageError
     */
    class fileStorageError : public opencvError { public: virtual const char * what(void) const throw () { return "File storage error. ";}};

    /**
     * \class videoWriterError
     */
    class videoWriterError : public opencvError { public: virtual const char * what(void) const throw () { return "Video writer error. ";}};

    /**
     * \class videoCaptureError
     */
    class videoCaptureError : public opencvError { public: virtual const char * what(void) const throw () { return "Video capture error. ";}};


    // ############################### cloudError

    /**
     * \class avatarCloudError
     */
    class avatarCloudError : public cloudError { public: virtual const char * what(void) const throw () { return "Avatar cloud error. ";}};

    /**
     * \class swcloudError
     */
    class swcloudError : public cloudError { public: virtual const char * what(void) const throw () { return "SWCloud error. ";}};

    // ############################### deviceError

    /**
     * \class swFaceLabError
     */
    class swFaceLabError : public deviceError { public: virtual const char * what(void) const throw () { return "FaceLab device not detected. ";}};

    /**
     * \class swKinectError
     */
    class swKinectError : public deviceError { public: virtual const char * what(void) const throw () { return "Kinect device error. ";}};

    /**
     * \class swFastrakError
     */
    class swFastrakError : public deviceError { public: virtual const char * what(void) const throw () { return "Fastrak device error. ";}};


    // ############################### openglError

    /**
     * \class swBufferGLError
     */
    class swBufferGLError : public openglError { public: virtual const char * what(void) const throw () { return "GL buffer error. ";}};


    /**
     * \class swShaderGLError
     */
    class swShaderGLError : public openglError { public: virtual const char * what(void) const throw () { return "GL shader error. ";}};


    // ######################################################################## sub-sub exceptions

    // ############################### haarCascadeError

    /**
     * \class haarFileInitError
     * \brief Haar cascade file initialization exception
     */
    class haarFileInitError : public haarCascadeError { public: virtual const char * what(void) const throw () {return "Opencv Haar cascade file initialization failed. ";}};

    /**
     * \class haarDetectError
     * \brief Haar cascade detection exception
     */
    class haarDetectError : public haarCascadeError { public : virtual const char * what(void) const throw (){ return "An error occurred during Opencv haar cascade detection. ";}};


    // ############################### opticalFlowError

    /**
     * \class opticalComputeError
     * \brief Optical flow computing exception
     */
    class opticalComputeError : public opticalFlowError { public: virtual const char * what(void) const throw (){ return "An error occurred during Opencv calcOpticalFlowPyrLK. ";}};

    /**
     * \class opticalFeaturesError
     * \brief Optical flow features exception
     */
    class opticalFeaturesError : public opticalFlowError { public: virtual const char * what(void) const throw () { return "An error occurred during Opencv goodFeaturesToTrack. ";}};


    // ############################### avatarCloudError

    /**
     * \class cloudBadDataError
     * \brief Avatar cloud input data is bad.
     */
    class cloudBadDataError :  public avatarCloudError { public: virtual const char * what(void) const throw () { return "Bad input data in avatar cloud function. ";}};

    // ############################### fileStorageError

    /**
     * \class fileStorageInitError
     * \brief File storage initialization failed.
     */
    class fileStorageInitError :  public fileStorageError { public: virtual const char * what(void) const throw () { return "Opencv file storage initialization failed. ";}};


    // ############################### videoWriterError

    /**
     * \class videoWriterInitError
     * \brief Video writer initialization failed.
     */
    class videoWriterInitError :  public videoWriterError { public: virtual const char * what(void) const throw () { return "Opencv videoWriter initialization failed. ";}};


    // ############################### videoCaptureError

    /**
     * \class videoCaptureInitError
     * \brief Video capture initlization failed.
     */
    class videoCaptureInitError :  public videoCaptureError { public: virtual const char * what(void) const throw () { return "Opencv videoCapture initialization failed. ";}};

    // ############################### kinect

    /**
     * \class kinectInitError
     * \brief Kinect initialization error.
     */
    class kinectInitError :  public swKinectError { public: virtual const char * what(void) const throw () { return "Kinect initialization failed. ";}};

    /**
     * \class kinectGrabError
     * \brief Kinect grab new image error.
     */
    class kinectGrabError :  public swKinectError { public: virtual const char * what(void) const throw () { return "Kinect grab failed. ";}};

    /**
     * \class kinectDataTimeOutError
     * \brief Kinect data time out error.
     */
    class kinectDataTimeOutError :  public swKinectError { public: virtual const char * what(void) const throw () { return "Kinect data time out launched. ";}};

    // ############################### fastrak

    /**
     * \class fastrakInitError
     * \brief Fastrak initialization error.
     */
    class fastrakInitError :  public swFastrakError { public: virtual const char * what(void) const throw () { return "Fastrak initialization failed. ";}};


    /**
     * \class fastrakDataTimeOutError
     * \brief Fastrak data time out error.
     */
    class fastrakDataTimeOutError :  public swFastrakError { public: virtual const char * what(void) const throw () { return "Fastrak data time out launched. ";}};
}


#endif
