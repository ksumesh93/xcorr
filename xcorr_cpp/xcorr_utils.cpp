#include <iostream>
#include <vector>
#include "spectrum.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <list>
#include "xcorr_utils.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>


std::vector<std::string> tokenize(std::string str, char delimiter){
    std::stringstream ss(str);
    std::string token;
    std::vector<std::string> split;
    while(std::getline(ss, token, delimiter)){
        split.push_back(token);
    }
    return split;
}


///////////////////////////////////////////////////////
/// CREATING AN INDEX OF ALL THE SPECTRA //////////////
///////////////////////////////////////////////////////

std::vector<spectrum> create_index(const char *path, const char *bin_path){
    /* File handles for reading spectra info */
    std::fstream spectra_fptr;
    spectra_fptr.open(path, std::ios::in);

    char *data = NULL;
    // Open the file for writing
    int fd = open(bin_path, O_RDWR);
    // Allocate the memory for newly created file
    posix_fallocate(fd, 0, 4096);
    //Mapping to a buffer of size 4KB
    data = (char *)mmap(NULL, 4096, PROT_WRITE, MAP_SHARED, fd, 0);

    std::vector<spectrum> spec_index;

    /* Read the file line by line to get only info */
    /* We are going to record the m/z and intensity pairs in a memory buffer */
    if(spectra_fptr.is_open()){
        std::string line;
        std::string token;
        unsigned byte_cur {0};
        bool start_spectrum {false};
        bool start_counting{false};
        unsigned count {0};
        spectrum *spec;
        std::vector<std::string> words; 
        //Local variables to read the pairs
        std::list<int> mz, intensity;
        int mzv, intensityv;
        unsigned int in_page {0};

        while(std::getline(spectra_fptr, line)){
            //byte_cur += line.length() + 1;
            if(line.at(0) == 'S'){
                if(start_spectrum){
                    spec->set_length(count);

                    // Later on we convert this loop to a memcpy as well, but let it be for a while
                    for(unsigned int i=0; i<count; i++){
                        // Selecting the address inside the 4KB buffer to write to
                        int* value = (int*)(data + in_page);
                        *value = mz.front();
                        mz.pop_front();
                        value = (int*)(data + in_page + 4);
                        *value = intensity.front();
                        intensity.pop_front();
                            

                        byte_cur += 8;
                        in_page += 8;

                        // Check if we have reached the end of the buffer
                        if(in_page % 4096 == 0){
                            // Add another page into the file
                            posix_fallocate(fd, byte_cur, 4096);      
                            //Unmap the current page 
                            munmap(data, 4096);
                            //Mapping to the extended buffer
                            data = (char *)mmap(NULL, 4*1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, byte_cur);
                            // Adjust the index
                            count -= i+1;
                            i = -1;
                            in_page = 0;
                        }
                    }
                    spec->set_end(byte_cur);

                    spec_index.push_back(*spec);
                    count = 0;
                }

                mz.clear();
                intensity.clear();
                words = tokenize(line, '\t');
                
                spec = new spectrum;
                spec->set_id(std::atoi(words[1].c_str()));
                words.clear();
                start_spectrum = true;
                start_counting = false;
            }
            else if(line.at(0) == 'Z'){
                words = tokenize(line, '\t');

                spec->set_charge(std::atoi(words[1].c_str()));
                spec->set_mass(std::atof(words[2].c_str()));
                spec->set_start(byte_cur);
                words.clear();
                start_counting = true;
            }
            else if(start_counting){
                // Tokenize the mass and intensity pairs
                words = tokenize(line, ' ');

                mzv = (int) (65536 * std::stof(words[0]));
                intensityv = (int) (65536 * std::stof(words[1]));
                mz.push_back(mzv);
                intensity.push_back(intensityv);
                words.clear();
                count++;
            }
        }
        spec->set_length(count);

        // Later on we convert this loop to a memcpy as well, but let it be for a while
        for(unsigned int i=0; i<count; i++){
            // Selecting the address inside the 4KB buffer to write to
            int* value = (int*)(data + i*8);
            *value = mz.front();
            
            mz.pop_front();
            value = (int*)(data + i*8 + 4);
            *value = intensity.front();
            intensity.pop_front();

            byte_cur += 8;
            in_page += 8;

            // Check if we have reached the end of the buffer
            if(in_page % 4096 == 0){
                // Add another page into the file
                posix_fallocate(fd, byte_cur, 4096);      
                //Unmap the current page 
                munmap(data, 4096);
                //Mapping to the extended buffer
                data = (char *)mmap(NULL, 4*1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, byte_cur);
                // Adjust the index
                count -= i+1;
                i = -1;
                in_page = 0;
            }
        }
        mz.clear();
        intensity.clear();
        spec->set_end(byte_cur);

        spec_index.push_back(*spec);
        spectra_fptr.close();
    }

    return spec_index;
}


///////////////////////////////////////////////////////
/// CREATING A FUNCTION TO MAP RANDOM REGION //////////
///////////////////////////////////////////////////////
char* map_random(const char *path, int byte_start, int byte_end, int *size_u){
    char *data = NULL;
    // Open the file for writing
    int fd = open(path, O_RDONLY);

    //Find the starting and ending boundary of the page
    int b_start = byte_start - (byte_start % 4096);
    int b_end = byte_end + (4096 - (byte_end % 4096));
    int size = b_end - b_start;
    std::cout << byte_start << "," << b_start << std::endl;
    std::cout << byte_end << "," << b_end << std::endl;


    //Mapping to a buffer of size 4KB
    data = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, b_start);
    *size_u = size; 

    return data;
}