#include <iostream>
#include <sstream>
#include <fstream>
#include "readfile.h"
#include "params.h"
#include <string>
#include <cmath>
#include <algorithm>
using namespace std;

int spectra_written = 0;

int extractMS2(const char* infile, const char* outfile)
{
	value_pair* spect_inst;
	remember* rem_inst = NULL;
	ofstream sparse;
	string line;
	bool start_reading = 0;
	string delimiter = " ";
	string m_z;
	string inten;
	string write_to_file;
	int n1 = 0;
	int n2 = 0;
	int address = 0;
	ifstream MS2file;
	ofstream memory;

	//memory.open("memory.bin", ios::out);
	sparse.open(outfile, ios::out);

	int count = 0;
	MS2file.open(infile);
	while (getline(MS2file, line))
	{
		//Start reading
		if (line.compare(0, 1, "Z") == 0)
		{
			start_reading = 1;
			rem_inst = new remember;
			rem_inst->oldval = 0;
			rem_inst->values_written = 0;
			n1 = line.find("\t");
			rem_inst->charge_state = stof(line.substr(n1 + 1, 1));
			rem_inst->precursor_mass = stof(line.substr(n1+2, line.length()-n1-2));
			//Create an array of all zeros which hold the maximum of each bin
			for (int i = 0; i < 10; i++)
			{
				rem_inst->max[i] = 0;
			}
		}
		//Stop reading
		else if (line.compare(0, 1, "S") == 0 && start_reading)
		{
			start_reading = 0;
			cout << line << endl;
			if (rem_inst != NULL)
			{
				write_sparse(rem_inst, 2032, 0, sparse);
				write_header(rem_inst, sparse);
				delete rem_inst;
			}
		}
		else if (start_reading)
		{
			n1 = line.find(delimiter);
			m_z = line.substr(0, n1);
			inten = line.substr(n1 + 1, line.length());

			spect_inst = new value_pair;

			spect_inst->mz_value = stof(m_z);
			spect_inst->inten_value = stof(inten);

			//Send to memory as a sparse array
			write_sparse(rem_inst, spect_inst->mz_value, spect_inst->inten_value, sparse);

			//Print the values
			//cout << "m/z: " << std::hex << spect_inst->mz_value << ", intensity: " << spect_inst->mz_value << endl;

			//Delete one instance 
			delete spect_inst;
		}
	}
	if (rem_inst != NULL)
	{
		write_sparse(rem_inst, LENGTH_OF_SPECTRUM-16, 0, sparse);
		write_header(rem_inst, sparse);
		delete rem_inst;
	}
	MS2file.close();
	memory.close();
	sparse.close();
	return 0;
}

string float_hex(float number)
{
	char* point;
	stringstream readhex;
	string returnstring = "00000000";
	string temp;
	point = (char*)&number;

	for (int i = 0; i < 4; i++)
	{
		readhex.str("");
		readhex << hex << (int)(*point & 255);
		temp = readhex.str();
		returnstring.replace(2 * i, temp.length(), temp);
		point++;
	}
	return returnstring;
}

int write_sparse(remember* inst, float m, float i, std::ofstream &sparse)
{
	string write_to_file;
	int val_num = 0;
	int old = 0;
	int val_written = inst->values_written;
	int bin = 0;

	old = (int)inst->oldval;
	val_num = ((int)m) - old - 1;

	if (val_num > -1)
	{
		for (int i = 0; i < val_num; i++)
		{
			write_to_file = float_hex(0);
			sparse << write_to_file;
			val_written++;
			if (((VALUES_PER_LINE)+val_written) % (VALUES_PER_LINE) == 0)
			{
				sparse << endl;
			}
		}
		write_to_file = float_hex((float)sqrt(i));
		sparse << write_to_file;
		val_written++;
		if (((VALUES_PER_LINE)+val_written) % (VALUES_PER_LINE) == 0)
		{
			sparse << endl;
		}

		inst->oldval = m;
		inst->values_written = val_written;
		bin = ((int)m) / (LENGTH_OF_SPECTRUM / 10);
		if (inst->max[bin] < (float)sqrt(i))
		{
			inst->max[bin] = (float)sqrt(i);
		}
		sparse.flush();
	}
	return 0;
}

int write_header(remember* inst, std::ofstream& sparse)
{
	string temp;
	const char *message = "SPECTRUM";
	temp = float_hex(*((float*)(message)));
	sparse << temp;
	temp = float_hex(*((float*)(message+4)));
	sparse << temp;
	temp = float_hex(inst->precursor_mass);
	sparse << temp;
	temp = float_hex(inst->charge_state);
	sparse << temp;
	temp = float_hex((float)spectra_written * LENGTH_OF_SPECTRUM * 4);
	sparse << temp;
	for (int i = 0; i < 10; i++)
	{
		temp = float_hex(inst->max[i]);
		sparse << temp;
	}
	temp = float_hex(0);
	sparse << temp;
	sparse << endl;
	return 0;
}

int extractFasta()
{
	ifstream t_pept;
	ofstream theoretical;
	string line;
	string precursor;
	float precursor_mass;
	string aline;
	string command;
	string::iterator it;
	char acid;
	float btype;
	float ytype;
	float* m_z;
	int ind = 0;
	int peptide_count = 0;
	//Extract the fast File to generate target peptides
	system("crux generate-peptides --overwrite T small-yeast.fasta");

	//Move the generated file into current directory
	system("move /y .\\crux-output\\generate-peptides.target.txt peptides.txt");

	t_pept.open("peptides.txt");
	theoretical.open("theoretical.ms2");

	while (getline(t_pept, line))
	{
		stringstream ss(line);
		getline(ss, line, '\t');
		getline(ss, precursor, '\t');
		precursor_mass = stof(precursor);
		//cout << line << "\t" << precursor_mass << endl;
		int n = 2* (line.length());
		m_z = new float[n];
		ind = -2;
		for (it = line.begin(); it < line.end(); it++)
		{
			ind = ind + 2;
			acid = *it;
			btype = amino[acid - 65];
			ytype = precursor_mass - btype;
			if (ind >= 2)
				m_z[ind] = m_z[ind - 2] + btype;
			else
				m_z[ind] = btype + 1;
			m_z[ind + 1] = precursor_mass - m_z[ind] + 1;
			//cout << m_z[ind] << "\t" << m_z[ind + 1] << endl;
		}
		sort(m_z, m_z + n);

		theoretical << "S\t" << peptide_count << "\t" << precursor_mass << endl;
		theoretical << "Z\t2\t" << 2 * precursor_mass << endl;

		for (int i = 0; i < n; i++)
		{
			theoretical << m_z[i] - 1 << "\t" << 25 << endl;
			theoretical << m_z[i] << "\t" << 50 << endl;
			theoretical << m_z[i] + 1 << "\t" << 25 << endl;
 		}

		peptide_count++;
		delete[] m_z;
	}

	return 0;
}