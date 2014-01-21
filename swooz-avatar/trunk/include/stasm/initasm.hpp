// $initasm.hpp 3.0 milbo$ routines to read .asm files and associated .conf files

// namespace swStasm
// {

	extern bool CONF_fXm2vts;   // use (extended) XM2VTS tables in landmarks.hpp
	extern bool CONF_fMe17;
	extern bool CONF_fStasmSkipIfNotInShapeFile;

	int                                      // returns the number of models
	nInitAsmModels(ASM_MODEL  Models[],      // out: two ASM models
		       const char sConfFile0[],  // in: 1st config filename
		       const char sConfFile1[]); // in: 2nd config filename, "" if none
// };
