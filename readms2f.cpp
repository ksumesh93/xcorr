#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int extractMS2(ifstream* MS2file, ofstream* memory);
string float_hex(float number);

int main()
{
	ifstream ms2f;
	ofstream memory;
	string line;
	bool start_reading = 0;
	string delimiter = " ";
	string m_z;
	string inten;
	string write_to_file;
	float mass_z = 0;
	float intensity = 0;
	int n1 = 0;
	int n2 = 0;
	int address = 0;

	memory.open("memory.hex", ios::out);

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
			mass_z = stof(m_z);
			intensity = stof(inten);
			cout << "m/z: " << std::hex <<  mass_z << ", intensity: " << intensity << endl;

			/*From this point, convert the floating point values into characters and write them in to file*/
			write_to_file = float_hex(mass_z);
			memory << write_to_file;
			write_to_file = float_hex(intensity);
			memory << write_to_file;

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
