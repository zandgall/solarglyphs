#pragma once
#include "Tag.h"
#include "FileChooser.h"
class ListTag : virtual public Tag {
public:
	ListTag();
	~ListTag();
	void load(byte* bytes, int index);
	int next(byte* bytes, int index);
	/*union Support {
		Support() {};
		std::vector<byte> b;
		std::vector<short> s;
		std::vector<int> i;
		std::vector<long> l;
		std::vector<float> f;
		std::vector<double> d; 
		~Support() {};
	} Types;*/
	struct Support {
		std::vector<byte> b;
		std::vector<short> s;
		std::vector<int> i;
		std::vector<long long> l;
		std::vector<float> f;
		std::vector<double> d;
		std::vector<std::string> c;
		std::string stringval;
	};
	Support getValue();
	std::vector<byte> getBytesValue();
	std::vector<byte> outBytes() {
		std::vector<byte> out = std::vector<byte>();
		out.push_back(0x09);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 8);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		out.push_back(tagIDs & 0x00'00'00'ff);
		out.push_back((size & 0xff'00'00'00) >> 24);
		out.push_back((size & 0x00'ff'00'00) >> 16);
		out.push_back((size & 0x00'00'ff'00) >> 8);
		out.push_back(size & 0x00'00'00'ff);
		for (int i = 0; i < getBytesValue().size(); i++)
			out.push_back(getBytesValue().at(i));
		return out;
	};
	const char* getName() {
		return name.c_str();
	}
	unsigned int id() {
		return 9;
	}
	int tagIDs, tagsize;
	int size;
private:
	std::string name;
	std::vector<byte> tags;
};
