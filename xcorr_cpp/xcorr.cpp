#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <thread> 
#include <string.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <atomic>
#include <unistd.h>
#include <mutex>
#include <cmath>
#include <tuple>
#include "xcorr.h"
#include "mappedfile.h"
#include "spectrum.h"
#include "xcorr_utils.h"





int main(int argc, char **argv){

    std::vector<std::thread> threads;
    int fd_read, fd_write;

    /* Check for number of arguments */
    if(argc < 3){
        std::cout << "Error! Usage: xcorr [Spectra file] [Fasta file]" << std::endl;
        return ERR_INVALID_ARGS;
    }

    /* Create the vector for spectrum class object */
    std::vector<spectrum> spec_index = create_index(argv[1], "binary_spectra.bin");

    std::cout << spec_index.size() << std::endl;
    //for(unsigned int i = 0; i < spec_index.size(); i++){
    fd_read = open("binary_spectra.bin", O_RDONLY);
    fd_write = open("processed.ms2", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    for(unsigned int i = 0; i < 44; i++){
        threads.push_back(std::thread(process_spectra, spec_index, &fd_read, &fd_write, XCORR, i));
    } 

    std::cout << "synchronizing all threads...\n";
    for (auto& th : threads) th.join();
    close(fd_read);


    return STATUS_SUCCESS;
}

