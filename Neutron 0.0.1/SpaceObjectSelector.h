#pragma once
#ifndef SPACEOBJECTSELECTOR
#define SPACEOBJECTSELECTOR
#include "Neutron.h"
#include "MouseDetector.h"
class SpaceObjectSelector {
public:
	float size;
	glm::vec3 position, color;
	MouseDetector detect;
	unsigned short id;
	bool moon;
	SpaceObjectSelector() {
		position = glm::vec3(0);
		size = 0;
		id = 0;
		moon = 0;
	}
	SpaceObjectSelector(float size, glm::vec3 position, glm::vec3 color, unsigned short id) {
		this->size = size*=0.9;
		this->position = position;
		this->color = color;
		detect = MouseDetector(position-glm::vec3(size/2.4), position+glm::vec3(size/2.4));
		detect.shape = MouseDetector::ELLIPSE;
		this->id = id;
		moon = (position.y == 50);
	}

	void setPos(glm::vec3 position) {
		this->position = position;
		detect = MouseDetector(position - glm::vec3(size / 2.4), position + glm::vec3(size / 2.4));
		detect.shape = MouseDetector::ELLIPSE;
	}

	void setSize(float size) {
		this->size = size;
		detect = MouseDetector(position - glm::vec3(size / 2.4), position + glm::vec3(size / 2.4));
		detect.shape = MouseDetector::ELLIPSE;
	}

	void tick() {
		detect.tick();
	}

	void render() {
		neutron::reset();
		neutron::passes::cust_texures["plannorm"] = neutron::processor::getTextureFrom("res/textures/planetnorm.png");
		//neutron::passes::shader = neutron::processor::LoadShader("res/shaders/planet.shader");
		neutron::modes::gui = true;
		neutron::modes::draw = neutron::modes::DRAW_GLYPH;
		neutron::passes::transform = neutron::processor::rect(position.x-size*1.05, position.y-size/2, size*2.1, size);
		neutron::passes::distance_texture = neutron::processor::getTextureFrom("res/textures/Vector Testing/Ball.png");
		neutron::passes::glyph_clip = 0.2;
		neutron::passes::glyph_soft_edge = 0.2;
		neutron::passes::cust_mat4s["camvec"] = glm::mat4{
			glm::vec4(-1,  0,  0,  1),
			glm::vec4( 0, -1,  0,  1),
			glm::vec4( 0,  0, -1,  1),
			glm::vec4( 0,  0,  0,  1)
		};
		neutron::draw(0, glm::vec4(color*(detect.hovered ? vec3(1.5) : vec3(1)), 1));
	}
};

#endif