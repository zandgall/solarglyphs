#pragma once
#include <Windows.h>
#include <vector>
class Tag {
public:
	virtual void load(byte* bytes, int index) = 0;
	virtual int next(byte* bytes, int index) = 0;
	virtual std::vector<byte> getBytesValue() = 0;
	virtual std::vector<byte> outBytes() = 0;
	virtual const char* getName() = 0;
	virtual unsigned int id() = 0;
};

