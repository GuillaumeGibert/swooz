// $forward.cpp 3.0 milbo$
// Derived from code by Henry Rowley http://vasc.ri.cmu.edu/NNFaceDetector.
//
// The original files had the following header:
// By Henry A. Rowley (har@cs.cmu.edu, http://www.cs.cmu.edu/~har)
// Developed at Carnegie Mellon University for the face detection project.
// Code may be used, but please provide appropriate acknowledgements, and let
// har@cs.cmu.edu how you are using it.  Thanks!

#include "stasm.hpp"
// using namespace swStasm;

//-----------------------------------------------------------------------------
ForwardStruct::~ForwardStruct ()
{
if (pUnitList != NULL) delete[] pUnitList;
if (pConnList != NULL) delete[] pConnList;
if (pConns != NULL)    delete[] pConns;
}

//-----------------------------------------------------------------------------
// Do a forward pass through the given neural network
// Returns the value of the first output unit
//
// The code uses the fact that units are contiguous in memory and that
// units are ordered so only one pass is needed with a simple for loop

_FLOAT
ForwardPass (ForwardStruct *pNet)
{
ForwardUnit * const pFirstHiddenUnit = pNet->pUnitList + pNet->iFirstHidden;
ForwardUnit * const pLastUnit = pNet->pUnitList + pNet->nUnits;
for (ForwardUnit *pUnit = pFirstHiddenUnit; pUnit < pLastUnit;  pUnit++)
    {
    DASSERT(pUnit->type == UNIT_TYPE_Tanh);
    _FLOAT Total = 0.0;
    ForwardConnection * const pLastConn = pUnit->pConns + pUnit->nInputs;
    for (ForwardConnection *pConn = pUnit->pConns; pConn < pLastConn; pConn++)
        Total += pConn->weight * pConn->pFrom->activation;
    pUnit->activation = (_FLOAT)tanh(Total);
    }
return pNet->pUnitList[pNet->iFirstOutput].activation;
}
