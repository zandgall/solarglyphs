#pragma once
#include "Neutron.h"
#include "Quark.h"
#include "ParticleQuark.h"

#ifndef G_CONSTANT
#define G_CONSTANT 6.6743015e-11
#endif

#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H
using namespace glm;
class SpaceObject
{
public:

	/// GRAVITY = (G_CONSTANT*a.mass*b.mass)/pow(distance(a.position, b.position), 2);

	vec3 position, velocity, rotation, rotationalVelocity;

	long double size, mass;
	std::vector<Quark> appearance;
	vec3 glow;
	float glowIntensity;
	ParticleQuark trail;
	bool easyViewMode = false;

	std::string id = "Unnamed Space Object", desc = "No description given", data = "No data given";
	unsigned int preview = 0;
	float descHeight = -1, dataHeight = -1;
	SpaceObject() {
		position = vec3(0);
		size = 1;
		mass = 1;
		velocity = vec3(0);
		rotation = vec3(0);
		rotationalVelocity = vec3(0);
		appearance = std::vector<Quark>();
		trail = ParticleQuark();
		//easyViewMode = false;
	};
	SpaceObject(std::string id, vec3 position, long double size, long double mass, vec3 velocity = vec3(0), vec3 rotation = vec3(0), vec3 rotationalVelocity = vec3(0));
	SpaceObject(std::string id, vec3 position, long double size, long double mass, vec3 velocity, vec4 color, vec3 rotation = vec3(0), vec3 rotationalVelocity = vec3(0));
	~SpaceObject();
	void orbitAround(SpaceObject o) {
		this->position += o.position;
		this->velocity += o.velocity;
	}
	void setViewMode(bool easy) {
		this->easyViewMode = easy;
	}
	void tick(long double timePassed = 0.0166) {
		position += velocity * vec3(timePassed);
		rotation += rotationalVelocity * vec3(timePassed);
		index++;
		if (index > 20) {
			index = 0;
			trail.addInstance(position, vec3(size*0.15));
		}
	}
	void accelerate(vec3 acceleration) {
		velocity += acceleration;
	}
	void accelerateRotation(vec3 acceleration) {
		rotationalVelocity += acceleration;
	}
	void attractTo(SpaceObject a, long double timePassed = 0.0166) {
		vec3 i = vec3(1000)*a.position;
		vec3 j = vec3(1000)*this->position;
		long double dist = pow(i.x - j.x, 2) + pow(i.y - j.y, 2) + pow(i.z - j.z, 2);
		if (dist == 0 || a.mass / dist < 1e-10)
			return;
		long double fac = (G_CONSTANT*a.mass) / (dist * 1000 * sqrt(dist));
		vec3 dir = (i - j) * vec3(timePassed*fac);
		accelerate(dir);
	}
	void runAttraction(SpaceObject *a, int size, long double timePassed = 0.0166) {
		for (int i = 0; i < size; i++)
			if (a[i].id != this->id)
				attractTo(a[i], timePassed);
	}
	void render();
	std::vector<Quark> getRender();
	Quark *getGlowRender();
	long double getIdealSpeedToOrbit(vec3 position, long double mass) {
		long double r = length(position-this->position);
		vec3 d = glm::pow((vec3(1000)*position - vec3(1000)*this->position), vec3(2.f));
		long double dist = d.x + d.y + d.z;
		if (dist == 0)
			return 0;
		long double fac = ((G_CONSTANT*mass*this->mass) / dist) / 1000;
		return sqrt((fac*r) / this->mass);
	}
	long double getIdealSpeedToOrbit(SpaceObject o) {
		return getIdealSpeedToOrbit(o.position, o.mass);
	}
	long double getRotationSpeedFromDaytime(long double hours);
	bool mouseHovered();

	static Quark makeDefaultLook(vec3 color) {
		Quark look = Quark();
		look.setB(Quark::FACE_CAMERA, true);
		look.setUINT(Quark::DRAW, modes::DRAW_GLYPH);
		look.setF(Quark::GLYPH_CLIP, 0.2);
		look.setF(Quark::GLYPH_SOFT_EDGE, 0.2);
		look.setUINT(Quark::SHADER, neutron::processor::LoadShader("res/shaders/planet.shader"));
		look.setUINT(Quark::DISTANCE_TEXTURE, processor::textureFromColor(vec4(1,0,0,1)));
		look.setF4(Quark::COLOR, glm::vec4(color, 1));
		look.size = glm::vec3(1, 0.476, 1);
		look.layer = 0;
		return look;
	}

	static struct sortByDistance {
		bool operator()(SpaceObject a, SpaceObject b) const {
			glm::vec3 camPos = world::camera.position;
			if (world::camera.moveMode == world::Camera::MODE_ORBIT)
				camPos = world::camera.position - world::camera.Front*world::camera.OrbitDistance;
			return distance(a.position*vec3(0.00001), camPos) > distance(b.position*vec3(0.00001), camPos);
		}
	};

private:
	int index = 0;
};
#endif