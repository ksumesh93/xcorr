#include <iostream>
#include <vector>
#include "spectrum.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "xcorr_utils.h"


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

std::vector<spectrum> create_index(const char *path){
    /* File handles for reading spectra info */
    std::fstream spectra_fptr;
    spectra_fptr.open(path, std::ios::in);

    std::vector<spectrum> spec_index;

    /* Read the file line by line to get only info */
    if(spectra_fptr.is_open()){
        std::string line;
        std::string token;
        unsigned byte_cur {0};
        bool start_spectrum {false};
        bool start_counting{false};
        unsigned count {0};
        spectrum *spec;
        std::vector<std::string> words; 

        while(std::getline(spectra_fptr, line)){
            byte_cur += line.length() + 1;
            if(line.at(0) == 'S'){
                if(start_spectrum){
                    spec->set_end(byte_cur - (line.length() + 1));
                    spec->set_length(count);
                    spec_index.push_back(*spec);
                    count = 0;
                }
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
                count++;
            }
        }
        
        spec->set_end(byte_cur - (line.length() + 1));
        spec->set_length(count);
        spec_index.push_back(*spec);
        spectra_fptr.close();
    }

    return spec_index;
}
