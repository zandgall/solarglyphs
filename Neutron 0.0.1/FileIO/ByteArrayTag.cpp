#include "ByteArrayTag.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;


ByteArrayTag::ByteArrayTag()
{
}


ByteArrayTag::~ByteArrayTag()
{
}

void ByteArrayTag::load(byte* bytes, int index) {
	cout << "Loading byte array";
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	cout << ": " << namelength;
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
		cout << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = (ss.str());

	cout << ": " << name;
	index += 3 + namelength;

	int size = (int)bytes[index]*16*16*16*16*16*16 + (int)bytes[index+1]*16*16*16*16 + (int)bytes[index+2]*16*16+ (int)bytes[index+3];
	cout << " (" << size << "b) " << (int) bytes[index] << " : " << (int) bytes[index+1] << " : " << (int) bytes[index+2] << " : " << (int) bytes[index+3] << endl;
	data = vector<byte>();
	for (int i = 0; i < size; i++) {
		data.push_back(bytes[index + i + 4]);
	}
}

int ByteArrayTag::next(byte* bytes, int index) {
	return index + 7 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2] + data.size();
}

vector<byte> ByteArrayTag::getBytesValue() {
	return data;
}