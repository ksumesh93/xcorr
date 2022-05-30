#ifndef XCORR_UTILS_H
#define XCORR_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include "spectrum.h"

std::vector<std::string> tokenize(std::string str, char delimiter); 
std::vector<spectrum> create_index(const char *path);

#endif