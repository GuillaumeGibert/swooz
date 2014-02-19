
/**
 * \file SWTestCode.cpp
 * \brief ...
 * \author Florian Lance
 * \date 14/06/13
 */
 
#include "interface/SWMorphingInterface.h"

/**\brief Main function */
int main(int argc, char* argv[])
{       
    QApplication l_oApp(argc, argv);

    SWMorphingInterface l_otest;
    l_otest.resize(1800, 900);
    l_otest.move(50,50);
    l_otest.show();


    return l_oApp.exec();
}
