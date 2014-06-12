
//#include "SWFakeTracking.h"


#include <iostream>

// YARP
#include <yarp/dev/all.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>


#include "commonTypes.h"
#include <fstream>
#include <vector>
#include <sstream>

#include <time.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>




int countWhiteSpaces(std::ifstream  &oFileStream)
{
    int l_i32Pos = static_cast<int>(oFileStream.tellg());

    int l_i32WhiteSpaces = 0;

    std::string l_sLine;
    getline(oFileStream, l_sLine);

    for(uint ii = 0; ii < l_sLine.size(); ++ii)
    {
        if(l_sLine[ii] == ' ')
        {
            ++l_i32WhiteSpaces;
        }
    }

    oFileStream.seekg(l_i32Pos);

    return l_i32WhiteSpaces;
}


int main(int argc, char* argv[])
{
    // initialize yarp network
    yarp::os::Network l_oYarp;
    if (!l_oYarp.checkNetwork())
    {
        std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
        return -1;
    }

    if(argc < 2)
    {
        std::cerr << "Specify a tracking data txt file to be emulated in the command line. " << std::endl;
        return -1;
    }

    std::string l_sPathFile(argv[1]);

    std::ifstream  l_oFileStream;
    l_oFileStream.open(l_sPathFile);

    if(l_oFileStream.is_open())
    {
        std::cout << "Data file " << l_sPathFile << " opened. " << std::endl;
    }
    else
    {
        std::cerr << "Can't open " << l_sPathFile << " file. " << std::endl;
        return -1;
    }

    std::string l_sDevice, l_sLib, l_sRobotPart, l_sLine;
    l_oFileStream >> l_sDevice;
    l_oFileStream >> l_sLib;
    l_oFileStream >> l_sRobotPart;
    getline(l_oFileStream, l_sLine);

    std::cout << "Device : " << l_sDevice << std::endl;
    std::cout << "Lib : " << l_sLib << std::endl;
    std::cout << "Robot part : " << l_sRobotPart << std::endl;

    bool l_bEndFile = false;

    std::string l_sValue = "";

    int l_i32WhiteSpaces = countWhiteSpaces(l_oFileStream);

    std::vector<double> l_vBottlesTimes;
    std::vector<std::vector<double> > l_vVData;

    while(!l_bEndFile)
    {
        l_vVData.push_back(std::vector<double>(0));

        for(int ii = 0; ii < l_i32WhiteSpaces+1; ++ii)
        {
            l_oFileStream >> l_sValue;

            if(l_sValue.size() == 0)
            {
                l_bEndFile = true;
                break;
            }

            std::istringstream l_sBuffer(l_sValue);
            double l_dValue;
            l_sBuffer >> l_dValue;

            if(ii == 0)
            {
                l_vBottlesTimes.push_back(l_dValue);
            }
            else
            {
                l_vVData[l_vVData.size()-1].push_back(l_dValue);
            }
        }



        l_sValue = "";
    }

    l_oFileStream.close();

    //    for(uint ii = 0; ii < l_vBottlesTimes.size(); ++ii)
    //    {
    //        std::cout << l_vBottlesTimes[ii] << " ";

    //        for(uint jj = 0; jj < l_vVData[ii].size(); ++jj)
    //        {
    //            std::cout << l_vVData[ii][jj] << " ";
    //        }
    //        std::cout << std::endl;
    //    }


    // #######################################


    yarp::os::BufferedPort<yarp::os::Bottle> l_oFakeTrackingPort;
    std::string l_sPortName =  "/tracking/" + l_sDevice + "/"+ l_sLib + "/" + l_sRobotPart;
    l_oFakeTrackingPort.open(l_sPortName.c_str());

    clock_t l_oProgramTime = clock();

    int ii = 0;


    while(l_vBottlesTimes.back() * 1000> ((float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC))
    {
        while(l_vBottlesTimes[ii] * 1000> ((float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC))
        {

        }

        ++ii;

//        std::cout << l_vBottlesTimes.back() * 1000 << " | " <<  ((float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC) << " | ";

//        if(l_vBottlesTimes[ii] * 1000 > ((float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC))
//        {
//            std::cout << l_vBottlesTimes[ii] * 1000 << " ";
//            ++ii;
//        }

//        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }



//    (float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC







    return 0;
}
