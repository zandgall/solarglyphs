#include "SpaceObject.h"
#include "MouseDetector.h"
unsigned int planetglow = 0;
MouseDetector mcheck;
Quark gQ;
SpaceObject::SpaceObject(std::string id, vec3 position, long double size, long double mass, vec3 velocity, vec3 rotation, vec3 rotationalVelocity) {
	this->id = id;
	this->preview = neutron::processor::getTextureFrom(("res/textures/Planets/" + id + ".jpg").c_str(), GL_LINEAR, GL_CLAMP_TO_EDGE, false);
	this->position = position;
	this->size = size;
	this->mass = mass;
	this->velocity = velocity;
	this->rotation = rotation;
	this->rotationalVelocity = rotationalVelocity;
	glow = vec3(1);
	glowIntensity = 4;
	mcheck = MouseDetector(glm::vec4(this->position - glm::vec3(size*(easyViewMode ? 12.5 : 0.5)), 1), glm::vec4(this->position + glm::vec3(size *(easyViewMode ? 12.5 : 0.5)), 1));
	mcheck.GUI = false;
	mcheck.shape = MouseDetector::ELLIPSE;

	gQ = Quark();
	gQ.layer = -2;
	gQ.setB(Quark::FACE_CAMERA, true);
	gQ.setB(Quark::ALPHA_TEST, false);
	gQ.setUINT(Quark::DRAW, neutron::modes::DRAW_GLYPH);
	gQ.setUINT(Quark::DISTANCE_TEXTURE, neutron::processor::getTextureFrom("res/textures/circleDist.png"));
	gQ.setUINT(Quark::SHADER, neutron::processor::LoadShader("res/shaders/planetglow.shader"));
	gQ.setF(Quark::GLYPH_SOFT_EDGE, 0.5);
}

SpaceObject::SpaceObject(std::string id, vec3 position, long double size, long double mass, vec3 velocity, vec4 color, vec3 rotation, vec3 rotationalVelocity) : SpaceObject(id, position, size, mass, velocity, rotation, rotationalVelocity) {
	appearance.push_back(makeDefaultLook(color.xyz));
	glow = color.xyz;
	data = "Orbit Distance: " + std::to_string(length(position)) + "\nOrbit Velocity: "+std::to_string(length(velocity))+"\nMass: "+std::to_string(mass) + "\nRadius: "+ std::to_string(size)+"\n";
}
SpaceObject::~SpaceObject() {

}

void SpaceObject::render() {
	passes::transform = processor::form(position);
	glm::vec4 d = processor::useMatrices(glm::vec4(0, 0, 0, 1));
	if (d.w <= world::clip_near)
		return;
	passes::transform = processor::form(position, glm::vec3(size));
	d = processor::useMatrices(glm::vec4(-(easyViewMode ? 30 : 1), -(easyViewMode ? 30 : 1), -(easyViewMode ? 30 : 1), 1));
	glm::vec4 d2 = processor::useMatrices(glm::vec4((easyViewMode ? 30 : 1), (easyViewMode ? 30 : 1), (easyViewMode ? 30 : 1), 1));
	if (std::abs(d.x/d.w - d2.x/d2.w) < 1/frame::width || (d.x/d.w > 1 && d2.x/d2.w > 1) || (d.x/d.w < -1 && d2.x/d2.w < -1) || (d.y/d.w > 1 && d2.y/d2.w > 1) || (d.y/d.w < -1 && d2.y/d2.w < -1)) {
		//std::cout << id << ": Is Out of Bounds " << (d.x/d.w) << " - " << (d2.x / d2.w) << std::endl;
		return;
	}
	glm::vec3 front = glm::normalize((passes::globalTransform*vec4(position, 1) - vec4(world::camera.position - (world::camera.moveMode == world::Camera::MODE_ORBIT ? world::camera.Front*vec3(world::camera.OrbitDistance) : vec3(0)), 1)).xyz());
	glm::vec3 right = glm::normalize(glm::cross(front, world::camera.WorldUp));
	passes::cust_mat4s["camvec"] = glm::mat4{
		glm::vec4(-right, 1),
		glm::vec4(-glm::normalize(glm::cross(right, front)), 1),
		glm::vec4(-front, 1),
		glm::vec4(0,0,0,1)
	};
	glm::mat4 textrans = neutron::processor::form(rotation + vec3(std::atan2(front.x, front.z) / glm::two_pi<float>(), 0, 0));
	for (int i = 0; i < appearance.size(); i++) {
		Quark nQ = appearance[i];
		nQ.position += position;
		nQ.setM4(Quark::TEXTURE_TRANSFORM, nQ.getM4(Quark::TEXTURE_TRANSFORM) * textrans);
		nQ.size*=this->size*(easyViewMode ? 30 : 1);
		nQ.layer *= easyViewMode ? 30 : 1;
		nQ.render();
	}
}
std::vector<Quark> SpaceObject::getRender() {
	std::vector<Quark> out = std::vector<Quark>();
	for (int i = 0; i < appearance.size(); i++) {
		Quark nQ = appearance[i];
		nQ.position += position;
		nQ.setM4(Quark::TEXTURE_TRANSFORM, nQ.getM4(Quark::TEXTURE_TRANSFORM) * neutron::processor::form(rotation));
		nQ.size *= this->size*(easyViewMode ? 30 : 1);
		nQ.layer *= easyViewMode ? 30 : 1;
		out.push_back(nQ);
	}
	return out;
}
Quark *SpaceObject::getGlowRender() {

	neutron::passes::face_camera = true;
	neutron::modes::gui = false;
	neutron::passes::transform = neutron::processor::form(position, glm::vec3(size * (easyViewMode ? 15 : 0.5), size *(easyViewMode ? 15 : 0.5), 0));

	glm::vec4 p = neutron::processor::useMatrices(glm::vec4(1, 1, 1, 1));
	p.x = (p.x / p.w + 1) * neutron::frame::width / 2;
	p.y = (1 - p.y / p.w) * neutron::frame::height / 2;
	glm::vec4 s = neutron::processor::useMatrices(glm::vec4(-1, -1, -1, 1));
	s.x = (s.x / s.w + 1) * neutron::frame::width / 2;
	s.y = (1 - s.y / s.w) * neutron::frame::height / 2;

	if ((p.x > frame::width&&s.x > frame::width) || (p.x < 0 && s.x < 0) || (p.y > frame::height&&s.y > frame::height) || (p.y < 0 || s.y < 0) || p.w<=world::clip_near || std::abs(p.x-s.x)<0.01)
		return nullptr;

	gQ.setF4(Quark::COLOR, vec4(glow, (8-(max(p.x, s.x)-min(p.x, s.x)))/6));
	gQ.position = this->position;
	gQ.size = vec3(this->size*(easyViewMode ? 20 : 1)*glowIntensity);
	return &gQ;
}
long double SpaceObject::getRotationSpeedFromDaytime(long double hours) {
	long double seconds = (hours * 60 * 60);
	seconds *= glm::two_pi<long double>();
	return 1 / seconds;
}
bool SpaceObject::mouseHovered() {
	frame::selected_component = this;
	passes::face_camera = true;
	modes::gui = false;
	mcheck.start = glm::vec4(this->position - glm::vec3(size*(easyViewMode ? 15 : 0.5)), 1); 
	mcheck.end = glm::vec4(this->position + glm::vec3(size * (easyViewMode ? 15 : 0.5)), 1);
	mcheck.tick();
	return mcheck.hovered;
}