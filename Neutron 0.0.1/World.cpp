#include "World.h"
#include "FileIO/FileChooser.h"
#include "FileIO/CompoundTag.h"
#include "FileIO/IntArrayTag.h"
#include <vector>;
#include <glm/glm.hpp>

World::World() {

}

World::~World() {

}

void World::load(const char* filepath) {
	CompoundTag t = loadNBT(filepath);
	if (CompoundTag *graphics = dynamic_cast<CompoundTag*>(t.get("Graphics"))) {
		if (IntArrayTag *img = dynamic_cast<IntArrayTag*>(graphics->get("Image"))) {
			std::vector<glm::vec4> colors;
			for (int i = 0; i < img->getValue().size(); i++) {
				int r = (img->getValue().at(i) & 0x00'00'00'ff);
				int g = (img->getValue().at(i) & 0x00'00'ff'00) >> 8;
				int b = (img->getValue().at(i) & 0x00'ff'00'00) >> 16;
				int a = (img->getValue().at(i) & 0xff'00'00'00) >> 24;
				colors.push_back(glm::vec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0));
			}
		}
		
	}
}