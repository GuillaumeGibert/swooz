// $readasm.hpp 3.0 milbo$ routines to read .asm files and associated .conf files

#if !defined(readasm_hpp)
#define readasm_hpp

// namespace swStasm
// {


	void
	ReadAsmFile(ASM_MODEL  &Asm,            // out
		    int        &nLevs,          // out: initialized by ReadAsmProlog
		    const char sAsmFile[]);     // in

// };
	
#endif // readasm_hpp
