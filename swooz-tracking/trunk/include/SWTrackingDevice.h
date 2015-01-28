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
 * \file SWTrackingDevice.h
 * \brief Devices lib id for yarp bottles
 * \author Florian Lance
 * \date 02/12/13
 */

#ifndef _SWTRACKINGDEVICE_
#define _SWTRACKINGDEVICE_

#include <string>


namespace swTracking
{
    enum DeviceLib
    {
        FOREST_LIB,EMICP_LIB,COREDATA_LIB,FASTRAK_LIB,STASM_LIB,TOBII_LIB,FACESHIFT_LIB,OPENNI_LIB,LEAP_LIB,DUMMY_LIB,
        RankFirst = FOREST_LIB, RankLast = DUMMY_LIB
    };

    /**
     * @brief returnStringValue
     * @param i32IdLib
     * @return
     */
    static std::string returnStringValue(const int i32IdLib)
    {
        std::string l_sLIB = "UNDEFINED_LIB";

        switch(i32IdLib)
        {
            case swTracking::FOREST_LIB :
                l_sLIB = "FOREST_LIB";
            break;
            case swTracking::EMICP_LIB :
                l_sLIB = "EMICP_LIB";
            break;
            case swTracking::COREDATA_LIB :
                l_sLIB = "COREDATA_LIB";
            break;
            case swTracking::FASTRAK_LIB :
                l_sLIB = "FASTRAK_LIB";
            break;
            case swTracking::STASM_LIB :
                l_sLIB = "STASM_LIB";
            break;
            case swTracking::TOBII_LIB :
                l_sLIB = "TOBII_LIB";
            break;
            case swTracking::FACESHIFT_LIB :
                l_sLIB = "FACESHIFT_LIB";
            break;
            case swTracking::OPENNI_LIB :
                l_sLIB = "OPENNI_LIB";
            break;
			case swTracking::LEAP_LIB :
                l_sLIB = "LEAP_LIB";
            break;			
            case swTracking::DUMMY_LIB :
                l_sLIB = "DUMMY_LIB";
            break;
        }

        return l_sLIB;
    }
}


#endif
