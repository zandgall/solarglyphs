#include "SerialImage.h"
#include "stb_image.h"
#include <string>
#include "Neutron.h"
using namespace neutron::processor;

SerialImage::SerialImage(const char* filepath, bool vertflip)
{
	stbi_set_flip_vertically_on_load(vertflip);
	int loadtype = STBI_rgb;
	if (((std::string) filepath).find(".png") != std::string::npos)
		loadtype = STBI_rgb_alpha;
	unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, loadtype);
	for(int i = 0; i<width; i++)
		for (int j = 0; j < height; j++) {
			unsigned char* pixel = data + (j*width + i) * nrChannels;
			int red = *(int*)(new byte[4]{ pixel[0], 0, 0, 0 });
			int green = *(int*)(new byte[4]{ pixel[1], 0, 0, 0 });
			int blue = *(int*)(new byte[4]{ pixel[2], 0, 0, 0 });
			int alpha = *(int*)(new byte[4]{ pixel[3], 0, 0, 0 });
			colors.push_back(glm::vec4(red/255.0, green/255.0, blue/255.0, alpha/255.0));
		}
	texture = textureFromColors(colors, width, height);

	stbi_image_free(data);
}

SerialImage::~SerialImage()
{
}

SerialImage SerialImage::getCropped(int x, int y, int width, int height) {
	std::vector<glm::vec4> newColors = std::vector<glm::vec4>();

	for (int j = y; j < y + height; j++) {
		for (int i = x; i < x + width; i++) {
			newColors.push_back(colors.at(j*this->width + i));
		}
	}

	return SerialImage(newColors, width, height, nrChannels);
}

void SerialImage::crop(int x, int y, int width, int height) {
	std::vector<glm::vec4> newColors = std::vector<glm::vec4>();

	for (int j = y; j < y + height; j++) {
		for (int i = x; i < x + width; i++) {
			newColors.push_back(colors.at(j*this->width + i));
		}
	}
	this->width = width;
	this->height = height;
	this->colors = newColors;
	texture = textureFromColors(newColors, width, height);
}