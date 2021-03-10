#include "DoubleTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

DoubleTag::DoubleTag(){}


DoubleTag::~DoubleTag(){}

void DoubleTag::load(byte* bytes, int index) {
	cout << "Loading short";
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	cout << ": " << namelength;
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
		cout << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = (ss.str());

	cout << ": " << name << " (" << ss.str() << ")" << endl;
	index += 3 + namelength;
	//data = (double)bytes[index] * pow(16, 14) + (double)bytes[index + 1] * pow(16, 12) + (double)bytes[index + 2] * pow(16, 10) + (double)bytes[index + 3] * pow(16, 8) + (double)bytes[index + 4] * pow(16, 6) + (double)bytes[index + 5] * pow(16, 4) + (double)bytes[index + 6] * pow(16, 2) + (double)bytes[index + 7];
	union d_ouble {
		double d;
		char data[sizeof(double)];
	};
	//d_ouble conv;
	//conv.data[7] = bytes[index];
	//conv.data[6] = bytes[index+1]; 
	//conv.data[5] = bytes[index+2]; 
	//conv.data[4] = bytes[index+3]; 
	//conv.data[3] = bytes[index+4]; 
	//conv.data[2] = bytes[index+5]; 
	//conv.data[1] = bytes[index+6];
	//conv.data[0] = bytes[index+7];
	//data = (double) (new byte[8]{ bytes[index], bytes[index + 1], bytes[index + 2], bytes[index + 3], bytes[index + 4], bytes[index + 5], bytes[index + 6], bytes[index + 7] });
	//data = conv.d;
	data = *(double*)(new byte[8]{ bytes[index + 7], bytes[index + 6], bytes[index + 5], bytes[index + 4], bytes[index + 3], bytes[index + 2], bytes[index + 1], bytes[index + 0] });
}

int DoubleTag::next(byte* bytes, int index) {
	return index + 11 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
}

vector<byte> DoubleTag::getBytesValue() {
	vector<byte> out;

	union u_double {
		double d;
		char data[sizeof(double)];
	};

	u_double f;
	f.d = data;

	out.push_back(f.data[7]);
	out.push_back(f.data[6]);
	out.push_back(f.data[5]);
	out.push_back(f.data[4]);
	out.push_back(f.data[3]);
	out.push_back(f.data[2]);
	out.push_back(f.data[1]);
	out.push_back(f.data[0]);
	return out;
}