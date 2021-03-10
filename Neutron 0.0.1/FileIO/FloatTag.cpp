#include "FloatTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

FloatTag::FloatTag(){}


FloatTag::~FloatTag(){}

void FloatTag::load(byte* bytes, int index) {
	cout << "Loading float";
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
	//data = (float)bytes[index] * pow(16, 6) + (float)bytes[index + 1]*pow(16, 4) + (float)bytes[index + 2] * pow(16, 2) + (float)bytes[index + 3];
	data = *(float*)(new byte[4]{ bytes[index + 3], bytes[index + 2], bytes[index + 1], bytes[index] });
}

int FloatTag::next(byte* bytes, int index) {
	return index + 7 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
}

vector<byte> FloatTag::getBytesValue() {
	vector<byte> out;

	union u_float {
		float f;
		char data[sizeof(float)];
	};

	u_float f;
	f.f = data;

	out.push_back(f.data[3]);
	out.push_back(f.data[2]);
	out.push_back(f.data[1]);
	out.push_back(f.data[0]);
	return out;
}