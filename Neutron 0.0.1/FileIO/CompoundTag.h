#pragma once

#ifndef COMPOUNDTAG_H
#define COMPOUNDTAG_H

#include "Tag.h"
#include <vector>
#include <string>
class CompoundTag : virtual public Tag {
public:
	std::vector<Tag*> tags;
	Tag *testtag;
	CompoundTag();
	~CompoundTag();
	void load(byte* bytes, int index);
	std::string load(byte* bytes, int index, const std::string increment);
	int next(byte* bytes, int index);
	const std::vector<Tag*>& getValue();
	std::vector<byte> getBytesValue();
	std::vector<byte> outBytes();
	Tag *get(std::string name);
	const char* getName() {
		return name.c_str();
	}
	unsigned int id() {
		return 10;
	}
private:
	std::string name;
	int endIndex = 0;
};
#endif