#include "LongTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

LongTag::LongTag()
{
}


LongTag::~LongTag()
{
}

void LongTag::load(byte* bytes, int index) {
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
	//data = (long long)bytes[index] * pow(16, 14) + (long long)bytes[index + 1] * pow(16, 12) + (long long)bytes[index + 2] * pow(16, 10) + (long long)bytes[index + 3] * pow(16, 8) + (long long)bytes[index+4] * pow(16, 6) + (long long)bytes[index+5] * pow(16, 4) + (long long)bytes[index + 6] * pow(16, 2) + (long long)bytes[index + 7];
	data = *(long long*)(new byte[8]{ bytes[index + 7], bytes[index + 6], bytes[index + 5], bytes[index + 4], bytes[index + 3], bytes[index + 2], bytes[index + 1], bytes[index + 0] });
}

int LongTag::next(byte* bytes, int index) {
	return index + 11 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
}

vector<byte> LongTag::getBytesValue() {
	vector<byte> out;
	out.push_back((data & 0xff'00'00'00'00'00'00'00) >> 56);
	out.push_back((data & 0x00'ff'00'00'00'00'00'00) >> 48);
	out.push_back((data & 0x00'00'ff'00'00'00'00'00) >> 40);
	out.push_back((data & 0x00'00'00'ff'00'00'00'00) >> 32);
	out.push_back((data & 0x00'00'00'00'ff'00'00'00) >> 24);
	out.push_back((data & 0x00'00'00'00'00'ff'00'00) >> 16);
	out.push_back((data & 0x00'00'00'00'00'00'ff'00) >> 8);
	out.push_back((data & 0x00'00'00'00'00'00'00'ff));
	return out;
}