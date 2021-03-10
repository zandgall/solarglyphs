#include "Tag.h"
#include "FileChooser.h"
#pragma once
class LongTag : virtual public Tag {
public:
	LongTag();
	~LongTag();
	void load(byte* bytes, int index);
	int next(byte* bytes, int index);
	long long& getValue() {
		return data;
	}
	std::vector<byte> getBytesValue();
	std::vector<byte> outBytes() {
		std::vector<byte> out = std::vector<byte>();
		out.push_back(0x04);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 8);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		for (int i = 0; i < getBytesValue().size(); i++)
			out.push_back(getBytesValue().at(i));
		return out;
	};
	const char* getName() {
		return name.c_str();
	}
	unsigned int id() {
		return 4;
	}
private:
	std::string name;
	long long data;
};

