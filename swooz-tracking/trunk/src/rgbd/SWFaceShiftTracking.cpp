
/**
 * \file SWFaceShiftTracking.cpp
 * \brief Defines SWFaceShiftTracking class
 * \author Florian Lance
 * \date 14/01/14
 */

#include "rgbd/SWFaceShiftTracking.h"
#include "SWTrackingDevice.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

#define DEFAULT_BUFLEN 1024

char recvbuf[DEFAULT_BUFLEN];


SWFaceShiftTracking::SWFaceShiftTracking() : m_i32Fps(30)
{
    std::string l_sDeviceName  = "rgbd";
    std::string l_sLibraryName = "faceshift";

    // set ports name
    m_sHeadTrackingPortName =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/head";
    m_sGazeTrackingPortName =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/gaze";

    m_oHeadTrackingPort.open(m_sHeadTrackingPortName.c_str());
    m_oGazeTrackingPort.open(m_sGazeTrackingPortName.c_str());

    initFaceShift();

    if(!m_bIsFaceShiftInitialized)
    {
        std::cerr << "-ERROR : faceShift initialization failed " << std::endl;
        interruptModule();
        close();
    }
}

bool SWFaceShiftTracking::isFaceShiftInitialized() const
{
    return m_bIsFaceShiftInitialized;
}

bool SWFaceShiftTracking::close()
{
    // clean faceShift
    closesocket(ConnectSocket);
    WSACleanup();

    // close yarp port
    m_oHeadTrackingPort.close();

    // terminate network
    Network::fini();

    return true;
}

bool SWFaceShiftTracking::interruptModule()
{
    return true;
}

bool SWFaceShiftTracking::configure(ResourceFinder &oRf)
{
    // ... nothing for now
    return true;
}

void SWFaceShiftTracking::initFaceShift()
{
    int err = 0;
    recvbuflen = DEFAULT_BUFLEN;

    WSADATA wsaData;
    WORD wVersionRequested;
    struct sockaddr_in clientService;
    ConnectSocket = INVALID_SOCKET;

    wVersionRequested = MAKEWORD(2,2);
    err = WSAStartup(wVersionRequested, &wsaData);

    m_bIsFaceShiftInitialized = true;

    if (err != 0)
    {
        m_bIsFaceShiftInitialized = false;
    }

    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        m_bIsFaceShiftInitialized = false;
    }

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        m_bIsFaceShiftInitialized = false;
    }

    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    clientService.sin_port = htons( 33433 );

    iResult = connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService));
    if ( iResult == SOCKET_ERROR)
    {
        closesocket (ConnectSocket);
        WSACleanup();
        m_bIsFaceShiftInitialized = false;
    }

    if (ConnectSocket == INVALID_SOCKET)
    {
        closesocket(ConnectSocket);
        WSACleanup();
    }
}

double SWFaceShiftTracking::getPeriod()
{
    // module periodicity (seconds), called implicitly by myModule
    return 1./m_i32Fps;
}


bool SWFaceShiftTracking::updateModule()
{
    if(!m_bIsFaceShiftInitialized)
    {
        // if an error occured during the configuration
        interruptModule();
        close();

        return false;
    }

    bool l_bTrackingSuccessful = false;
    fs::fsTrackingData data;

    //  grab faceShift data
    {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

        if ( iResult > 0 )
        {
            parserIn.received(iResult, recvbuf);
            while(msg=parserIn.get_message())
            {
                if(dynamic_cast<fs::fsMsgTrackingState*>(msg.get()))
                {
                    fs::fsMsgTrackingState *ts = dynamic_cast<fs::fsMsgTrackingState*>(msg.get());
                    fs::fsTrackingData &dataTracking = ts->tracking_data();
                    l_bTrackingSuccessful = dataTracking.m_trackingSuccessful;

                    data = dataTracking;
                }
            }
        }
        if(!parserIn.valid())
        {
            std::cerr << "-ERROR : parser in invalid state. " << std::endl;
            parserIn.clear();
        }
    }

    if(l_bTrackingSuccessful)
    {
        // head bottle
        Bottle &l_oHeadBottle = m_oHeadTrackingPort.prepare();
        l_oHeadBottle.clear();

            // device lib id
            l_oHeadBottle.addInt(swTracking::FACESHIFT_LIB); //head : FACESHIFT_LIB id / get(0).asInt()

            // head translation
            l_oHeadBottle.addDouble(data.m_headTranslation.x); //head: Tx / get(1).asDouble()
            l_oHeadBottle.addDouble(data.m_headTranslation.y); //head: Ty / get(2).asDouble()
            l_oHeadBottle.addDouble(data.m_headTranslation.z); //head: Tz / get(3).asDouble()

            // head rotation
            l_oHeadBottle.addDouble(data.m_headRotation.x); //head: Rx / get(4).asDouble()
            l_oHeadBottle.addDouble(data.m_headRotation.y); //head: Ry / get(5).asDouble()
            l_oHeadBottle.addDouble(data.m_headRotation.z); //head: Rz / get(6).asDouble()

        m_oHeadTrackingPort.write();


        // gaze bottle
        Bottle &l_oGazeBottle = m_oGazeTrackingPort.prepare();
        l_oGazeBottle.clear();

            // device lib id
            l_oGazeBottle.addInt(swTracking::FACESHIFT_LIB); //gaze : FACESHIFT_LIB id / get(0).asInt()

            // left eye
            l_oGazeBottle.addDouble(data.m_eyeGazeLeftPitch);    //gaze : left pitch / get(1).asDouble()
            l_oGazeBottle.addDouble(data.m_eyeGazeLeftYaw);      //gaze : left yaw   / get(2).asDouble()

            // right eye
            l_oGazeBottle.addDouble(data.m_eyeGazeRightPitch);   //gaze : right pitch / get(3).asDouble()
            l_oGazeBottle.addDouble(data.m_eyeGazeRightYaw);     //gaze : right yaw / get(4).asDouble()

        m_oGazeTrackingPort.write();
    }

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
    SWFaceShiftTracking l_oFaceShiftTracking;

    if(!l_oFaceShiftTracking.isFaceShiftInitialized())
    {
        std::cerr << "-ERROR: Failed to init the FaceShift tracking module. " << std::endl;
        return 0;
    }

    // prepare and configure the resource finder
    ResourceFinder rf;
        rf.setVerbose(true);
        rf.setDefaultConfigFile("faceShiftTracking.ini"); //overridden by --from parameter
        rf.setDefaultContext("swtracking/conf");        //overridden by --context parameter
        rf.configure("ICUB_ROOT", argc, argv);

    // configure the module
    std::cout << "Configuring the FaceShift tracking module..."<< std::endl;
    if (l_oFaceShiftTracking.configure(rf))
    {
        // run the module
        std::cout << "Starting the FaceShift tracking module..." << std::endl;
        l_oFaceShiftTracking.runModule();
    }
    else
    {
        std::cerr << "Failed to configure the FaceShift tracking module!"<< std::endl;
    }

    std::cout << "End running FaceShift tracking module. " << std::endl;

    return 0;
}
