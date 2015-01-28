
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


#include "SWTrackingDevice.h"

// facelab min max
// -0.69 -1.22 -0.95
// 0.52 1.04 0.87


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

    std::string l_sDevice, l_sLib, l_sRobotPart, l_sLIB_DEVICE, l_sLine;
    l_oFileStream >> l_sDevice;
    l_oFileStream >> l_sLib;
    l_oFileStream >> l_sRobotPart;
    l_oFileStream >> l_sLIB_DEVICE;
    getline(l_oFileStream, l_sLine);

    std::cout << "Device : " << l_sDevice << std::endl;
    std::cout << "Lib : " << l_sLib << std::endl;
    std::cout << "Robot part : " << l_sRobotPart << std::endl;
    std::cout << "Lib device : " << l_sLIB_DEVICE << std::endl;

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

    // ###################################

    int l_i32Lib_Id = 0;

    for (int ii = swTracking::RankFirst; ii <= swTracking::RankLast; ++ii)
    {
        if(swTracking::returnStringValue(ii) == l_sLIB_DEVICE)
        {
            l_i32Lib_Id = ii;
            break;
        }
    }

    yarp::os::BufferedPort<yarp::os::Bottle> l_oFakeTrackingPort;
    std::string l_sPortName =  "/tracking/" + l_sDevice + "/"+ l_sLib + "/" + l_sRobotPart;
    l_oFakeTrackingPort.open(l_sPortName.c_str());

    std::cout << "Start sending bottles to " << l_sPortName << " ? " << std::endl;
    char l_cKey;
    std::cin >> l_cKey;


    clock_t l_oInitialTime = clock();

    for(uint ii = 0; ii < l_vBottlesTimes.size(); ++ii)
    {
        double l_dTimeToWait = l_vBottlesTimes[ii] - (float)(clock() - l_oInitialTime)/CLOCKS_PER_SEC;
        boost::this_thread::sleep(boost::posix_time::milliseconds(static_cast<int64>(1000*l_dTimeToWait)));

        // stuff to do
        yarp::os::Bottle &l_oFakeTrackingBottle = l_oFakeTrackingPort.prepare();
        l_oFakeTrackingBottle.clear();

            // device lib id
            l_oFakeTrackingBottle.addInt(l_i32Lib_Id); //head : l_i32Lib_Id id / get(0).asInt()

//            std::cout << l_vBottlesTimes[ii] << " ";

            for(uint jj = 0; jj < l_vVData[ii].size(); ++jj)
            {
                l_oFakeTrackingBottle.addDouble(l_vVData[ii][jj]);
//                std::cout << l_vVData[ii][jj] << " ";
            }

//            std::cout << std::endl;

        l_oFakeTrackingPort.write(); // true
    }

    l_oFakeTrackingPort.close();

    // terminate network
    yarp::os::Network::fini();


    return 0;
}
