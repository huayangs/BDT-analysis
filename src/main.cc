#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#include "MyInterface.hh"

using namespace std;

int main( int argc, char **argv )
{
    MyInterface * interface= new MyInterface();
    interface->run_interface(argc,argv);
    delete interface;
}
