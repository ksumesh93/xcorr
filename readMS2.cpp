#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "readfile.h"
using namespace std;



int main()
{

	extractFasta();
	extractMS2("demo.ms2", "sparse.txt");
	extractMS2("theoretical.ms2", "spar_th.txt");

	return 0;
}



