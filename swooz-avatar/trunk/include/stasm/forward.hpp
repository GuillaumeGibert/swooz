// $forward.hpp 3.0 milbo$
// Derived from code by Henry Rowley http://vasc.ri.cmu.edu/NNFaceDetector.

#ifndef forward_hpp
#define forward_hpp

#define _FLOAT   float   // base type of Rowley neural nets

#ifndef POW12
    // Different Microsoft compilers give (very slightly) different values
    // for pow().  This causes slighly different results for the Rowley
    // detector, which messes up regression testing.  So fix that by rounding,
    // which doesn't solve all issues but works well enough for our purposes.

    #define POW12(x) (floor(pow(1.2, (x)) * 1e6) / 1e6)
#endif

#ifndef DASSERT
    #if _DEBUG
    #define DASSERT(x) (void)((x) || (Err("%s %d: FAIL %s", __FILE__, __LINE__, #x), 0) )
    #else
    #define DASSERT(x)
    #endif
#endif

// namespace swStasm
// {

	static const int UNIT_TYPE_Tanh = 0;
	//static const int UNIT_TYPE_Sigmoid = 1;
	//static const int UNIT_TYPE_NoisyOr = 2;
	//static const int UNIT_TYPE_NoisyAnd = 3;
	static const int UNIT_TYPE_Bias = 5;
	static const int UNIT_TYPE_Input = 5;
	//static const int UNIT_TYPE_Vote3 = 6;
	//static const int UNIT_TYPE_Linear = 7;
	//static const int UNIT_TYPE_Multiply = 8;
	//static const int UNIT_TYPE_Sum = 9;

	struct ForwardUnit;

	struct ForwardConnection
	  {
	  _FLOAT            weight;
	  ForwardUnit       *pFrom;
	  };

	struct ForwardConnectionRef
	  {
	  ForwardConnection *pConn;
	  int               iFrom, iTo;
	  };

	struct ForwardUnit
	  {
	  _FLOAT            activation;
	  int               nInputs;
	  int               type;
	  ForwardConnection *pConns;
	  };

	struct UnitInfo
	  {
	  int               x, y, iGroup;
	  };

	struct GroupInfo
	  {
	  int               startx, starty, endx, endy;
	  int               nUnits;
	  int               firstUnit;
	  };

	// ForwardStruct: one of these for each network.
	// Example: pUnitList[33] has the details for unit number 33.
	// Hidden units start at net->pUnitList+net->iFirstHidden.
	// Ditto for input and output units.
	// Therefore all units with unit nbr < nInputs are input units.

	struct ForwardStruct
	  {
	  const char *sName;
	  int nUnits, nInputs, nConns;
	  int iFirstHidden, iFirstOutput;
	  ForwardUnit           *pUnitList;
	  ForwardConnection     *pConns;
	  ForwardConnectionRef  *pConnList; // only used during initialization
	  ~ForwardStruct();
	  };

	_FLOAT ForwardPass(ForwardStruct *pNet);
  // };

#endif // forward_hpp
