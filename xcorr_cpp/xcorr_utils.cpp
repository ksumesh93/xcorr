#include <iostream>
#include <vector>
#include "spectrum.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <list>
#include "xcorr_utils.h"
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <mutex>
#include <atomic>
#include <cmath>

std::atomic<unsigned int> global_counter{0};
std::atomic<unsigned int>  pointer{0};
std::mutex write_mutex;


char *data_write {NULL};


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
        std::list<float> mz, intensity;
        float mzv, intensityv;
        unsigned int in_page {0};
        float value;

        while(std::getline(spectra_fptr, line)){
            //byte_cur += line.length() + 1;
            if(line.at(0) == 'S'){
                if(start_spectrum){
                    spec->set_length(count);

                    // Later on we convert this loop to a memcpy as well, but let it be for a while
                    for(unsigned int i=0; i<count; i++){
                        // Selecting the address inside the 4KB buffer to write to
                        value = mz.front();
                        memcpy((void*)(data + in_page), (void*) (&value), sizeof(float));
                        mz.pop_front();

                        value = intensity.front();
                        memcpy((void*)(data + in_page + 4), (void*) (&value), sizeof(float));
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

                mzv = std::stof(words[0]);
                intensityv = std::stof(words[1]);
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
            value = mz.front();
            memcpy((void*)(data + i*8), (void*) (&value), sizeof(float));
            mz.pop_front();

            value = intensity.front();
            memcpy((void*)(data + i*8 + 4), (void*) (&value), sizeof(float));
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
char* map_random(const char *path, int byte_start, int byte_end, int *size_u, int *fd){
    char *data = NULL;
    // Open the file for writing
    //*fd = open(path, O_RDONLY);

    //Find the starting and ending boundary of the page
    int b_start = byte_start - (byte_start % 4096);
    int b_end = byte_end + (4096 - (byte_end % 4096));
    int size = b_end - b_start;
    // std::cout << byte_start << "," << b_start << std::endl;
    // std::cout << byte_end << "," << b_end << std::endl;


    //Mapping to a buffer of size 4KB
    data = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, *fd, b_start);
    *size_u = size; 

    return data;
}

void unmap_random(char *data, size_t length){
    munmap(data, length);
    data = NULL;
}

void normalizeEachRegion(
  std::vector<float>& observed,  ///< intensities to normalize
  const std::vector<float>& max_intensity_per_region, ///< the max intensity in each 10 regions -in
  int region_selector ///< the size of each regions -in
  )
{
  int region_idx = 0;
  unsigned int next_region = region_selector;
  float max_intensity = max_intensity_per_region[region_idx];

  // normalize each region
  for (unsigned int i = 0; i < observed.size(); ++i) {
    // increment the region index and update max_intensity if this
    // peak is in the next region
    if (i >= next_region && region_idx < (NUM_REGIONS - 1)) {
      max_intensity = max_intensity_per_region[++region_idx];
      next_region += region_selector;
    }
    // normalize intensity to max 50
    observed[i] = (max_intensity != 0)
      ? (observed[i] / max_intensity) * MAX_PER_REGION
      : 0.0;
  }
}

void process_spectra(std::vector<spectrum> spec_index, int *fd, int *fd_write, int stop_after, int id){

    char *data {NULL};
    char buffer[100];
    int size;
    long unsigned int index;
    spectrum *obj;
    float previous;
    float *observed_;
    unsigned int rem {0}, in_page {0}, in_line {0}, bytes{0}, partial{0};
    std::vector<std::tuple <int,float>> reduced_storage;

    float precursor_mz, experimental_mass_cut_off, sp_max_mz;

    

    // std::cout << "Executing thread number: " << num << std::endl;
    
    
    while(global_counter < spec_index.size()){
        index = global_counter++;
        obj = &spec_index[index];
        data = map_random("binary_spectra.bin", obj->get_start(), obj->get_end(), &size, fd);

        obj->set_data(data);
        precursor_mz = obj->get_mass()/obj->get_charge();
        experimental_mass_cut_off = precursor_mz * obj->get_charge() + 50;

        if(experimental_mass_cut_off > 512){
            int x = (int)experimental_mass_cut_off / 1024;
            float y = experimental_mass_cut_off - (1024 * x);
            sp_max_mz = x * 1024;

            if(y > 0){
            sp_max_mz += 1024;
            }
        }

        std::vector<float> observed(INTEGERIZE(sp_max_mz, BIN_SIZE, BIN_OFFSET), 0);
        // Store the max intensity in entire spectrum
        float max_intensity_overall = 0.0;
        // store the max intensity in each 10 regions to later normalize
        std::vector<float> max_intensity_per_region(NUM_REGIONS, 0);

        // while there are more peaks to iterate over..
        // find the maximum peak m/z (location)
        float max_peak = 0.0;

        for(unsigned int x = 0; x < obj->get_length() ; x++) {
            float peak_location = obj->get_mz(x);
            if (peak_location < experimental_mass_cut_off && peak_location > max_peak
                && obj->get_intensity(x) > 0) {
                max_peak = peak_location;
            }
        }

        int region_selector = INTEGERIZE(max_peak, BIN_SIZE, BIN_OFFSET) / NUM_REGIONS + 1;
        float tolerance = 10;

        // while there are more peaks to iterate over..
        // bin peaks, adjust intensties, find max for each region
        for (unsigned int x = 0; x < obj->get_length() ; x++) {
            float peak_location = obj->get_mz(x);

            // skip all peaks larger than experimental mass
            // skip all peaks within precursor ion mz +/- 15
            if (peak_location > experimental_mass_cut_off ||
                (stop_after >= REMOVE_PRECURSOR &&
                peak_location < precursor_mz + tolerance &&
                peak_location > precursor_mz - tolerance)) {
            continue;
            }

            // map peak location to bin
            int mz = INTEGERIZE(peak_location, BIN_SIZE, BIN_OFFSET);
            int region = mz / region_selector;

            // don't let index beyond array
            if (region == NUM_REGIONS && mz < experimental_mass_cut_off) {
            // Force peak into lower bin
                region = NUM_REGIONS - 1;
            } else if (region >= NUM_REGIONS) {
            // Skip peak altogether
                continue;
            }

            // get intensity
            // sqrt the original intensity
            float intensity = (stop_after >= SQUARE_ROOT)
            ? sqrt(obj->get_intensity(x)) : obj->get_intensity(x);

            // Record the max intensity in the full spectrum
            if (intensity > max_intensity_overall) {
                max_intensity_overall = intensity;
            }

            // set intensity in array with correct mz, only if max peak in the bin
            if (observed[mz] < intensity) {
                observed[mz] = intensity;
                // check if this peak is max intensity in the region(one out of 10)
                if (max_intensity_per_region[region] < intensity) {
                    max_intensity_per_region[region] = intensity;
                }
            }
        }

        // For compatibility with SEQUEST drop peaks with intensity less than 1/20 of
        // the overall max intensity.
        if (stop_after >= REMOVE_GRASS) {
            for (std::vector<float>::iterator i = observed.begin(); i != observed.end(); i++) {
            if (*i <= 0.05 * max_intensity_overall) {
                *i = 0.0;
            }
            }
        }

        // normalize each 10 regions to max intensity of 50
        if (stop_after >= TEN_BIN) {
            normalizeEachRegion(observed, max_intensity_per_region, region_selector);
        }

        observed_ = (float*)calloc(observed.size(), sizeof(float));
        copy(observed.begin(), observed.end(), observed_);

        if (stop_after == XCORR) {
            // TODO maybe replace with a faster implementation that uses cum distribution
            for (unsigned int i = 0; i < observed.size(); i++) {
                for (unsigned int j = i - MAX_XCORR_OFFSET; j <= i + MAX_XCORR_OFFSET; j++) {
                    if (j > 0 && j < observed.size()) {
                    observed_[i] -= (observed[j] / (MAX_XCORR_OFFSET * 2.0 + 1));
                    }
                }
            }

            previous = -1;
            for(unsigned int i = 0; i < observed.size(); i++){
                if(observed_[i] != previous){
                    reduced_storage.push_back(std::tuple<int,float>(i, observed_[i]));
                    previous = observed_[i];
                }
            }
        }

        // std::cout << "Counter: " << global_counter << ", ID: " << obj->get_id() << ", Mass: " << obj->get_mass() << ", Charge: " 
        // << obj->get_charge() << ", Start: " << obj->get_start() << ", End: " << obj->get_end() << ", Size: " << size << std::endl;

        // for(unsigned int x = 0; x < reduced_storage.size() ; x++){
        //     std::cout << std::get<0>(reduced_storage[x])  << "," << std::get<1>(reduced_storage[x]) << std::endl;
        // }

        write_mutex.lock();

        if(data_write == NULL){ 
            // Allocate the memory for newly created file
            posix_fallocate(*fd_write, 0, 4096);
            //Mapping to a buffer of size 4KB
            data_write = (char *)mmap(NULL, 4096, PROT_WRITE, MAP_SHARED, *fd_write, 0);
            in_page = 0;
        }
        else{
            in_page = pointer % 4096;
        }

        in_line = sprintf(buffer, "ID: %d, Mass: %f, Charge: %d\n", obj->get_id(), obj->get_mass(), obj->get_charge());
        bytes = in_line;
        //Write first line in the file
        if(in_page + bytes <= 4096){ 
            memcpy((void*)(data_write + in_page), (void*) (buffer), bytes);
            pointer += bytes;
            in_page += bytes;
            rem = 0;
            bytes = 0;
        }
        else{
            partial = 4096 - in_page;
            memcpy((void*)(data_write + in_page), (void*) (buffer), partial);
            rem = bytes - partial;
            pointer += partial;
            in_page += partial;
            // Add another page into the file
            posix_fallocate(*fd_write, pointer, 4096);      
            //Unmap the current page 
            munmap(data_write, 4096);
            //Mapping to the extended buffer
            data_write = (char *)mmap(NULL, 4*1024, PROT_READ | PROT_WRITE, MAP_SHARED, *fd_write, pointer);
            in_page = 0;
            //Write the remaining characters from previous line
            memcpy((void*)(data_write + in_page), (void*) (buffer + partial), rem);
            in_page += rem;
            pointer += rem;
            partial = 0;
            rem = 0;
        }
        

        for(unsigned int x = 0; x < reduced_storage.size() ; x++){
            in_line = sprintf(buffer, "%d,%0.5f\n", std::get<0>(reduced_storage[x]), std::get<1>(reduced_storage[x]));
            bytes = in_line;
            if(in_page + bytes <= 4096){ 
                memcpy((void*)(data_write + in_page), (void*) (buffer + partial), bytes);
                pointer += bytes;
                in_page += bytes;
                partial = 0;
                rem = 0;
                in_line = 0;
            }
            else{
                partial = 4096 - in_page;
                memcpy((void*)(data_write + in_page), (void*) (buffer), partial);
                rem = bytes - partial;
                pointer += partial;
                in_page += partial;

                // Add another page into the file
                posix_fallocate(*fd_write, pointer, 4096);      
                //Unmap the current page 
                munmap(data_write, 4096);
                //Mapping to the extended buffer
                data_write = (char *)mmap(NULL, 4*1024, PROT_READ | PROT_WRITE, MAP_SHARED, *fd_write, pointer);
                in_page = 0;
                //Write the remaining characters from previous line
                memcpy((void*)(data_write + in_page), (void*) (buffer + partial), rem);
                in_page += rem;
                pointer += rem;
                partial = 0;
                rem = 0;
            }
            
        }

        write_mutex.unlock();

        unmap_random(data, size);
    // }while(global_counter < spec_index.size());
    }


}