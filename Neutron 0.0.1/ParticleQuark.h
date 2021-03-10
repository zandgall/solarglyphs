#pragma once
#ifndef PARTICLE_QUARK_H
#define PARTICLE_QUARK_H
#include "Quark.h"
using namespace glm;
class ParticleQuark : public Quark {
public:
	std::vector<glm::mat4> instances;
	ParticleQuark();
	~ParticleQuark();
	void addInstance(vec3 position = vec3(0), vec3 scale = vec3(1), vec3 rotation = vec3(0));
	void addInstance(mat4 matrix);
	void render();
};
#endif