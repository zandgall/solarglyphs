#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "Neutron.h"
using namespace neutron::processor;
class VectorImage 
{
public:
	unsigned int diffuse, distanceMap, nearestMask;
	
	VectorImage(unsigned int diffuse = 0, unsigned int distanceMap = 0, unsigned int nearestMask = 0) {
		this->diffuse = diffuse;
		this->distanceMap = distanceMap;
		this->nearestMask = nearestMask;
		int width = 0, height = 0;
		if (diffuse != 0) {
			width = sizeofTexture(diffuse).x;
			height= sizeofTexture(diffuse).y;
			if (distanceMap == 0) {
				std::vector<glm::vec4> col = colorsFromTexture(diffuse);
				std::vector<glm::vec4> newcol = std::vector<glm::vec4>();
				for (int i = 0; i < col.size(); i++) {
					newcol.push_back(glm::vec4(col.at(i).w, 0, 0, 1));
				}
				this->distanceMap = textureFromColors(newcol, width, height, GL_LINEAR);
			}
			width = sizeofTexture(distanceMap).x;
			height = sizeofTexture(distanceMap).y;
			if (nearestMask == 0) {
				this->nearestMask = textureFromColors(colorsFromTexture(this->distanceMap), width, height);
			}
			
		}
		std::cout << std::endl;
	}
	~VectorImage();
};

