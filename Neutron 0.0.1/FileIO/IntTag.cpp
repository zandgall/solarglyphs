#include "IntTag.h"
#include "FileChooser.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

IntTag::IntTag()
{
}


IntTag::~IntTag()
{
}


void IntTag::load(byte* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];

	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << bytes[i + index + 3];
	}
	name = (ss.str());

	index += 3 + namelength;
	data = *(int*) (new byte[4]{ bytes[index + 3], bytes[index + 2], bytes[index + 1], bytes[index] });
}

int IntTag::next(byte* bytes, int index) {
	return index + 7 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
}

vector<byte> IntTag::getBytesValue() {
	vector<byte> out;
	out.push_back((data & 0xff'00'00'00) >> 24);
	out.push_back((data & 0x00'ff'00'00) >> 16);
	out.push_back((data & 0x00'00'ff'00) >> 8);
	out.push_back(data & 0x00'00'00'ff);
	return out;
}