// $initnet.hpp 3.0 milbo$
// Derived from code by Henry Rowley http://vasc.ri.cmu.edu/NNFaceDetector.

#ifndef initnet_hpp
#define initnet_hpp

// namespace swStasm
// {

	extern ForwardStruct **gNetList;
	void FreeNetworks(void);
	void AllocateNetworks(int nNetworks);
	void InitNetwork(int iNet, int nNetworks, const char *pNetworkNames[]);
// };
	
#endif // initnet_hpp
