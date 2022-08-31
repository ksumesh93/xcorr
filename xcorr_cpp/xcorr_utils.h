#ifndef XCORR_UTILS_H
#define XCORR_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include "spectrum.h"
#include <atomic>
#include <mutex>

#define BIN_SIZE  0.02
#define BIN_OFFSET 0.4
#define NUM_REGIONS 10

#define REMOVE_PRECURSOR 0
#define SQUARE_ROOT 1
#define REMOVE_GRASS 2
#define TEN_BIN 3
#define XCORR 4

#define INTEGERIZE(VALUE,BIN_SIZE,BIN_OFFSET) \
            ((int)( ( ( VALUE / BIN_SIZE ) + 1.0 ) - BIN_OFFSET ) )

#define MAX_PER_REGION  50
#define MAX_XCORR_OFFSET  75

extern std::atomic<unsigned int> global_counter;
extern std::atomic<unsigned int>  pointer;
extern std::mutex write_mutex;


extern char *data_write;

std::vector<std::string> tokenize(std::string str, char delimiter); 
std::vector<spectrum> create_index(const char *path, const char *bin_path);
char* map_random(const char *path, int byte_start, int byte_end, int *size_u, int *fd);
void unmap_random(char *data, size_t length);
void normalizeEachRegion(
  std::vector<float>& observed,  ///< intensities to normalize
  const std::vector<float>& max_intensity_per_region, ///< the max intensity in each 10 regions -in
  int region_selector );///< the size of each regions -in);
void process_spectra(std::vector<spectrum> spec_index, int *fd, int *fd_write, int stop_after, int id);

#endif