
#ifndef READ_FILE_H
#define READ_FILE_H


struct value_pair {
	float mz_value;
	float inten_value;
};

struct remember{
	float oldval;
	float max[10];
	int values_written;
};
int extractMS2(const char* infile, const char* outfile);
int extractFasta();
int write_sparse(remember *inst, float m, float i, std::ofstream &sparse);
std::string float_hex(float number);

const float amino[] = {
		71.04F,
		0.0F,
		103.01F,
		115.03F,
		129.04F,
		147.07F,
		57.02F,
		137.06F,
		113.08F,
		0.0F,
		128.09F,
		113.08F,
		131.04F,
		114.04F,
		0.0F,
		97.05F,
		128.06F,
		156.10F,
		87.03F,
		101.05F,
		0.0F,
		99.07F,
		186.08F,
		0.0F,
		163.06F,
		0.0F};

#endif