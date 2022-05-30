#include <iostream>
#include <vector>
#include <fstream>
#include <string>
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
    std::vector<spectrum> spec_index = create_index(argv[1]);

    std::cout << spec_index.size() << std::endl;
    for(unsigned int i = 0; i < spec_index.size(); i++){
        std::cout << "ID: " << spec_index[i].get_id() << ", Mass: " << spec_index[i].get_mass() << ", Charge: " 
        << spec_index[i].get_charge() << ", Start: " << spec_index[i].get_start() << ", End: " << spec_index[i].get_end() << std::endl;
    }

    return STATUS_SUCCESS;
}

