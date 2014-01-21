// $landmarks.cp 3.0 milbo$ landmark tables for XM2VTS shapes
//-----------------------------------------------------------------------------
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// A copy of the GNU General Public License is available at
// http://www.r-project.org/Licenses/
//-----------------------------------------------------------------------------

#include "stasm.hpp"
// using namespace swStasm;

// The table below is ordered like XM2VTS but has extra points.
// By convention in the table: iPrev and iNext are ordered so
// whiskers for partner landmarks point in symmetric directions --
// generally away from the center of the face, except on the mouth and eyes
// where they point from/to the eye/mouth center.

LANDMARK gLandTab[] =
{
{ FA_Beard|FA_Glasses,      14, 21,  1, },      // 00_LTemple
{ FA_Beard|FA_Glasses,      13, -1, -1, },      // 01_LJaw1
{ FA_Beard,                 12, -1, -1, },      // 02_LJaw2
{ FA_Beard,                 11, -1, -1, },      // 03_LJaw3
{ FA_Beard,                 10, -1, -1, },      // 04_LJaw4
{ FA_Beard,                 9,  -1, -1, },      // 05_LJaw5
{ FA_Beard,                 8,  -1, -1, },      // 06_LJaw6
{ FA_Beard,                 -1, -1, -1, },      // 07_TipOfChin
{ FA_Beard,                 6,  -1, -1, },      // 08_RJaw6
{ FA_Beard,                 5,  -1, -1, },      // 09_RJaw5
{ FA_Beard,                 4,  -1, -1, },      // 10_RJaw4
{ FA_Beard,                 3,  -1, -1, },      // 11_RJaw3
{ FA_Beard,                 2,  -1, -1, },      // 12_RJaw2
{ FA_Beard|FA_Glasses,      1,  -1, -1, },      // 13_RJaw1
{ FA_Beard|FA_Glasses,      0,  -1, -1, },      // 14_RTemple
{ FA_Glasses,               21, -1, -1, },      // 15_ROuterEyeBrow
{ FA_Glasses,               22, -1, -1, },      // 16_ROuterTopEyeBrow
{ FA_Glasses,               23, -1, -1, },      // 17_RInnerTopEyeBrow
{ FA_Glasses,               24, 19, 17, },      // 18_RInnerEyeBrow
{ FA_Glasses,               25, 20, 18, },      // 19_Point19
{ FA_Glasses,               26, 32, 19, },      // 20_Point20
{ FA_Glasses,               15, 22,  0, },      // 21_LOuterEyeBrow
{ FA_Glasses,               16, 23, 21, },      // 22_LOuterTopEyeBrow
{ FA_Glasses,               17, 24, 22, },      // 23_LInnerTopEyeBrow
{ FA_Glasses,               18, -1, -1, },      // 24_LInnerEyeBrow
{ FA_Glasses,               19, -1, -1, },      // 25_Point25
{ FA_Glasses,               20, -1, -1, },      // 26_Point26
{ FA_Glasses,               32, 28, 30, },      // 27_LEyeOuter
{ FA_Glasses,               33, 29, 27, },      // 28_LEyeTop
{ FA_Glasses,               34, 30, 28, },      // 29_LEyeInner
{ FA_Glasses,               35, 27, 29, },      // 30_LEyeBottom
{ FA_Glasses,               36, 27, 29, },      // 31_LEye
{ FA_Glasses,               27, 35, 33, },      // 32_REyeOuter
{ FA_Glasses,               28, -1, -1, },      // 33_REyeTop
{ FA_Glasses,               29, -1, -1, },      // 34_REyeInner
{ FA_Glasses,               30, 34, 32, },      // 35_REyeBottom
{ FA_Glasses,               31, 34, 32, },      // 36_REye
{ FA_Glasses,               45, -1, -1, },      // 37_LNoseTop
{ FA_Glasses,               44, -1, -1, },      // 38_LNoseMid
{ FA_Mustache,              43, -1, -1, },      // 39_LNoseBot0
{ FA_Mustache,              42, -1, -1, },      // 40_LNoseBot1
{ FA_Mustache,              -1, 40, 42, },      // 41_Nosebase
{ FA_Mustache,              40, -1, -1, },      // 42_RNoseBot1
{ FA_Mustache,              39, -1, -1, },      // 43_RNoseBot0
{ FA_Glasses,               38, -1, -1, },      // 44_MRNoseMid
{ FA_Glasses,               37, 44, 31, },      // 45_RNoseTop
{ FA_Mustache,              47, -1, -1, },      // 46_LNostril
{ FA_Mustache,              46, 46, 37, },      // 47_RNostril
{ FA_Mustache,              54, 57, 51, },      // 48_LMouthCorner
{ FA_Mustache,              53, -1, -1, },      // 49_Mouth49
{ FA_Mustache,              52, -1, -1, },      // 50_Mouth50
{ FA_Mustache,              -1, -1, -1, },      // 51_MouthTopOfTopLip
{ FA_Mustache,              50, -1, -1, },      // 52_Mouth52
{ FA_Mustache,              49, -1, -1, },      // 53_Mouth53
{ FA_Mustache,              48, 51, 57, },      // 54_RMouthCorner
{ 0,                        59, -1, -1, },      // 55_Mouth55
{ 0,                        58, -1, -1, },      // 56_Mouth56
{ 0,                        -1, -1, -1, },      // 57_MouthBotOfBotLip
{ 0,                        56, -1, -1, },      // 58_Mouth58
{ 0,                        55, 58, 48, },      // 59_Mouth59
{ 0,                        62, 61, 59, },      // 60_Mouth60
{ 0,                        -1, 62, 60, },      // 61_Mouth61
{ 0,                        60, 55, 61, },      // 62_Mouth62
{ 0,                        65, 48, 54, },      // 63_Mouth63
{ 0,                        -1, 65, 63, },      // 64_Mouth64
{ 0,                        63, 48, 54, },      // 65_Mouth65
{ 0,                        -1, 54, 48, },      // 66_MouthBotOfTopLip
{ 0,                        -1, 48, 54, },      // 67_NoseTip

// The definitions below are for 84 landmarks.
// The definitions for 76 point landmarks are different --- we update
// this table dynamically if we are working with 76 point landmarks.

// extra left eye landmarks
// Whisker directions chosen so that we don't need to mirror 1D profiles

{ FA_Synthesize|FA_Glasses, 76,  0, 14, },      // 68_LEye0
{ FA_Synthesize|FA_Glasses, 77,  0, 14, },      // 69_LEye1
{ FA_Synthesize|FA_Glasses, 78,  0, 14, },      // 70_LEye2
{ FA_Synthesize|FA_Glasses, 79, 14,  7, },      // 71_LEye3
{ FA_Synthesize|FA_Glasses, 80,  0, 14, },      // 72_LEye4
{ FA_Synthesize|FA_Glasses, 81,  0, 14, },      // 73_LEye5
{ FA_Synthesize|FA_Glasses, 82, 23,  7, },      // 74_LEye6
{ FA_Synthesize|FA_Glasses, 83, 23,  7, },      // 75_LEye7

//  REye landmarks synthesized from extra LEye landmarks

{ FA_Synthesize|FA_Glasses, 68,  0, 14, },      // 76_REye0
{ FA_Synthesize|FA_Glasses, 69,  0, 14, },      // 77_REye1
{ FA_Synthesize|FA_Glasses, 70,  0, 14, },      // 78_REye2
{ FA_Synthesize|FA_Glasses, 71,  0,  7, },      // 79_REye3
{ FA_Synthesize|FA_Glasses, 72,  0, 14, },      // 80_REye4
{ FA_Synthesize|FA_Glasses, 73,  0, 14, },      // 81_REye5
{ FA_Synthesize|FA_Glasses, 74, 17,  7, },      // 82_REye6
{ FA_Synthesize|FA_Glasses, 75, 17,  7, },      // 83_REye7
};

const int ngElemsLandTab = NELEMS(gLandTab);  // for conf parameter checking

static LANDMARK gLand76Tab[] =
{
{ FA_Glasses, -1,  27, 28, },      // 68
{ FA_Glasses, -1,  28, 29, },      // 69
{ FA_Glasses, -1,  29, 30, },      // 70
{ FA_Glasses, -1,  30, 27, },      // 71

{ FA_Glasses, -1,  32, 33, },      // 72
{ FA_Glasses, -1,  33, 34, },      // 73
{ FA_Glasses, -1,  34, 35, },      // 74
{ FA_Glasses, -1,  35, 32, },      // 75
};

//-----------------------------------------------------------------------------
// This is needed only if you are using shapes with more than 68 points
// and need the landmark table gLandTab (i.e. fXm2vts is set).
// If so, we have to initialize gLandTab correctly, and that's what this does.

void InitLandTab (int nPoints)
{
static int nOldPoints;
if (nOldPoints != nPoints)
    {
    ASSERT(nOldPoints <= 68); // prevent mixing 76 and 84 point landmarks
    ASSERT(nPoints <= 68 || nPoints == 76 || nPoints == 84); // sanity check
    if (nPoints == 76)
        memcpy(&gLandTab[68], gLand76Tab, sizeof(gLand76Tab));
    nOldPoints = nPoints;
    }
}
