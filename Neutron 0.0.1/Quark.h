#pragma once
#ifndef QUARK_H
#define QUARK_H

#include "Neutron.h"
#include <vector>
#include <iostream>
#include <string>
using namespace neutron;
class Quark {
public:
	static enum {
		// Unsigned int
		TEXTURE = 0, MULT_TEXTURE = 1, ADD_TEXTURE = 2, DISTANCE_TEXTURE = 3, VAO = 4, SHADER = 5, DRAW = 6, 
		
		LAST_UINT = DRAW,
		// Float4
		COLOR = 0, MULT_COLOR = 1, ADD_COLOR = 2,

		LAST_FLOAT4 = ADD_COLOR,
		// Float
		GLYPH_CLIP = 0, GLYPH_SOFT_EDGE = 1,

		LAST_FLOAT = GLYPH_SOFT_EDGE,
		// Bool
		GUI = 0, ALPHA_TEST = 1, FACE_CAMERA = 2,

		LAST_BOOL = FACE_CAMERA,
		// Mat4
		TEXTURE_TRANSFORM = 0,

		LAST_MAT = TEXTURE_TRANSFORM,

		// String
		TEXT = 0, FONT = 1,
		LAST_STRING = 1,
	};
	glm::vec3 position = glm::vec3(0);
	glm::vec3 size = glm::vec3(1);
	glm::vec3 rotation = glm::vec3(0);
	float layer;

	Quark(glm::vec3 position = glm::vec3(0), float layer = 0.0f, glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0));
	~Quark();

	virtual void copyOptionsFromNeutron() {
		// Set Unsigned ints
		setUINT(MULT_TEXTURE, passes::mult_texture);
		setUINT(ADD_TEXTURE, passes::add_texture);
		setUINT(DISTANCE_TEXTURE, passes::distance_texture);
		setUINT(VAO, passes::vao);
		setUINT(SHADER, passes::shader);
		setUINT(DRAW, modes::draw);
		// Set Float4s
		setF4(MULT_COLOR, passes::mult_color);
		setF4(ADD_COLOR, passes::add_color);
		// Set Floats
		setF(GLYPH_CLIP, passes::glyph_clip);
		setF(GLYPH_SOFT_EDGE, passes::glyph_soft_edge);
		// Set Bools
		setB(GUI, modes::gui);
		setB(ALPHA_TEST, modes::alpha_test);
	}

	virtual void setVao(unsigned int value, unsigned int vertCount) {
		while (UINTS.size() <= VAO)
			UINTS.push_back(0);
		UINTS[VAO] = value;
		this->vertCount = vertCount;
	}

	virtual void loadOBJ(std::string filepath);

	virtual void writeQuarkFromOBJ(std::string objpath);

	virtual void loadQuark(std::string filepath);

	virtual void setUINT(unsigned int id, unsigned int value) {
		while (UINTS.size() <= id)
			UINTS.push_back(0);
		if (id == VAO) {
			std::cout << "SET VAO THROUGH SETVAO" << std::endl;
			throw EXCEPTION_INVALID_HANDLE;
		}

		UINTS[id] = value;
	}

	virtual void setF(unsigned int id, float value) {
		while (FLOATS.size() <= id)
			FLOATS.push_back(0);
		FLOATS[id] = value;
	}

	virtual void setF4(unsigned int id, glm::vec4 value) {
		while (FLOAT4S.size() <= id)
			FLOAT4S.push_back(glm::vec4(0));
		FLOAT4S[id] = value;
	}

	virtual void setB(unsigned int id, bool value) {
		while (BOOLS.size() <= id)
			BOOLS.push_back(false);
		BOOLS[id] = value;
	}

	virtual void setM4(unsigned int id, glm::mat4 value) {
		while (MATS.size() <= id)
			MATS.push_back(glm::mat4(0));
		MATS[id] = value;
	}

	virtual void setS(unsigned int id, const std::string &value) {
		while (STRINGS.size() <= id)
			STRINGS.push_back("");
		STRINGS[id] = value;
	}

	virtual float getF(unsigned int id) {
		if (id >= FLOATS.size())
			return 0;
		return FLOATS[id];
	}

	virtual glm::vec4 getF4(unsigned int id) {
		if (id >= FLOAT4S.size())
			return glm::vec4(0);
		return FLOAT4S[id];
	}

	virtual bool getB(unsigned int id) {
		if (id >= BOOLS.size())
			return 0;
		return BOOLS[id];
	}

	virtual unsigned int getUINT(unsigned int id) {
		if (id >= UINTS.size())
			return 0;
		return UINTS[id];
	}

	virtual glm::mat4 getM4(unsigned int id) {
		if (id >= MATS.size())
			return glm::mat4(0);
		return MATS[id];
	}

	virtual std::string getS(unsigned int id) {
		if (id >= STRINGS.size())
			return "";
		return STRINGS[id];
	}

	virtual unsigned int getVertCount() {
		return vertCount;
	}

	virtual void tick();

	virtual void set();

	virtual void render();

	static struct sort {
		bool operator()(Quark a, Quark b) const {
			glm::vec3 camPos = world::camera.position;
			if (world::camera.moveMode == world::Camera::MODE_ORBIT)
				camPos = world::camera.position - world::camera.Front*world::camera.OrbitDistance;
			glm::vec4 ap = passes::globalTransform * glm::vec4(a.position, 1);
			glm::vec4 bp = passes::globalTransform * glm::vec4(b.position, 1);
			return glm::distance(glm::vec3(ap.x, ap.y, ap.z), camPos) - a.layer / 100.0f > glm::distance(glm::vec3(bp.x, bp.y, bp.z), camPos) - b.layer / 100.0f;
		}
	};

protected:
	std::vector<unsigned int>	UINTS;
	std::vector<float>			FLOATS;
	std::vector<bool>			BOOLS;
	std::vector<glm::vec4>		FLOAT4S;
	std::vector<glm::mat4>		MATS;
	std::vector<std::string>	STRINGS;

	unsigned int vertCount = 6;
};
#endif