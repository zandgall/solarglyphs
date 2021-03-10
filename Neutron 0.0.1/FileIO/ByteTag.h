#include "Tag.h"
#include "FileChooser.h"
#pragma once
class ByteTag : virtual public Tag {
public:
	ByteTag();
	~ByteTag();
	void load(byte* bytes, int index);
	int next(byte* bytes, int index);
	byte& getValue() {
		return data;
	}
	std::vector<byte> getBytesValue();
	std::vector<byte> outBytes() {
		std::vector<byte> out = std::vector<byte>();
		out.push_back(0x01);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 16);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		out.push_back(data);
		return out;
	};
	const char* getName() {
		return name.c_str();
	}
	unsigned int id() {
		return 1;
	}
private:
	std::string name;
	byte data;
};

