#pragma once
#include "Tag.h"
#include "FileChooser.h"
class StringTag : virtual public Tag {
public:
	StringTag();
	~StringTag();
	void load(byte* bytes, int index);
	int next(byte* bytes, int index);
	std::string& getValue() {
		return data;
	}
	std::vector<byte> getBytesValue();
	std::vector<byte> outBytes() {
		std::vector<byte> out = std::vector<byte>();
		out.push_back(0x08);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 8);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		out.push_back((data.size() & 0x00'00'ff'00) >> 8);
		out.push_back(data.size() & 0x00'00'00'ff);
		for (int i = 0; i < getBytesValue().size(); i++)
			out.push_back(getBytesValue().at(i));
		return out;
	};
	const char* getName() {
		return name.c_str();
	}
	unsigned int id() {
		return 8;
	}
private:
	std::string name;
	std::string data;
};

