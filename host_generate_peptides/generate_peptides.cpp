// generate_peptides.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include "PCIE.h"
#include <fstream>


#define DEVELOPER_MODE 1

#define PCIE_CORE_GENERATE_PEPTIDES_BASE 0x4000010

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

struct parameters {
	uint32_t min_mass;
	uint32_t max_mass;
	uint32_t min_length;
	uint32_t max_length;
	uint32_t seed;
	uint32_t decoy_format;
    uint32_t missed_cleavages;
    uint32_t last_address;
};

typedef enum {
	MIN_MASS = 0,
	MAX_MASS,
	MIN_LENGTH,
	MAX_LENGTH,
	SEED,
	DECOY_FORMAT,
    MISSED_CLEAVAGES,
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
	else if (strcmp(param, "--missed_cleavages") == 0)
		p = MISSED_CLEAVAGES;
	else
		p = UNKOWN_PARAMETER;

	return p;
}

int write_to_peptram(std::ofstream &file, char* buffer, uint64_t bytes_written);

int main(int argc, char* argv[])
{
	int args = 1;
	char param[20];
	char par_value[20];
	int param_type;
	struct parameters compute_parameters;
	bool error = false;
	std::ifstream fasta_file;
	std::string line1, line2;
	std::string protein_id;
	std::string str_prot_length;
	char* buffer;
	char* buffer_loc;
	int n1, n2, n3;
	uint64_t bytes_written = 0;
	bool bpass = true;
	void* lib_handle;
	uint32_t buffer_size = DMA_CHUNK_SIZE;
	uint32_t pro_num = 0;
    std::string empty;
    int bts;

    //Setting default compute parameters
    compute_parameters.min_mass = 200;
    compute_parameters.max_mass = 7200;
    compute_parameters.min_length = 6;
    compute_parameters.max_length = 50;
    compute_parameters.decoy_format = 0;
    compute_parameters.seed = 1;
    compute_parameters.missed_cleavages = 0;

#ifdef DEVELOPER_MODE
    std::ofstream peptide_ram;
#else
    PCIE_HANDLE hPCIE;
#endif

	buffer = (char*)malloc(buffer_size);

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
    peptide_ram.open("peptide_ramh.txt");
#endif

	if (argc < 2) {
		printf("Number of arguments cannot be less than 1.\n");
	}
	else {
		while (args < argc && !error) {
			strcpy(param, *(argv + args));
			args++;
			if (param[0] == '-' && param[1] == '-') {
				param_type = compare_param(param);
				switch (param_type) {
				case MIN_MASS:
					strcpy(par_value, *(argv + args + 1));
					compute_parameters.min_mass = atoi(par_value);
					args = args + 2;
					break;
				case MAX_MASS:
					strcpy(par_value, *(argv + args + 1));
					compute_parameters.max_mass = atoi(par_value);
					args = args + 2;
					break;
				case MIN_LENGTH:
					strcpy(par_value, *(argv + args + 1));
					compute_parameters.min_length = atoi(par_value);
					args = args + 2;
					break;
				case MAX_LENGTH:
					strcpy(par_value, *(argv + args + 1));
					compute_parameters.max_length = atoi(par_value);
					args = args + 2;
					break;
				case SEED:
					strcpy(par_value, *(argv + args + 1));
					compute_parameters.seed = atoi(par_value);
					args = args + 2;
					break;
				case DECOY_FORMAT:
					strcpy(par_value, *(argv + args + 1));
					compute_parameters.decoy_format = atoi(par_value);
					args = args + 2;
					break;
                case MISSED_CLEAVAGES:
					strcpy(par_value, *(argv + args + 1));
					compute_parameters.missed_cleavages = atoi(par_value);
					args = args + 2;
					break;
				case UNKOWN_PARAMETER:
					error = true;
					break;
				}
			}
			else {
				fasta_file.open(param, std::ios::in);

				buffer_loc = buffer;
               // memcpy(buffer_loc, &compute_parameters, sizeof(compute_parameters));
                buffer_loc = buffer_loc + 64;
                bytes_written = bytes_written + 64;
				while (std::getline(fasta_file, line1)) {
					if (line1 != "" && line1.at(0) == '>') {
						protein_id = line1.substr(0, line1.find(' ')) + '>';
						n1 = protein_id.length();
						pro_num = pro_num + 1;
						//printf("Protein Number: %d\n\t %s", pro_num, protein_id);
						std::getline(fasta_file, line2);
                       // line2 = '>' + line2;
						n2 = line2.length();
                        n3 = 4;
						if (bytes_written + 64 + n2 < DMA_CHUNK_SIZE) {
							if (buffer_loc) {
								strcpy(buffer_loc, protein_id.c_str());
							    buffer_loc = buffer_loc + 13;
								memcpy(buffer_loc, &n2, sizeof(n2));
							    //buffer_loc = buffer_loc + n3;
                                buffer_loc = buffer_loc + 51;
								strcpy(buffer_loc, line2.c_str());
								buffer_loc = buffer_loc + n2;
							//	bytes_written = bytes_written + n1 + n2 + n3;
                                bytes_written = bytes_written + 64 + n2;
                            //    bts = (n1 + n2 + n3)%64;
                                bts = (64 + n2)%64;
                                if(bts != 0){
                                    empty = std::string(64-bts, '-');
                                    strcpy(buffer_loc, empty.c_str());
                                    buffer_loc = buffer_loc + 64-bts;
                                    bytes_written += 64-bts;
                                }
							}
						}
						else {
                        #ifndef DEVELOPER_MODE
							bpass = PCIE_DmaWrite(hPCIE, DEMO_PCIE_DDR4A_MEM_ADDR, buffer, bytes_written);
						#else
                            bpass = write_to_peptram(peptide_ram, buffer, bytes_written);
                        #endif
                        }

					}
				}
                compute_parameters.last_address = bytes_written/64;
                memcpy(buffer, &compute_parameters, sizeof(compute_parameters));
				fasta_file.close();
                #ifndef DEVELOPER_MODE
                    bpass = PCIE_DmaWrite(hPCIE, DEMO_PCIE_DDR4A_MEM_ADDR, buffer, bytes_written);
                #else
                    bpass = write_to_peptram(peptide_ram, buffer, bytes_written);
                #endif
			}
		}
		free(buffer);
    #ifndef DEVELOPER_MODE
		PCIE_Close(hPCIE);
		PCIE_Unload(lib_handle);
    #else
        peptide_ram.close();
    #endif
		printf("Number of bytes written: %d\n", bytes_written);
	}
return 0;
}


int write_to_peptram(std::ofstream &file, char* buffer, uint64_t bytes_written){
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
