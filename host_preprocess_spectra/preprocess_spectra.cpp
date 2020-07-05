// generate_peptides.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include "PCIE.h"
#include <fstream>
#include <sstream>

#define PCIE_CORE_GENERATE_PEPTIDES_BASE 0x4000010

#define DEVELOPER_MODE 1

#define DEMO_PCIE_ONCHIP_MEM_ADDR	0x00000000
#define DEMO_PCIE_DDR4A_MEM_ADDR	0x800000000
#define DEMO_PCIE_DDR4B_MEM_ADDR	0xA00000000
#define DEMO_PCIE_DDR4C_MEM_ADDR	0xC00000000
#define DEMO_PCIE_DDR4D_MEM_ADDR	0xE00000000

#define DMA_CHUNK_SIZE				(1ull*1024*1024*1024) //2GB

struct gpept_registers {
	uint32_t core_status_register;
	uint32_t core_write_address_register;
	uint32_t core_write_size_register;
	uint32_t core_read_address_register;
	uint32_t core_read_size_register;
	uint32_t core_parameter_register_0;
	uint32_t core_parameter_register_1;
	uint32_t core_parameter_register_2;
	uint32_t core_parameter_register_3;
	uint32_t core_parameter_register_4;
	uint32_t core_parameter_register_5;
	uint32_t core_parameter_register_6;
	uint32_t core_parameter_register_7;
	uint32_t core_parameter_register_8;
	uint32_t core_parameter_register_9;
	uint32_t core_parameter_register_10;
	uint32_t core_parameter_register_11;
	uint32_t core_parameter_register_12;
};

struct header{
    uint32_t spec_id;
    uint32_t ion_charge;
    uint32_t precursor_mass;
    uint32_t min_mass;
    uint32_t max_mass;
    uint32_t next_address;
};

typedef enum {
	MIN_MASS = 0,
	MAX_MASS,
	MIN_LENGTH,
	MAX_LENGTH,
	SEED,
	DECOY_FORMAT,
	UNKOWN_PARAMETER
};

int compare_param(char* param)
{
	int p;
	if (strcmp(param, "--min_mass") == 0)
		p = MIN_MASS;
	else if (strcmp(param, "--max_mass") == 0)
		p = MAX_MASS;
	else if (strcmp(param, "--min_length") == 0)
		p = MIN_LENGTH;
	else if (strcmp(param, "--max_length") == 0)
		p = MAX_LENGTH;
	else if (strcmp(param, "--seed") == 0)
		p = SEED;
	else if (strcmp(param, "--decoy_format") == 0)
		p = DECOY_FORMAT;
	else
		p = UNKOWN_PARAMETER;

	return p;
}


int write_to_specram(std::ofstream &file, char* buffer, uint64_t bytes_written);

int main(int argc, char* argv[])
{
//	int args = 1;
	char param[20];
	char par_value[20];
	int param_type;
	struct header spectrum_header;
	bool error = false;
	std::ifstream ms2_file;
	std::string line1, line2;
	uint32_t spectrum_id;
	uint32_t charge_state;
	float precursor_mass;
	char* buffer;
	char* buffer_loc;
	int n1, n2, n3;
	uint64_t bytes_written = 0;
	bool bpass = true;
	void* lib_handle;
	uint32_t buffer_size = DMA_CHUNK_SIZE;
	uint32_t pro_num = 0;
	bool start_reading = false;
	std::string token;
	uint32_t num_peaks = 0;
	float inten, m_z;
	uint32_t inten_int, m_z_int = 0;
	uint32_t pre_mass_int = 0;

#ifdef DEVELOPER_MODE
    std::ofstream spectra_ram;
#else
    PCIE_HANDLE hPCIE;
#endif


#ifndef DEVELOPER_MODE
    lib_handle = PCIE_Load();
    if (!lib_handle) {
        printf("PCIE_Load failed!\r\n");
        return 0;
    }
    hPCIE = PCIE_Open(DEFAULT_PCIE_VID, DEFAULT_PCIE_DID, 0);
    if (!hPCIE) {
        printf("PCIE_Open failed\r\n");
    }
#else
    spectra_ram.open("spectra_ramh.txt");
#endif

	buffer = (char*)malloc(buffer_size);

	if (argc < 1) {
		printf("Number of arguments cannot be less than 1.");
	}
	else {
        strcpy(param, *(argv + 1));
 //       args++;
        ms2_file.open(param, std::ios::in);

        buffer_loc = buffer;
        while (std::getline(ms2_file, line1)) {
            if (line1.at(0) == 'S') {
                std::stringstream tokenize(line1.substr(2, line1.length()-2));
                std::getline(tokenize, token, '\t');
                spectrum_id = stoi(token);
                start_reading = false;
            //	std::memcpy(buffer_loc, &indic, 1);
            //	buffer_loc++;
            //	std::memcpy(buffer_loc, &spectrum_id, sizeof(spectrum_id));
            //	buffer_loc = buffer_loc + sizeof(spectrum_id);
                spectrum_header.spec_id = spectrum_id;
            }
            else if (!start_reading && line1.at(0) == 'Z') {
                std::stringstream tokenize(line1.substr(2, line1.length() - 2));
                std::getline(tokenize, token, '\t');
                charge_state = std::stoi(token);
                std::getline(tokenize, token, '\t');
                precursor_mass = 65536 * std::stof(token);
                pre_mass_int = (uint32_t)(precursor_mass);
                start_reading = true;

                

            //	std::memcpy(buffer_loc, &indic, 1);
            //	buffer_loc++;
            //	std::memcpy(buffer_loc, &charge_state, sizeof(charge_state));
            //	buffer_loc = buffer_loc + sizeof(charge_state);
            //	std::memcpy(buffer_loc, &indic, 1);
            //	buffer_loc++;
            //	std::memcpy(buffer_loc, &pre_mass_int, sizeof(pre_mass_int));
            //	buffer_loc = buffer_loc + sizeof(pre_mass_int);
                
                spectrum_header.ion_charge = charge_state;
                spectrum_header.precursor_mass = precursor_mass;
                spectrum_header.min_mass = 65536*10000;
                spectrum_header.max_mass = 0;

                memcpy(buffer_loc, &spectrum_header, sizeof(spectrum_header));
                buffer_loc += 64;
                bytes_written += 64;
            }
            else if (start_reading && line1.at(0) != 'Z') {
                std::stringstream tokenize(line1);
                std::getline(tokenize, token, ' ');
                inten = stof(token);
                std::getline(tokenize, token, ' ');
                m_z = stof(token);
                inten_int = (uint32_t)(inten* 65536);
                m_z_int = (uint32_t)(65536 * m_z);
                
                if(m_z_int < spectrum_header.min_mass)
                    spectrum_header.min_mass = m_z_int;
                if(m_z_int > spectrum_header.max_mass)
                    spectrum_header.max_mass = m_z_int;


                memcpy(buffer_loc, &inten_int, sizeof(inten_int));
                buffer_loc = buffer_loc + sizeof(inten_int);
                memcpy(buffer_loc, &m_z_int, sizeof(m_z_int));
                buffer_loc = buffer_loc + sizeof(m_z_int);
                bytes_written = bytes_written + 8;
             }
        }
        ms2_file.close();
        #ifndef DEVELOPER_MODE
            bpass = PCIE_DmaWrite(hPCIE, DEMO_PCIE_DDR4A_MEM_ADDR, buffer, bytes_written);
        #else
            bpass = write_to_specram(spectra_ram, buffer, bytes_written);
        #endif
        free(buffer);
    
        #ifndef DEVELOPER_MODE
            PCIE_Close(hPCIE);
            PCIE_Unload(lib_handle);
        #else
            spectra_ram.close();
        #endif


        printf("Number of bytes written: %d\n", bytes_written);
    }

    return 0;
}

int write_to_specram(std::ofstream &file, char* buffer, uint64_t bytes_written){
    bool bracket_start = false;
    char one[2];
    for(uint64_t i=0; i<bytes_written; i=i+64){
        for(int j=i; j<i+64; j++){
           sprintf(one, "%02X", uint8_t(*(buffer + j)));
           file << one ; 

           // file << std::hex <<uint8_t(buffer[j]);

           /* if(buffer[j] == '>')
                bracket_start = !(bracket_start);
            
            if (buffer[j] >= 16 )
                file << std::hex <<uint8_t(buffer[j]);
            else if(!bracket_start){
                file << '0' ;
                file << std::hex <<uint8_t(buffer[j]);
            }*/
        }
        file << "\n";
    }
    return 1;
}
