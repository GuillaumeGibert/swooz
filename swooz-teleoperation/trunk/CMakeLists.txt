# Copyright: (C) 2015 SWoOz
# Author: Guillaume Gibert
# YARP needs CMake 2.8.9 or greater
cmake_minimum_required(VERSION 2.8.9)
# find YARP
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(Urbi REQUIRED)
# add YARP include directories
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Urbi_INCLUDE_DIRS})
# set up our program
ADD_EXECUTABLE(StandAloneCPP StandAloneCPP.cpp)
# link with YARP libraries
TARGET_LINK_LIBRARIES(StandAloneCPP ${YARP_LIBRARIES} ${Urbi_LIBRARIES} -lrt -lpthread)
