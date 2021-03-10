#include "ListTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

ListTag::ListTag()
{
}


ListTag::~ListTag()
{
}


void ListTag::load(byte* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = (ss.str());

	index += 3 + namelength;
	
	tagIDs = (int) bytes[index];
	
	size = (int)(bytes[index+1] * 16 * 16 * 16 * 16 * 16 * 16) + (int)bytes[index + 2] * 16 * 16 * 16 * 16 + (int)(bytes[index + 3] * 16 * 16) + (int)bytes[index + 4];
	tagsize = 1; // Byte size
	if (tagIDs == 2) tagsize = 2; // Short Size
	if (tagIDs == 3) tagsize = 4; // Int Size
	if (tagIDs == 4) tagsize = 8; // Long Size
	if (tagIDs == 5) tagsize = 4; // Float size
	if (tagIDs == 6) tagsize = 8; // Double size

	index += 5;
	if (tagIDs == 8) {
		tagsize = 1;
		for (int i = 0; i < size; i++) {
			int strS = ((int)bytes[index]*16*16 + (int)bytes[index + 1]);
			tags.push_back(bytes[index]);
			tags.push_back(bytes[index+1]);
			for (int j = 0; j < strS; j++) {
				tags.push_back(bytes[index + 2 + j]);
			}
			index += strS + 2;
		}
	} else for (int i = 0; i < size; i++) {
		for (int j = 0; j < tagsize; j++) {
			tags.push_back(bytes[j + i * (int)tagsize + index]);
		}
	}
}

int ListTag::next(byte* bytes, int index) {
	return index + 8 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2] + tags.size();
}

ListTag::Support ListTag::getValue() {
	Support s;
	stringstream ss;
	for (int i = 0; i < tags.size(); i+=tagsize) {
		if (tagIDs == 1) s.b.push_back(tags.at(i));
		if (tagIDs == 2) s.s.push_back((short) ((short) tags.at(i)*(short) (16*16) + (short) tags.at(i+1)));
		if (tagIDs == 3) s.i.push_back(*(int*)(new byte[4]{ tags.at(i + 3), tags.at(i + 2), tags.at(i + 1), tags.at(i) }));
		if (tagIDs == 4) s.l.push_back(*(long long*)(new byte[8]{ tags.at(i + 7), tags.at(i + 6), tags.at(i + 5), tags.at(i + 4), tags.at(i + 3), tags.at(i + 2), tags.at(i + 1), tags.at(i) }));
		if (tagIDs == 5) {
			vector<byte> tag{ tags.at(i+3), tags.at(i + 2), tags.at(i + 1), tags.at(i) };
			s.f.push_back(*(float*)tag.data());
		}
		//if (tagIDs == 6) s.d.push_back((double)(tags.at(i) * pow(16, 14) + tags.at(i + 1) * pow(16, 12) + tags.at(i + 2) * pow(16, 10) + tags.at(i + 3) * pow(16, 8) + tags.at(i + 4) * pow(16, 6) + tags.at(i + 5) * pow(16, 4) + tags.at(i + 6) * pow(16, 2) + tags.at(i + 7)));
		if (tagIDs == 6) {
			vector<byte> tag{ tags.at(i + 7), tags.at(i + 6), tags.at(i + 5), tags.at(i + 4), tags.at(i + 3), tags.at(i + 2), tags.at(i + 1), tags.at(i) };
			s.d.push_back(*(double*)tag.data());
		}

		if (tagIDs == 8) {
			stringstream ds;
			int strS = (int)tags.at(i) * 16 * 16 + (int)tags.at(i+1);
			for (int j = 0; j < strS; j++) {
				ds << tags.at(i + j + 2);
				ss << tags.at(i + j + 2);
			}
			s.c.push_back(ds.str());
			i += strS + 1;
		}

		if (tagIDs == 1) ss << (s.b.at(s.b.size() - 1));
		if (tagIDs == 2) ss << (s.s.at(s.s.size() - 1));
		if (tagIDs == 3) ss << (s.i.at(s.i.size() - 1));
		if (tagIDs == 5) ss << (s.f.at(s.f.size() - 1));
		if (tagIDs == 6) ss << (s.d.at(s.d.size() - 1));
		
		ss << ", ";
	}

	s.stringval = ss.str();

	return s;
}

vector<byte> ListTag::getBytesValue() {
	return tags;
}