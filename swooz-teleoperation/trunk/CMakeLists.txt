# Copyright: (C) 2015 SWoOz
# Author: Guillaume Gibert
PROJECT(SWTeleoperation_Reeti)
SET(Urbi_DIR /usr/local/gostai/ CACHE PATH "Urbi installation path")
SET(SWTeleop_INCLUDE_DIRS /home/reeti/robotology/swooz/swooz-teleoperation/trunk/include CACHE PATH "SWoOz teleoperation include path")
SET(SWToolkit_INCLUDE_DIRS /home/reeti/robotology/swooz/swooz-toolkit/trunk/include CACHE PATH "SWoOz toolkit include path")
SET(SWTracking_INCLUDE_DIRS /home/reeti/robotology/swooz/swooz-tracking/trunk/include CACHE PATH "SWoOz tracking include path")
# YARP needs CMake 2.8.9 or greater
cmake_minimum_required(VERSION 2.8.9)
# find YARP
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(Urbi REQUIRED)
# add YARP; Urbi and local include directories
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Urbi_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SWTeleop_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SWToolkit_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SWTracking_INCLUDE_DIRS})
# set up our program
ADD_EXECUTABLE(SWTeleoperation_Reeti src/reeti/SWTeleoperation_reeti.cpp)
# link with YARP libraries
TARGET_LINK_LIBRARIES(SWTeleoperation_Reeti ${YARP_LIBRARIES} ${Urbi_LIBRARIES} -lrt -lpthread -lboost_system)


