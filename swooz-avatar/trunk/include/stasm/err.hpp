// $err.hpp 3.0 milbo$ error printing routines

#if !defined(err_hpp)
#define err_hpp

extern bool fgErr;                      // set if there is an error

void Err(const char *pArgs, ...);       // args like printf
void Warn(const char *pArgs, ...);
void WarnWithNewLine(const char *pArgs, ...);

#endif // err_hpp
