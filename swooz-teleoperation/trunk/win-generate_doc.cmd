@echo off

@echo generate doc
@echo ...

if not exist ./doc/nul (
    mkdir doc
)


doxygen ./Doxyfile
