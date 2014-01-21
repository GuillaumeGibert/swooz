// $landmarks.hpp 3.0 milbo$ landmark tables for XM2VTS shapes

#if !defined(landmarks_hpp)
#define landmarks_hpp

// if you increase MAX_NBR_LANDMARKS then you should increase gLandTab too
// namespace swStasm
// {
	static const int MAX_NBR_LANDMARKS = 84;

	// landmark names (these are XM2VTS landmarks, with extra landmarks appended)

	typedef enum eLandmarks
	    {
	    MLTemple,           // 0 landmark names in internal numbering scheme
	    MLJaw1,             // 1 left right are wrt the viewer, not the subject
	    MLJaw2,             // 2
	    MLJaw3_MouthLine,   // 3 mouth line on jaw
	    MLJaw4,             // 4
	    MLJaw5,             // 5
	    MLJaw6,             // 6
	    MTipOfChin,         // 7
	    MRJaw6,             // 8
	    MRJaw5,             // 9
	    MRJaw4,             // 10
	    MRJaw3_MouthLine,   // 11 mouth line on jaw
	    MRJaw2,             // 12
	    MRJaw1,             // 13
	    MRTemple,           // 14
	    MROuterEyeBrow,     // 15
	    MROuterTopEyeBrow,  // 16
	    MRInnerTopEyeBrow,  // 17
	    MRInnerEyeBrow,     // 18
	    MPoint19,           // 19
	    MPoint20,           // 20
	    MLOuterEyeBrow,     // 21
	    MLOuterTopEyeBrow,  // 22
	    MLInnerTopEyeBrow,  // 23
	    MLInnerEyeBrow,     // 24
	    MPoint25,           // 25
	    MPoint26,           // 26
	    MLEyeOuter,         // 27
	    MLEyeTop,           // 28
	    MLEyeInner,         // 29
	    MLEyeBottom,        // 30
	    MLEye,              // 31 pupil
	    MREyeOuter,         // 32
	    MREyeTop,           // 33
	    MREyeInner,         // 34
	    MREyeBottom,        // 35
	    MREye,              // 36 pupil
	    MLNoseTop,          // 37
	    MLNoseMid,          // 38
	    MLNoseBot0,         // 39
	    MLNoseBot1,         // 40
	    MNosebase,          // 41
	    MRNoseBot1,         // 42
	    MRNoseBot0,         // 43
	    MRNoseMid,          // 44
	    MRNoseTop,          // 45
	    MLNostril,          // 46
	    MRNostril,          // 47
	    MLMouthCorner,      // 48
	    MMouth49,           // 49
	    MMouth50,           // 50
	    MMouthTopOfTopLip,  // 51
	    MMouth52,           // 52
	    MMouth53,           // 53
	    MRMouthCorner,      // 54
	    MMouth55,           // 55
	    MMouth56,           // 56
	    MMouthBotOfBotLip,  // 57
	    MMouth58,           // 58
	    MMouth59,           // 59
	    MMouth60,           // 60
	    MMouthTopOfBotLip,  // 61
	    MMouth62,           // 62
	    MMouth63,           // 63
	    MMouth64,           // 64
	    MMouth65,           // 65
	    MMouthBotOfTopLip,  // 66
	    MNoseTip,           // 67
	    MLEye0,             // 68 extra points beyond XM2VTS
	    MLEye1,             // 69
	    MLEye2,             // 70
	    MLEye3,             // 71
	    MLEye4,             // 72
	    MLEye5,             // 73
	    MLEye6,             // 74
	    MLEye7,             // 75
	    MREye0,             // 76 REye points synthesized from extra LEye points
	    MREye1,             // 77
	    MREye2,             // 78
	    MREye3,             // 79
	    MREye4,             // 80
	    MREye5,             // 81
	    MREye6,             // 82
	    MREye7              // 83
	    }
	eLandmarks;

	//-----------------------------------------------------------------------------
	// Landmark tables

	typedef struct LANDMARK        // landmark information
	    {
	    unsigned    Bits;           // FA_ bits in atface.hpp

	    int         iPartner;       // symmetrical partner point, e.g. left eye pupil matches
					// right eye pupil, -1 means no partner

	    int         iPrev, iNext;   // previous and next point for pointing whisker
					// in correct direction
					// -1 means iPrev=iThis-1 iNext=iThis+1
	    }
	LANDMARK;


	const int iEnd76ToMid76[] =
	{
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	27, 68, 28, 69, 29, 70, 30, 71, 31,
	32, 72, 33, 73, 34, 74, 35, 75, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
	53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67
	};

	const int iMid76ToEnd76[] =
	{
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	27, 29, 31, 33, 35, 36, 38, 40, 42,
	44, 45, 46, 47, 48, 49, 50, 51, 52,
	53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
	69, 70, 71, 72, 73, 74, 75, 28, 30, 32, 34, 37, 39, 41, 43
	};

	extern LANDMARK gLandTab[];
	extern const int ngElemsLandTab; // for conf parameter checking
	extern void InitLandTab(int nPoints);
// };

#endif // landmarks_hpp
