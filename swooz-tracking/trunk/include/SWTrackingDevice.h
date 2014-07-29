
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
