#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include "xcorr.h"
#include "mappedfile.h"
#include "spectrum.h"
#include "xcorr_utils.h"

int main(int argc, char **argv){

    /* Check for number of arguments */
    if(argc < 3){
        std::cout << "Error! Usage: xcorr [Spectra file] [Fasta file]" << std::endl;
        return ERR_INVALID_ARGS;
    }

    /* Create the vector for spectrum class object */
    std::vector<spectrum> spec_index = create_index(argv[1], "binary_spectra.bin");

    std::cout << spec_index.size() << std::endl;
    for(unsigned int i = 0; i < spec_index.size(); i++){
        std::cout << "ID: " << spec_index[i].get_id() << ", Mass: " << spec_index[i].get_mass() << ", Charge: " 
        << spec_index[i].get_charge() << ", Start: " << spec_index[i].get_start() << ", End: " << spec_index[i].get_end() << std::endl;
    }

    // std::fstream exfile; 
    // exfile.open("textex", std::ios::out);
    // for(int i=0; i<8192; i++){
    //     if((i+1) % 256 == 0)
    //         exfile << std::endl;
    //     else
    //         exfile << i+1 << ",";
    // }



    //mapped_file map("textex"); // maps the file into memory

	// write size bytes from the mapped memory to cout
    //std::cout << map.length() << std::endl;
    //memset((void*)*map, 44,  map.length());
	//std::cout.write(*map, map.length());
    //std::cout << std::endl;

    return STATUS_SUCCESS;
}

