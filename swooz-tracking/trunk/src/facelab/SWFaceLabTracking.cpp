
/**
 * \file SWFaceLabTracking.cpp
 * \brief Defines SWFaceLabTracking class
 * \author Florian Lance
 * \date 29/11/13
 */

#include "facelab/SWFaceLabTracking.h"
#include "SWTrackingDevice.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace swExcept;

SWFaceLabTracking::SWFaceLabTracking() : m_bIsFaceLabInitialized(true), m_i32Fps(1000)
{
    std::string l_sDeviceName  = "facelab";
    std::string l_sLibraryName = "coredata";

    // set ports name
    m_sHeadTrackingPortName =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/head";
    m_sFaceTrackingPortName =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/face";
    m_sGazeTrackingPortName =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/gaze";

    m_oHeadTrackingPort.open(m_sHeadTrackingPortName.c_str());
    m_oFaceTrackingPort.open(m_sFaceTrackingPortName.c_str());
    m_oGazeTrackingPort.open(m_sGazeTrackingPortName.c_str());

    try
    {
        initFaceLab();
    }
    catch(swFaceLabError &e)
    {
        // close the module
        std::cerr << "-ERROR : " << e.what() << std::endl;
        interruptModule();
        close();
        m_bIsFaceLabInitialized = false;
    }
}

bool SWFaceLabTracking::isFaceLabInitialized() const
{
    return m_bIsFaceLabInitialized;
}

bool SWFaceLabTracking::close()
{
    m_oFaceLab.stop();

    m_oHeadTrackingPort.close();
    m_oFaceTrackingPort.close();
    m_oGazeTrackingPort.close();

    // terminate network
    Network::fini();

    return true;
}

bool SWFaceLabTracking::interruptModule()
{
    return true;
}

bool SWFaceLabTracking::configure(ResourceFinder &oRf)
{
    // ... nothing for now
    return true;
}

void SWFaceLabTracking::initFaceLab()
{
    m_oFaceLab.init();
}

double SWFaceLabTracking::getPeriod()
{
    // module periodicity (seconds), called implicitly by myModule
    return 1./m_i32Fps;
}


bool SWFaceLabTracking::updateModule()
{
    if(!m_bIsFaceLabInitialized)
    {
        // if an error occured during the configuration
        interruptModule();
        close();

        return false;
    }

    //  grab FaceLab data
    m_oFaceLab.read();
    HeadGazeData l_oDataResult = m_oFaceLab.m_oHeadGazeData;

    //  Time::delay(0.001);

    // head bottle
    Bottle &l_oHeadBottle = m_oHeadTrackingPort.prepare();
    l_oHeadBottle.clear();

        // device lib id
        l_oHeadBottle.addInt(swTracking::COREDATA_LIB); //head : COREDATA_LIB id / get(0).asInt()

        // head translation
        l_oHeadBottle.addDouble(l_oDataResult.headTranslation.x); //head: Tx / get(1).asDouble()
        l_oHeadBottle.addDouble(l_oDataResult.headTranslation.y); //head: Ty / get(2).asDouble()
        l_oHeadBottle.addDouble(l_oDataResult.headTranslation.z); //head: Tz / get(3).asDouble()

        // head rotation
        l_oHeadBottle.addDouble(l_oDataResult.headRotation.x); //head: Rx / get(4).asDouble()
        l_oHeadBottle.addDouble(l_oDataResult.headRotation.y); //head: Ry / get(5).asDouble()
        l_oHeadBottle.addDouble(l_oDataResult.headRotation.z); //head: Rz / get(6).asDouble()

    m_oHeadTrackingPort.write();

    // face bottle
    Bottle &l_oFaceBottle = m_oFaceTrackingPort.prepare();
    l_oFaceBottle.clear();

        // device lib id
        l_oFaceBottle.addInt(swTracking::COREDATA_LIB); //face : COREDATA_LIB id / get(0).asInt()

        // outer lip
        l_oFaceBottle.addDouble(l_oDataResult.outerLip0.x); //face: outerLip0 x / get(1).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip0.y); //face: outerLip0 y / get(2).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip0.z); //face: outerLip0 z / get(3).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip1.x); //face: outerLip1 x / get(4).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip1.y); //face: outerLip1 y / get(5).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip1.z); //face: outerLip1 z / get(6).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip2.x); //face: outerLip2 x / get(7).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip2.y); //face: outerLip2 y / get(8).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip2.z); //face: outerLip2 z / get(9).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip3.x); //face: outerLip3 x / get(10).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip3.y); //face: outerLip3 y / get(11).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip3.z); //face: outerLip3 z / get(12).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip4.x); //face: outerLip4 x / get(13).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip4.y); //face: outerLip4 y / get(14).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip4.z); //face: outerLip4 z / get(15).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip5.x); //face: outerLip5 x / get(16).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip5.y); //face: outerLip5 y / get(17).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.outerLip5.z); //face: outerLip5 z / get(18).asDouble()

        // inner lip
        l_oFaceBottle.addDouble(l_oDataResult.innerLip0.x); //face: innerLip0 x / get(19).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip0.y); //face: innerLip0 y / get(20).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip0.z); //face: innerLip0 z / get(21).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip1.x); //face: innerLip1 x / get(22).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip1.y); //face: innerLip1 y / get(23).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip1.z); //face: innerLip1 z / get(24).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip2.x); //face: innerLip2 x / get(25).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip2.y); //face: innerLip2 y / get(26).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip2.z); //face: innerLip2 z / get(27).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip3.x); //face: innerLip3 x / get(28).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip3.y); //face: innerLip3 y / get(29).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip3.z); //face: innerLip3 z / get(30).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip4.x); //face: innerLip4 x / get(31).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip4.y); //face: innerLip4 y / get(32).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip4.z); //face: innerLip4 z / get(33).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip5.x); //face: innerLip5 x / get(34).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip5.y); //face: innerLip5 y / get(35).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip5.z); //face: innerLip5 z / get(36).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip6.x); //face: innerLip6 x / get(37).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip6.y); //face: innerLip6 y / get(38).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip6.z); //face: innerLip6 z / get(39).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip7.x); //face: innerLip7 x / get(40).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip7.y); //face: innerLip7 y / get(41).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.innerLip7.z); //face: innerLip7 z / get(42).asDouble()

        // right eyebrow
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow0.x); //face: rightEyebrow0 x / get(43).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow0.y); //face: rightEyebrow0 y / get(44).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow0.z); //face: rightEyebrow0 z / get(45).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow1.x); //face: rightEyebrow1 x / get(46).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow1.y); //face: rightEyebrow1 y / get(47).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow1.z); //face: rightEyebrow1 z / get(48).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow2.x); //face: rightEyebrow2 x / get(49).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow2.y); //face: rightEyebrow2 y / get(50).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyebrow2.z); //face: rightEyebrow2 z / get(51).asDouble()

        // left eyebrow
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow0.x); //face: rightEyebrow0 x / get(52).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow0.y); //face: rightEyebrow0 y / get(53).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow0.z); //face: rightEyebrow0 z / get(54).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow1.x); //face: rightEyebrow1 x / get(55).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow1.y); //face: rightEyebrow1 y / get(56).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow1.z); //face: rightEyebrow1 z / get(57).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow2.x); //face: rightEyebrow2 x / get(58).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow2.y); //face: rightEyebrow2 y / get(59).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyebrow2.z); //face: rightEyebrow2 z / get(60).asDouble()

        // nose center
        l_oFaceBottle.addDouble(l_oDataResult.noseCenter.x); //face: noseCenter x / get(61).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.noseCenter.y); //face: noseCenter y / get(62).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.noseCenter.z); //face: noseCenter z / get(63).asDouble()

        // left eye center
        l_oFaceBottle.addDouble(l_oDataResult.leftEyeCenter.x); //gaze: leftEyeCenter x / get(64).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyeCenter.y); //gaze: leftEyeCenter y / get(65).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.leftEyeCenter.z); //gaze: leftEyeCenter z / get(66).asDouble()

        // right eye center
        l_oFaceBottle.addDouble(l_oDataResult.rightEyeCenter.x); //gaze: rightEyeCenter x / get(67).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyeCenter.y); //gaze: rightEyeCenter y / get(68).asDouble()
        l_oFaceBottle.addDouble(l_oDataResult.rightEyeCenter.z); //gaze: rightEyeCenter z / get(69).asDouble()


     m_oFaceTrackingPort.write();

    // gaze bottle
    Bottle &l_oGazeBottle = m_oGazeTrackingPort.prepare();
    l_oGazeBottle.clear();

        // device lib id
        l_oGazeBottle.addInt(swTracking::COREDATA_LIB);         //gaze : COREDATA_LIB id / get(0).asInt()

        // left eye center
        l_oGazeBottle.addDouble(l_oDataResult.leftEyeCenter.x); //gaze: leftEyeCenter x / get(1).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.leftEyeCenter.y); //gaze: leftEyeCenter y / get(2).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.leftEyeCenter.z); //gaze: leftEyeCenter z / get(3).asDouble()

        // right eye center
        l_oGazeBottle.addDouble(l_oDataResult.rightEyeCenter.x); //gaze: rightEyeCenter x / get(4).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.rightEyeCenter.y); //gaze: rightEyeCenter y / get(5).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.rightEyeCenter.z); //gaze: rightEyeCenter z / get(6).asDouble()

        // left eye miscellaneous
        l_oGazeBottle.addDouble(l_oDataResult.leftPupilDiameter);  //gaze: leftPupilDiameter    / get(7).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.leftEyeClosure);     //gaze: leftEyeClosure       / get(8).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.leftGazeRotation.x); //gaze: leftGazeRotation x   / get(9).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.leftGazeRotation.y); //gaze: leftGazeRotation y   / get(10).asDouble()
        l_oGazeBottle.addInt(   l_oDataResult.leftEyeValidity);    //gaze: leftEyeValidity (see HeadGazeData.h) / get(11).asInt()

        // right eye miscellaneous
        l_oGazeBottle.addDouble(l_oDataResult.rightPupilDiameter);  //gaze: rightPupilDiameter  / get(12).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.rightEyeClosure);     //gaze: rightEyeClosure     / get(13).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.rightGazeRotation.x); //gaze: rightGazeRotation x / get(14).asDouble()
        l_oGazeBottle.addDouble(l_oDataResult.rightGazeRotation.y); //gaze: rightGazeRotation y / get(15).asDouble()
        l_oGazeBottle.addInt(   l_oDataResult.rightEyeValidity);    //gaze: rightEyeValidity (see HeadGazeData.h)   / get(16).asInt()

    m_oGazeTrackingPort.write();

    return true;
}


int main(int argc, char* argv[])
{
    // initialize yarp network
    Network l_oYarp;
    if (!l_oYarp.checkNetwork())
    {
        std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
        return -1;
    }

    // create module
    SWFaceLabTracking l_oFaceLabTracking;

    if(!l_oFaceLabTracking.isFaceLabInitialized())
    {
        std::cerr << "-ERROR: Failed to init the faceLab module. " << std::endl;
        return 0;
    }

    // prepare and configure the resource finder
    ResourceFinder rf;
        rf.setVerbose(true);
        rf.setDefaultConfigFile("facelabTracking.ini");
        rf.setDefaultContext("swtracking/conf");
        rf.configure("ICUB_ROOT", argc, argv);

    // configure the module
    std::cout << "Configuring the FaceLab tracking module..."<< std::endl;
    if (l_oFaceLabTracking.configure(rf))
    {
        // run the module
        std::cout << "Starting the FaceLab tracking module..." << std::endl;
        l_oFaceLabTracking.runModule();
    }
    else
    {
        std::cerr << "Failed to configure the FaceLab tracking module!"<< std::endl;
    }

    std::cout << "End running FaceLab tracking module. " << std::endl;

    return 0;
}
