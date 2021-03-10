#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Neutron.h"
class SerialImage
{
public:
	int width, height, nrChannels;
	std::vector<glm::vec4> colors;
	SerialImage(const char* filepath, bool vertflip = true);
	unsigned int texture;

	SerialImage() {
		colors = std::vector<glm::vec4>();
		width = 0;
		height = 0;
		nrChannels = 4;
		texture = 0;
	}
	SerialImage(std::vector<glm::vec4> colors, int width, int height, int nrChannels = 4) {
		this->colors = colors;
		this->width = width;
		this->height = height;
		this->nrChannels = nrChannels;
		texture = neutron::processor::textureFromColors(colors, width, height);
	}
	~SerialImage();
	SerialImage getCropped(int x, int y, int width, int height);
	void crop(int x, int y, int width, int height);
};

