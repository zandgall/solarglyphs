#include "IntArrayTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

IntArrayTag::IntArrayTag()
{
}


IntArrayTag::~IntArrayTag()
{
}

void IntArrayTag::load(byte* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++)
		ss << bytes[i + index + 3];
	name = (ss.str());

	index += 3 + namelength;

	int size = *(int*)(new byte[4]{ bytes[index + 3], bytes[index + 2], bytes[index + 1], bytes[index] });
	data = vector<int>();
	for (int i = 0; i < size; i++)
		data.push_back(*(int*)(new byte[4]{ bytes[index + i*4 + 7], bytes[index + i*4 + 6], bytes[index + i*4 + 5], bytes[index + i*4 + 4] }));
}

int IntArrayTag::next(byte* bytes, int index) {
	return index + 7 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2] + data.size()*4;
}

vector<byte> IntArrayTag::getBytesValue() {
	vector<byte> out = vector<byte>();
	for (int i = 0; i < data.size(); i++) {
		out.push_back((data.at(i) & 0xff'00'00'00) >> 24);
		out.push_back((data.at(i) & 0x00'ff'00'00) >> 16);
		out.push_back((data.at(i) & 0x00'00'ff'00) >> 8);
		out.push_back((data.at(i) & 0x00'00'00'ff));
	}
	return out;
}