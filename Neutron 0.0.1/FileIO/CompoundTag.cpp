#include "CompoundTag.h"
#include "ShortTag.h"
#include "ByteArrayTag.h"
#include "IntTag.h"
#include "ByteTag.h"
#include "LongTag.h"
#include "FloatTag.h"
#include "DoubleTag.h"
#include "StringTag.h"
#include "ListTag.h"
#include "IntArrayTag.h"
#include "LongArrayTag.h"

#include "FileChooser.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <Windows.h>

using namespace std;
CompoundTag::CompoundTag(){
	tags = std::vector<Tag*>(); 
}


CompoundTag::~CompoundTag(){
	tags.clear();
	tags.~vector();
}

void CompoundTag::load(byte* bytes, int index) {
	cout << load(bytes, index, "") << endl;
}

string CompoundTag::load(byte* bytes, int index, const string increment) {
	cout << "Loading compound";
	int namelength = (int) (bytes[index+1] * 16 * 16) + (int) bytes[index+2];
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int) bytes[i + index + 3]);
	}
	name = ss.str();
	//ss.~basic_stringstream();

	cout << ": " << name << " (" << ss.str() << ")" << endl;
	index += 3 + namelength;

	string compilation = increment + "CompoundTag(" + string(name) + "): {\n";

	//*
	for (index; true;) {
		int TId = bytes[index];
		cout << "ID: " << TId << endl;
		switch (TId) {
			case 0: {// End
				endIndex = index;
				compilation += increment + "};";
				cout << "Ending... Index:" << endIndex << endl;
				//cout << compilation << endl;
				return compilation + "\n";
			}
			case 1: {// Byte
				cout << "Adding Byte..." << endl;
				ByteTag *tag = new ByteTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				ss << hex << (int) tag->getValue();
				compilation += increment + "\tByteTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 2: {// Short
				cout << "Adding Short..." << endl;
				ShortTag *tag = new ShortTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				ss << tag->getValue();
				compilation += increment + "\tShortTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() +"\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 3: {// Int
				cout << "Adding Int..." << endl;
				IntTag *tag = new IntTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				ss << tag->getValue();
				compilation += increment + "\tIntTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 4: {// Long
				cout << "Adding Long..." << endl;
				LongTag *tag = new LongTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				ss << tag->getValue();
				compilation += increment + "\tLongTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 5: {// Float
				cout << "Adding Float..." << endl;
				FloatTag *tag = new FloatTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				ss << tag->getValue();
				compilation += increment + "\tFloatTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 6: {// Double
				cout << "Adding Double..." << endl;
				DoubleTag *tag = new DoubleTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				ss << tag->getValue();
				compilation += increment + "\tDoubleTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 7: {// Byte Array
				cout << "Adding Byte Array..." << endl;
				ByteArrayTag *tag = new ByteArrayTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				for (int i = 0; i < tag->getValue().size(); i++) {
					ss << hex << (int) tag->getValue().at(i);
					if (i != tag->getValue().size() - 1)
						ss << ", ";
				}
				compilation += increment + "\tByteArrayTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment +"\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 8: {// String
				cout << "Adding String String..." << endl;
				StringTag *tag = new StringTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				compilation += increment + "\tStringTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + tag->getValue() +"\n" + increment + "\t};\n";
				break;
			}
			case 9: { // List
				cout << "Adding List..." << endl;
				ListTag *tag = new ListTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				compilation += increment + "\tListTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + tag->getValue().stringval + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 10: { // Compoundx
				cout << "Adding Compound..." << endl;
				CompoundTag *tag = new CompoundTag();
				compilation += tag->load(bytes, index, increment + "\t");
				index = tag->next(bytes, index);
				tags.push_back(tag);
				break;
			}
			case 11: {// Int array
				cout << "Adding Int Array..." << endl;
				IntArrayTag *tag = new IntArrayTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				for (int i = 0; i < tag->getValue().size(); i++) {
					ss << dec << (int)tag->getValue().at(i);
					if (i != tag->getValue().size() - 1)
						ss << ", ";
				}
				compilation += increment + "\tIntArrayTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			case 12: {// Long array
				cout << "Adding Long Array..." << endl;
				LongArrayTag *tag = new LongArrayTag();
				tag->load(bytes, index);
				index = tag->next(bytes, index);
				tags.push_back(tag);
				stringstream ss;
				for (int i = 0; i < tag->getValue().size(); i++) {
					ss << dec << (long long)tag->getValue().at(i);
					if (i != tag->getValue().size() - 1)
						ss << ", ";
				}
				compilation += increment + "\tLongArrayTag(" + string(tag->getName()) + ") {\n" + increment + "\t\t" + ss.str() + "\n" + increment + "\t};\n";
				//ss.~basic_stringstream();
				break;
			}
			default:
				index++;
		}
	}//*/
	return compilation;
}

int CompoundTag::next(byte* bytes, int index) {
	return endIndex+1;
}

const vector<Tag*>& CompoundTag::getValue() {
	return tags;
}

Tag *CompoundTag::get(std::string name) {
	for (int i = 0; i < tags.size(); i++) {
		Tag *tag = tags.at(i);
		if (tag->getName() == name)
			return tag;
	}
	return nullptr;
}

std::vector<byte> CompoundTag::getBytesValue() {
	return vector<byte>{ 00 };
}
vector<byte> CompoundTag::outBytes() {
	vector<byte> out = vector<byte>();

	out.push_back(0x0A);
	out.push_back((byte)(name.size() & 0x00'00'ff'00) >> 8);
	out.push_back((byte)(name.size() & 0x00'00'00'ff));

	for (int i = 0; i < name.size(); i++) {
		out.push_back((int)name.at(i));
	}
	
	for (int i = 0; i < tags.size(); i++) {
		Tag *tag = tags.at(i); 
		cout << "tag: " << i << " " << tag->getName() << endl;
		for (int j = 0; j < tag->outBytes().size(); j++)
			out.push_back(tag->outBytes().at(j));
	}
	
	out.push_back(0x00);
	
	return out;
}