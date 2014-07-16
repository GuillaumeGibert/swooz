

if(not(-d "doc"))
{
    mkdir "doc";
}

system("doxygen ./Doxyfile");
1;
