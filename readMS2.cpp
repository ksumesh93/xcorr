#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int extractMS2(ifstream* MS2file, ofstream* memory);
string float_hex(float number);

struct value_pair {
	float mz_value;
	float inten_value;
};

int main()
{
	value_pair *spect_inst;
	ifstream ms2f;
	ofstream memory;
	string line;
	bool start_reading = 0;
	string delimiter = " ";
	string m_z;
	string inten;
	string write_to_file;
	int n1 = 0;
	int n2 = 0;
	int address = 0;
	char* mem_block;

	memory.open("memory.bin", ios::out);

	int count = 0;
	ms2f.open("demo.ms2");
	while (getline(ms2f, line))
	{
		if (line.compare(0, 1, "Z") == 0)
		{
			start_reading = 1;
		}
		else if (line.compare(0, 1, "S") == 0 && start_reading)
		{
			start_reading = 0;
			break;
		}
		else if (start_reading)
		{
			n1 = line.find(delimiter);
			m_z = line.substr(0, n1);
			inten = line.substr(n1 + 1, line.length());
			
			spect_inst = new value_pair;
			
			spect_inst->mz_value = stof(m_z);
			spect_inst->inten_value = stof(inten);
			cout << "m/z: " << std::hex << spect_inst->mz_value << ", intensity: " << spect_inst->mz_value << endl;
			mem_block = (char*)(spect_inst);
			/*From this point, write the float values to file*/
			//write_to_file = float_hex(mass_z);
			//memory << write_to_file;
			//write_to_file = float_hex(intensity);
			//memory << write_to_file;
			memory.write(mem_block, sizeof(value_pair));

		}
		
	}
	ms2f.close();
	memory.close();

	return 0;
}

string float_hex(float number)
{
	char *point;
	stringstream readhex;
	string returnstring = "00000000";
	string temp;
	point = (char*)&number;

	for (int i = 0; i < 4; i++)
	{
		readhex.str("");
		readhex << hex << (int)(*point & 255);
		temp = readhex.str();
		returnstring.replace(2*i, temp.length(), temp);
		point++;
	}
	return returnstring;
}

int extractMS2(ifstream* MS2file, ofstream* memory)
{

	return 0;
}