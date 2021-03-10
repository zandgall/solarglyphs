#pragma once
#include "Tag.h"
#include "FileChooser.h"
#include <string>
class ShortTag : virtual public Tag {
public:
	ShortTag();
	~ShortTag();
	void load(byte* bytes, int index);
	int next(byte* bytes, int index);
	short& getValue() {
		return data;
	}
	std::vector<byte> getBytesValue();
	std::vector<byte> outBytes() {
		std::vector<byte> out = std::vector<byte>();
		out.push_back(0x02);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 16);
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
		return 2;
	}
private:
	std::string name;
	short data;
};

