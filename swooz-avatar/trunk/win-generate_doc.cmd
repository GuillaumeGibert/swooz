@echo off

@echo generate
@echo ...

if not exist ./doc/nul (
    mkdir doc
)


doxygen ./Doxyfile
