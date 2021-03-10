#include "Neutron.h"
#include "Main.h"

#include "Title.h"
#include "SerialImage.h"
#include "ButtonQuark.h"
#include "SliderQuark.h"
#include "SpaceObject.h"
#include "ValueAnimation.h"
#include "GraphQuark.h"
#include "ParagraphQuark.h"
#include "PageQuark.h"
#include "SpaceObjectSelector.h"
#include <iostream>
#include <algorithm>
using namespace std;
using namespace neutron::processor;

//class Envelope;

KeyFrame slide0 = KeyFrame(0, 0, KeyFrame::CALCCUBIC, KeyFrame::TYPEOUT), slide1 = KeyFrame(100, 1, 0, 0);
KeyFrame mode0 = KeyFrame(0, 0, KeyFrame::CALCCUBIC, KeyFrame::TYPEOUT), mode1 = KeyFrame(1, 1, KeyFrame::CALCCUBIC, KeyFrame::TYPEIN);
KeyFrame info0 = KeyFrame(0, 0, KeyFrame::CALCCUBIC, KeyFrame::TYPEOUT), info1 = KeyFrame(1, 1, KeyFrame::CALCCUBIC, KeyFrame::TYPEIN);
float slideTime = 0.0, modeTime = 1.0, modeAccel = -1.0, infoTime = 1.0;
ButtonQuark button, easyViewMode, infoButton;// , infoPanelBG;
SliderQuark slider;
ParagraphQuark infoText, infoName, infoData;
Quark infopreview;
PageQuark info;
std::vector<PageQuark::Splitter> infoSplits = std::vector<PageQuark::Splitter>();

unsigned int skyboxtex = 0;

int selected = -1;
bool findingSelected = false;

std::vector<SpaceObjectSelector> fasttravel = std::vector<SpaceObjectSelector>();

std::vector<SpaceObject> objects = std::vector<SpaceObject>();
Quark howfast, framerate, eframerate, mode, infoImage;
GraphQuark graph;
vec3 cameraVelocity = vec3(0);
vec2 mouseGlide = vec2(0);
void mforward() { if (selected == -1) cameraVelocity += glm::normalize(world::camera.Front*(vec3(1)-world::camera.WorldUp))*vec3(frame::delta_time * world::camera.MovementSpeed * (world::camera.Zoom / 360.0f)); }
void mbackward() { if(selected == -1) cameraVelocity -= glm::normalize(world::camera.Front*(vec3(1) - world::camera.WorldUp))*vec3(frame::delta_time * world::camera.MovementSpeed * (world::camera.Zoom / 360.0f)); }
void mleft() { if(selected == -1) cameraVelocity -= glm::normalize(world::camera.Right*(vec3(1) - world::camera.WorldUp))*vec3(frame::delta_time * world::camera.MovementSpeed * (world::camera.Zoom / 360.0f)); }
void mright() { if(selected == -1) cameraVelocity += glm::normalize(world::camera.Right*(vec3(1) - world::camera.WorldUp))*vec3(frame::delta_time * world::camera.MovementSpeed * (world::camera.Zoom / 360.0f)); }
void mup() { if(selected == -1) cameraVelocity += world::camera.WorldUp*vec3(frame::delta_time * world::camera.MovementSpeed * (world::camera.Zoom / 360.0f)); }
void mdown() { if(selected == -1) cameraVelocity -= world::camera.WorldUp*vec3(frame::delta_time * world::camera.MovementSpeed * (world::camera.Zoom / 360.0f)); }
void addEarth();
void addMoon();
void addSun();
void addMercury();
void addVenus();
void addMars();
void addJupiter();
void addSaturn();
void addUranus();
void addNeptune();
void addPluto();

void updateViewMode() {
	for (int i = 0; i < objects.size(); i++)
		objects[i].setViewMode(easyViewMode.on);
}

Title::Title() {
	keys::whenKeyPressedDo(GLFW_KEY_W, *mforward);
	keys::whenKeyPressedDo(GLFW_KEY_S, *mbackward);
	keys::whenKeyPressedDo(GLFW_KEY_D, *mright);
	keys::whenKeyPressedDo(GLFW_KEY_A, *mleft);
	keys::whenKeyPressedDo(GLFW_KEY_E, *mup);
	keys::whenKeyPressedDo(GLFW_KEY_Q, *mdown);
	keys::whenKeyPressedDo(GLFW_KEY_ESCAPE, *frame::stop);
	world::camera.LOCK_UP_VALUES = true;
	world::camera.OrbitDistance = 0.0001;
	world::clip_near = 0.025f;
	world::clip_far = 1e+36;
	world::camera.position = vec3(1496, 0, 0.13);
	passes::glyph_anti_alias_spread_def = 2.f;

	passes::def_cust_textures["plannorm"] = processor::getTextureFrom("res/textures/plannetnorm.png", GL_LINEAR, GL_CLAMP_TO_EDGE, false);

	button = ButtonQuark(vec3(800, 0, 0), vec3(100, 50, 0), vec3(0));
	//button.lookType = ButtonQuark::OUTLINE;
	button.interactionType = ButtonQuark::TOGGLE;
	button.setB(Quark::ALPHA_TEST, false);
	button.setF(Quark::GLYPH_CLIP, 0.2);
	button.setS(Quark::TEXT, "Simulate");

	infoButton = ButtonQuark(vec3(10, 700, 0), vec3(40, 40, 0), vec3(0));
	infoButton.lookType = ButtonQuark::OUTLINE;
	infoButton.interactionType = ButtonQuark::TOGGLE;
	infoButton.setShape(ButtonQuark::ELLIPSE);
	infoButton.setF(ButtonQuark::CORNERROUNDING, 20.0f);
	infoButton.setB(Quark::ALPHA_TEST, false);
	infoButton.setS(Quark::TEXT, "i");
	infoButton.setF(Quark::GLYPH_CLIP, 0.2);
	infoButton.setColorPallete(vec4(0.3, 0.3, 1, 1));
	infoButton.setTextPallete(vec4(0.3, 0.3, 1, 1));

	easyViewMode = ButtonQuark(vec3(800, 60, 0), vec3(100, 50, 0), vec3(0));
	easyViewMode.setS(Quark::TEXT, "Easy View");
	//easyViewMode.lookType = ButtonQuark::JARVOPIA;
	easyViewMode.interactionType = ButtonQuark::TOGGLE;
	easyViewMode.setB(Quark::ALPHA_TEST, false);
	easyViewMode.onClickedDo(&updateViewMode);

	slider = SliderQuark(vec3(400, 120, 0), glm::vec3(680, 50, 0), 0, 100, 1, SliderQuark::HORIZONTAL);
	slider.setB(Quark::ALPHA_TEST, false);

	howfast = Quark(vec3(260, 35, 0), 0, vec3(50, 25, 0));
	howfast.setB(Quark::GUI, true);
	howfast.setB(Quark::ALPHA_TEST, false);
	howfast.setUINT(Quark::DRAW, neutron::modes::DRAW_GLYPH);
	howfast.setS(Quark::FONT, "arial");
	howfast.setF(Quark::GLYPH_CLIP, 0.2);

	mode = Quark(vec3(740, 60, 0), 0.0, vec3(60, 60, 0));
	mode.setB(Quark::GUI, true);
	mode.setB(Quark::ALPHA_TEST, false);
	mode.setUINT(Quark::TEXTURE, neutron::processor::getTextureFrom("res/textures/free move.png"));

	framerate = Quark(vec3(80, 35, 0), 0, vec3(80, 25, 0));
	framerate.setB(Quark::GUI, true);
	framerate.setB(Quark::ALPHA_TEST, false);
	framerate.setUINT(Quark::DRAW, neutron::modes::DRAW_GLYPH);
	framerate.setS(Quark::FONT, "arial");
	framerate.setF(Quark::GLYPH_CLIP, 0.2);

	eframerate = Quark(vec3(80, 70, 0), 0, vec3(80, 25, 0));
	eframerate.setB(Quark::GUI, true);
	eframerate.setB(Quark::ALPHA_TEST, false);
	eframerate.setUINT(Quark::DRAW, neutron::modes::DRAW_GLYPH);
	eframerate.setS(Quark::FONT, "arial");
	eframerate.setF(Quark::GLYPH_CLIP, 0.2);
	
	graph = GraphQuark();
	graph.position = vec3(100, 400, 0);
	graph.size = vec3(100);

	graph.add("1", 0, vec3(1, 0, 0));
	graph.add("2", 0, vec3(0, 1, 0));
	graph.add("3", 0, vec3(0, 0, 1));

	//info = ParagraphQuark(vec3(-200, 90, 0), "This is the Info panel, click on a planet and open this up to get information about the planet.", 380, 24);
	//infoPanelBG = ButtonQuark(vec3(-200, 85, 0), vec3(400, 640, 0));
	//infoPanelBG.setS(Quark::TEXT, "");
	//infoPanelBG.setF(ButtonQuark::BEVEL, 0.01);
	//infoPanelBG.setB(Quark::ALPHA_TEST, 0);
	//infoPanelBG.setF(ButtonQuark::CORNERROUNDING, 20.f);

	info = PageQuark(vec3(-200, 85, 0), vec3(400, 640, 0), vec3(400, 640, 0));
	info.setF(PageQuark::BEVEL, 0.05f);
	info.setB(Quark::ALPHA_TEST, false);


	infoText = ParagraphQuark(vec3(10, 260, 0), "Click on a planet and open this up to get information about the planet.", 380, 24);
	infoName = ParagraphQuark(vec3(15, 200, 0), "Info Panel", 380, 48, 0);
	infoData = ParagraphQuark(vec3(15, 200, 0), "", 380, 18, 0);

	infoText.setF(Quark::GLYPH_CLIP, 0.2);
	infoData.setF(Quark::GLYPH_CLIP, 0.3);

	infoSplits.push_back(PageQuark::Splitter(250, 1, 380, 200));
	infoSplits.push_back(PageQuark::Splitter(250, 1, 380, 200));

	infopreview = Quark(vec3(200, 91, 0), 0, vec3(200, 91, 0));
	infopreview.setB(Quark::GUI, true);
	infopreview.setUINT(Quark::TEXTURE, processor::getTextureFrom("res/textures/Planets/fullprev.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE, false));

	info.put("preview", &infopreview);
	info.put("info", &infoText);
	info.put("infoName", &infoName);
	info.put("infoData", &infoData);
	info.put("split1", &infoSplits[0]);
	info.put("split2", &infoSplits[1]);
	addMoon();
	addSun();
	addMercury();
	addVenus();
	addMars();
	addJupiter();
	addSaturn();
	addUranus();
	addNeptune();
	addPluto();
	addEarth();

	vector<string> skyfaces{
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg"
	};
	skyboxtex = processor::loadCubemap(skyfaces, true);
}

void addEarth() {
	SpaceObject earth = SpaceObject("Earth", vec3(149.6e+6, 0, 0), 12756, 5.97e+24, vec3(0, 0, -29.8));
	earth.glow = glm::vec3(0.5, 0.53, 0.9);
	earth.accelerateRotation(vec3(earth.getRotationSpeedFromDaytime(23.9), 0, 0));
	earth.desc = "Earth is the third planet away from the sun. It is the only planet in our solar system that currently has water on it's surface, and so far, the only planet to hold life.\nEarth's surface is split up into 7 continents, and 4 oceans. The oceans and continents are life to various families of Mammals, Birds, Reptiles, Amphibians, Fish, and Invertebrates. The Earth uses an Atmostphere of 5 different layers each with their own composition and \"job\", to protect most life forms from types of radiation, and extreme temperatures.";
	earth.data = "Mass: 5.97 Septillion (10^24) Kilograms\nDiameter: 12,756 Kilometers\nDistance From Sun: 149.6 Million Kilometers\nDay time: 24 hours\nGravity: 9.8 Meters per Second\nOrbital Velocity: 29.9 Kilometers per Second\nMean Temperature: 15° C\nSurface Pressure: 1 Bar\nMoons: 1\n";
	Quark look = Quark();
	look.setB(Quark::FACE_CAMERA, true);
	unsigned int shader = neutron::processor::LoadShader("res/shaders/planet.shader");
	look.setUINT(6, modes::DRAW_GLYPH);
	look.setF(0, 0.2);
	look.setF(Quark::GLYPH_SOFT_EDGE, 0);
	look.setB(Quark::ALPHA_TEST, false);

	look.setUINT(5, neutron::passes::SHADER_FLAT);
	look.setUINT(Quark::DISTANCE_TEXTURE, passes::def_cust_textures["planet"]);
	look.setF4(Quark::COLOR, glm::vec4(0.125, 0.2, 0.5, 0.2));
	look.size = glm::vec3(1.4, 0.476*1.4, 1.4);
	look.layer = -0.3;
	earth.appearance.push_back(look);
	look.setF4(Quark::COLOR, glm::vec4(0.15, 0.25, 0.55, 0.4));
	look.size = glm::vec3(1.23, 0.476*1.23, 1.23);
	look.layer = -0.2;
	earth.appearance.push_back(look);
	look.setF4(Quark::COLOR, glm::vec4(0.2, 0.3, 0.6, 0.8));
	look.size = glm::vec3(1.1, 0.476*1.1, 1.1);
	look.layer = -0.1;
	earth.appearance.push_back(look);

	look.setF4(Quark::COLOR, glm::vec4(0.125, 0.2, 0.5, 1));
	look.size = glm::vec3(1.02, 0.476*1.02, 1.02);
	look.layer = -0.075;
	earth.appearance.push_back(look);

	//look.setF(Quark::GLYPH_SOFT_EDGE, 0.1);
	look.setUINT(5, shader);
	look.setUINT(Quark::DISTANCE_TEXTURE, processor::textureFromColor(vec4(1, 0, 0, 1)));
	look.setF4(Quark::COLOR, glm::vec4(0.2, 0.5, 1, 1));
	look.size = glm::vec3(1, 0.476, 1);
	look.layer = -0.05;
	earth.appearance.push_back(look);

	look.setUINT(3, neutron::processor::getTextureFrom("res/textures/Vector Testing/Earth Dist.png", GL_LINEAR, GL_REPEAT, false));
	look.setF4(0, glm::vec4(0.55, 0.85, 0.4, 1));
	look.size = glm::vec3(1, 0.476, 1);
	look.layer = 0;
	earth.appearance.push_back(look);

	look.setUINT(3, neutron::processor::getTextureFrom("res/textures/Vector Testing/Ball - copy.png", GL_LINEAR, GL_REPEAT, false));
	look.setF4(0, glm::vec4(1));
	look.size = glm::vec3(1, 0.476, 1);
	look.layer = 0.1;
	earth.appearance.push_back(look);

	objects.push_back(earth);
	
	SpaceObjectSelector eb = SpaceObjectSelector(50, vec3(235, 30, 0), vec3(0.4, 0.5, 0.8), objects.size()-1);
	fasttravel.push_back(eb);

	SpaceObject moon = SpaceObject("Moon", vec3(396756, 0, 0), 3475, 0.073e+24);
	//moon.glowIntensity = 1500;
	moon.desc = "The Moon is Earth's one and only natural satellite. The Moon has the same rotation speed as the Earth, facing the same side to Earth at all times. Back when the Moon was first studied by Astronomers, the dark patches shown were called \"Maria\"s as they mistook them for seas.\nThe Moon has been an attractive target for human exploration, as it's the closest object in space to earth. The first time anything reached the surface of the moon, was the Luna 2 from the Soviet Union. This was crashed intentionally, but followed up almost 10 years later, the Luna 9 has successfully landed on the surface. There have been manned missions to the Moon, starting with the Apollo missions. Apollo 8 was the first to send a crew in orbit of the moon, and Apollo 11 was the first to land with crew.";
	moon.data = "Mass: 73 Sextillion (10^21) Kilograms\nDiameter: 3,475 Kilometers\nDistance From Earth: 384 Thousand Kilometers\nDay time: 708.7 hours\nGravity: 1.6 Meters per Second\nOrbital Velocity: 1 Kilometer per Second\nMean Temperature: -20° C\nSurface Pressure: 0 Bars\n";
	moon.accelerate(vec3(0, 0, -moon.getIdealSpeedToOrbit(moon)));
	moon.orbitAround(earth);
	moon.accelerateRotation(vec3(moon.getRotationSpeedFromDaytime(655.7), 0, 0));
	look = SpaceObject::makeDefaultLook(vec3(1));
	look.layer = -0.1;
	moon.appearance.push_back(look);
	look.setUINT(Quark::DISTANCE_TEXTURE, processor::getTextureFrom("res/textures/Vector Testing/Moon Dist.png", GL_LINEAR, GL_REPEAT, false));
	look.setF4(Quark::COLOR, glm::vec4(0.6, 0.6, 0.7, 1));
	look.layer = 0;
	moon.appearance.push_back(look);
	objects.push_back(moon);

	eb = SpaceObjectSelector(20, vec3(255, 50, 0), vec3(0.5, 0.5, 0.5), objects.size()-1);
	fasttravel.push_back(eb);
}

void addMoon() {
	
}

void addMercury() {
	SpaceObject mercury = SpaceObject("Mercury", vec3(46.0e+6, 0, 0), 4879, 0.33e+24, vec3(0, 0, -47.4));
	mercury.glow = glm::vec3(0.5, 0.5, 0.5);

	mercury.desc = "The smallest and closest planet to the sun. It's years take about 88 Earth days, while it's days take about 175 Earth days. It's one of the 4 Solid planets closer to the center of our solar system.";
	mercury.data = "Mass: 330 Sextillion (10^21) Kilograms\nDiameter: 4,879 Kilometers\nDistance From Sun: 57.9 Million Kilometers\nDay time: 4222.6 hours\nGravity: 3.7 Meters per Second\nOrbital Velocity: 47.4 Kilometers per Second\nMean Temperature: 167° C\nSurface Pressure: 0 Bars\nMoons: 0\n";

	Quark look = SpaceObject::makeDefaultLook(vec3(0.65));
	mercury.appearance.push_back(look);
	objects.push_back(mercury);
	SpaceObjectSelector eb = SpaceObjectSelector(30, vec3(175, 30, 0), vec3(0.65), objects.size() - 1);
	fasttravel.push_back(eb);
}

void addVenus() {
	SpaceObject venus = SpaceObject("Venus", vec3(107.5e+6, 0, 0), 12104, 4.87e+24, vec3(0, 0, -35));
	venus.glow = glm::vec3(0.5, 0.3, 0);

	venus.desc = "The second planet from the sun, as well as the hottest with temperatures reaching 465 degrees Celcius (900 Fahrenheit). Venus is the closest planet both in size, mass, and distance to Earth. It's days however, are longer than it's years. With the days being 116 Earth days long, and the years being 243 Earth days long.";
	venus.data = "Mass: 4.87 Septillion (10^24) Kilograms\nDiameter: 12,104 Kilometers\nDistance From Sun: 108.2 Million Kilometers\nDay time: 2802 hours\nGravity: 8.9 Meters per Second\nOrbital Velocity: 35.0 Kilometers per Second\nMean Temperature: 465° C\nSurface Pressure: 92 Bars\nMoons: 0\n";

	venus.appearance.push_back(SpaceObject::makeDefaultLook(vec3(0.7, 0.5, 0.25)));
	objects.push_back(venus);
	SpaceObjectSelector eb = SpaceObjectSelector(35, vec3(202.5, 30, 0), vec3(0.7, 0.5, 0.25), objects.size() - 1);
	fasttravel.push_back(eb);
}

void addMars() {
	SpaceObject mars = SpaceObject("Mars", vec3(206.6e+6, 0, 0), 6792, 0.642e+24, vec3(0, 0, -24.1));
	mars.glow = glm::vec3(1, 0.4, 0);
	
	mars.desc = "The fourth and furthest terrestrial planet from the sun. About 1/9th Earths Mass, and half it's size, Mars is one of the closer easier targets for space eploration. With days around 24.7 hours, and temperatures reaching -65 degrees celcius, it's much more habitable than Venus. There have been many theories about water being on mars, but no signs of life have yet been found.";
	mars.data = "Mass: 642 Sextillion (10^21) Kilograms\nDiameter: 6,792 Kilometers\nDistance From Sun: 227.9 Million Kilometers\nDay time: 24.7 hours\nGravity: 3.7 Meters per Second\nOrbital Velocity: 24.1 Kilometers per Second\nMean Temperature: -65° C\nSurface Pressure: 0.01 Bar\nMoons: 2\n";

	mars.appearance.push_back(SpaceObject::makeDefaultLook(vec3(1, 0.4, 0.1)));
	objects.push_back(mars);

	SpaceObjectSelector eb = SpaceObjectSelector(40, vec3(280, 30, 0), vec3(1, 0.4, 0.1), objects.size() - 1);
	fasttravel.push_back(eb);

	SpaceObject phobos = SpaceObject("Phobos", vec3(9376, 0, 0), 2*11.1, 1.065e+16, vec3(0, 0, -2.13), vec4(1));
	phobos.desc = "Discovered in 1877 by Asaph Hall, this is the bigger of the two moons of mars discovered by Asaph Hill. Phobos has a crater about 9.7 Kilometers across.";
	phobos.orbitAround(mars);
	objects.push_back(phobos);
	eb = SpaceObjectSelector(10, vec3(295, 50, 0), vec3(0.5), objects.size() - 1);
	fasttravel.push_back(eb);
	SpaceObject deimos = SpaceObject("Deimos", vec3(23458, 0, 0), 2*6.2, 1.476e+15, vec3(0, 0, -1.35), vec4(1));
	deimos.desc = "Discovered in 1877 by Asaph Hall, this is the smaller of the two moons of mars discovered by Asaph Hill. Deimos is very similar to Asteroids in composition";
	deimos.orbitAround(mars);
	objects.push_back(deimos);
	eb = SpaceObjectSelector(10, vec3(305, 50, 0), vec3(0.5), objects.size() - 1);
	fasttravel.push_back(eb);
}

void addSun() {
	SpaceObject sun = SpaceObject("Sun", vec3(0, 0, 0), 1391000, 1.9891e+30);
	sun.glow = glm::vec3(1, 0.9, 0.4);
	Quark look = Quark();
	look.setB(Quark::FACE_CAMERA, true);
	look.layer = 0;
	unsigned int shader = neutron::processor::LoadShader("res/shaders/planet.shader");
	look.setUINT(6, modes::DRAW_GLYPH);
	look.setF(0, 0.2);
	look.setF(Quark::GLYPH_SOFT_EDGE, 0);
	look.setB(Quark::ALPHA_TEST, false);

	look.setUINT(5, neutron::passes::SHADER_FLAT);
	look.setUINT(Quark::DISTANCE_TEXTURE, processor::getTextureFrom("res/textures/circleDist.png"));

	look.setF4(Quark::COLOR, glm::vec4(1, 0.9, 0.4, 1));
	look.layer = -0.3;
	sun.appearance.push_back(look);

	sun.desc = "The Sun is the center star of our Solar System. It is a Yellow Dwarf, located 26 Thousand Light Years along the Orion Belt. The Sun gives energy to the 8 planets, 5+ dwarf planets, and the trillions of other bodies taken up by comets and asteroids. The top of this takes around 34 Earth days for one day, and the equater takes about 24.5 Earth days.";
	sun.data = "Mass: 1.98 Nonillion (10^30) Kilograms\nDiameter: 1,391,000 Kilometers\nDistance From Center of Milky Way: 26,000 Million Kilometers\nDay time: ~30 Earth days\nGravity: 274 Meters per Second\nOrbital Velocity: 828,000 Kilometers per Hour\nMean Temperature: 5504° C\nSurface Pressure: 1e-14 Bars\n";
	
	objects.push_back(sun);

	SpaceObjectSelector eb = SpaceObjectSelector(70, vec3(135, 30, 0), vec3(1, 0.9, 0.4), objects.size() - 1);
	fasttravel.push_back(eb);
}

void addJupiter() {
	SpaceObject jupiter = SpaceObject("Jupiter", vec3(740.6e+6, 0, 0), 142984, 1898e+24, vec3(0, 0, -13.1));
	jupiter.glow = glm::vec3(0.9, 0.6, 0.4);

	jupiter.desc = "5th planet from the Sun, and the largest of all planets combined, Jupiter is the first Gas Giant closest to the sun. Because of this, it takes 12 Earth years for a Jupiter (Jovian) year. If Jupiter has a solid core under all the gas, it is estimated to be just about the size of Earth. And despite not being nearly as visible as any of the other planets, Jupiter has a ring system.";
	jupiter.data = "Mass: 1.89 Octillion (10^27) Kilograms\nDiameter: 142,984 Kilometers\nDistance From Sun: 778.6 Million Kilometers\nDay time: 9.9 hours\nGravity: 23.1 Meters per Second\nOrbital Velocity: 13.1 Kilometers per Second\nMean Temperature: -110° C\nMoons: 79\n";

	jupiter.appearance.push_back(SpaceObject::makeDefaultLook(vec3(1, 0.7, 0.4)));
	objects.push_back(jupiter);

	SpaceObjectSelector eb = SpaceObjectSelector(60, vec3(340, 30, 0), vec3(1, 0.7, 0.4), objects.size() - 1);
	fasttravel.push_back(eb);

	SpaceObject callisto = SpaceObject("Callisto", vec3(1882700, 0, 0), 2*2410, 107e+21, vec3(0, 0, -8.2), vec4(1));
	callisto.desc = "Discovered by Galileo Galilie in 1670, it is the second largest moon around Jupiter, and the third largest moon in the whole Solar System. Thought to be covered with ice, with a possible salty ocean underneath. However, research suggests that it's either far deeper beneath the surface than previously thought, or there's no ocean whatsoever. If there was an ocean, it could potentially be a harbor of life.";
	callisto.orbitAround(jupiter);
	objects.push_back(callisto);
	eb = SpaceObjectSelector(25, vec3(372.5, 50, 0), vec3(0.8), objects.size() - 1);
	fasttravel.push_back(eb);
	SpaceObject europa = SpaceObject("Europa", vec3(671100, 0, 0), 2*1560.8, 47.998e+21, vec3(0, 0, -13.74), vec4(0.5, 0.5, 0.3, 1));
	europa.desc = "Europa is one of the more popular moons, for the fact that it almost certainly contains an ocean beneath it's surface. About a quarter of the size of Earth, it (most likely) contains more water than all the oceans on Earth combinded. With this ocean in mind, it has the most potential to harbor life, or to have harbored life than anything else in the solar system apart from Earth.";
	europa.orbitAround(jupiter);
	objects.push_back(europa);
	eb = SpaceObjectSelector(20, vec3(395, 50, 0), vec3(0.5, 0.5, 0.3), objects.size() - 1);
	fasttravel.push_back(eb);
	SpaceObject ganymede = SpaceObject("Ganymede", vec3(1070400, 0, 0), 2*2631.2, 148.185e+21, vec3(0, 0, -10.879), vec4(200/255.0, 150/255.0, 125/255.0, 1));
	ganymede.desc = "The largest moon in our solar system. About a quarter as massive, and half as large as Mars. The Moon has a large metallic core that creates it's own magnetic field (The only moon to do so). This field even gives Ganymede it's own Auroras. It is one of the 4 Galilean moons, along with Europa, Io, and Callisto.";
	ganymede.orbitAround(jupiter);
	objects.push_back(ganymede);
	eb = SpaceObjectSelector(30, vec3(420, 50, 0), vec3(0.7, 0.6, 0.5), objects.size() - 1);
	fasttravel.push_back(eb);
	SpaceObject io = SpaceObject("Io", vec3(421800, 0, 0), 2*1821.6, 89.319e+21, vec3(0, 0, -17.34), vec4(1, 1, 0.8, 1));
	io.desc = "The most volcanically active body in our Solar System, Io is one of the 4 Galilean Moons. It experiences tugs from both Europa and Ganymede (two other Galilean Moons around Jupiter) while trying to orbit Jupiter. It is about a quarter of the size of Earth.";
	io.orbitAround(jupiter);
	objects.push_back(io);
	eb = SpaceObjectSelector(20, vec3(445, 50, 0), vec3(1, 1, 0.8), objects.size() - 1);
	fasttravel.push_back(eb);

	for (int i = 0; i < 0*76; i++) {
		SpaceObject moon = SpaceObject("Anonymous Juvian Moon " + std::to_string(i), vec3((std::rand()%100/100.0)*3000000.0, 0, 0), (std::rand()%100/100.0)*2000.0, (std::rand()%100/100.0)*100e+21, vec3(0), vec4(1));
		moon.accelerate(vec3(0, 0, ((std::rand()%2)*2-1)*moon.getIdealSpeedToOrbit(jupiter)));
		moon.orbitAround(jupiter);
		objects.push_back(moon);
	}

}

void addSaturn() {
	SpaceObject saturn = SpaceObject("Saturn", vec3(1352.5e+6, 0, 0), 120536, 568e+24, vec3(0, 0, -9.7), vec4(0.6, 0.6, 0.4, 1));
	saturn.desc = "The second biggest planet, sixth from the sun. Saturn is known for it's easily visible, and large ring system, harboring 7 rings. Saturn is a Gas Giant with a large 29 Earth years for a Saturnian year. Out of all the planets in the Solar System, Satrun has the most moons. 53 of the Moons are named, but 29 Moons are still waiting to be confirmed, which gives Saturn a total of 82 Moons.";
	saturn.data = "Mass: 568 Septillion (10^24) Kilograms\nDiameter: 120,536 Kilometers\nDistance From Sun: 1.352 Billion Kilometers\nDay time: 10.7 hours\nGravity: 9.0 Meters per Second\nOrbital Velocity: 9.7 Kilometer per Second\nMean Temperature: -140° C\nMoons: 82\n";
	objects.push_back(saturn);

	SpaceObjectSelector eb = SpaceObjectSelector(55, vec3(482.5, 30, 0), vec3(0.6, 0.6, 0.4), objects.size() - 1);
	fasttravel.push_back(eb);

	SpaceObject dione = SpaceObject("Dione", vec3(377415, 0, 0), 2 * 561.7, 1.095e+21, vec3(0, 0, -10), vec4(1));
	dione.orbitAround(saturn);
	objects.push_back(dione);
	eb = SpaceObjectSelector(10, vec3(505, 50, 0), vec3(0.8), objects.size() - 1);
	fasttravel.push_back(eb);
	SpaceObject enceladus = SpaceObject("Enceladus", vec3(238037, 0, 0), 2*252.1, 107.944e+18, vec3(0, 0, -45487.3/60.0/60.0), vec4(1));
	enceladus.desc = "Enceladus is one of the few moons in the Solar System that may harbor life, or at least that has potential to. With an icy outer layer, and a confirmed ocean underneath, scientists have sampled the ocean from massive gushes of water and gas being spewed out. So much of this happens, that it gives Enceladus it's own ring.";
	enceladus.orbitAround(saturn);
	objects.push_back(enceladus);
	eb = SpaceObjectSelector(10, vec3(515, 50, 0), vec3(0.8), objects.size() - 1);
	fasttravel.push_back(eb);
	SpaceObject iapetus = SpaceObject("Iapetus", vec3(3560851, 0, 0), 2 * 735.6, 1.805e+21, vec3(0, 0, -11748.8 / 60.0 / 60.0), vec4(0.1, 0.07, 0.04, 1));
	iapetus.desc = "Iapetus, the third largest moon around Saturn, and one of the coolest looking. Iapetus has two sides with different brightnesses, one of them being a reddish black, and the other being a brighter white. They're put together almost like a stiched tennis ball.";
	iapetus.orbitAround(saturn);
	objects.push_back(iapetus);
	eb = SpaceObjectSelector(10, vec3(525, 50, 0), vec3(0.1, 0.07, 0.04), objects.size() - 1);
	fasttravel.push_back(eb);
	SpaceObject titan = SpaceObject("Titan", vec3(1221865, 0, 0), 2*2574.7, 134.552e+21, vec3(0, 0, -20051.2/60.0/60.0), vec4(0.8, 0.7, 0.5, 1));
	titan.desc = "One of the most notable moons around Saturn, and definitely one of the most notable moons in the entire Solar System. Titan is a tannish hazy planet on the outside, with hard ice on the surface. Titan's notability comes from it's subsurace ocean, but also it's breathable air. Titan's atmosphere is made up of mostly Nitrogen, just like Earth's atmosphere. Although, it's temperatures aren't safe to be in without protection, and you would still need an oxygen mask.";
	titan.orbitAround(saturn);
	eb = SpaceObjectSelector(10, vec3(535, 50, 0), vec3(0.8, 0.7, 0.5), objects.size() - 1);
	fasttravel.push_back(eb);
	objects.push_back(titan);

	for (int i = 0; i < 0*78; i++) {
		SpaceObject moon = SpaceObject("Anonymous Saturnian Moon " + std::to_string(i), vec3((std::rand() % 100 / 100.0)*3000000.0, 0, 0), (std::rand() % 100 / 100.0)*2000.0, (std::rand() % 100 / 100.0)*100e+21, vec3(0), vec4(1));
		moon.accelerate(vec3(0, 0, ((std::rand() % 2) * 2 - 1)*moon.getIdealSpeedToOrbit(saturn)));
		moon.orbitAround(saturn);
		objects.push_back(moon);
	}
}

void addUranus() {
	SpaceObject uranus = SpaceObject("Uranus", vec3(2741.3e+6, 0, 0), 51118, 86.8e+24, vec3(0, 0, -6.8), vec4(0.8, 0.8, 1, 1));
	uranus.desc = "Uranus is a big planet, the 7th from the sun. It's a gassy blue ball with a rocky core. It's got the second most visible ring system in the solar system, with 13 rings, and 27 (known) moons to go along with it. There's only been one flyby of Uranus, the Voyager 2. One notable difference between Uranus and a standard planet is the fact that it rotates along it's side, and like Venus, it rotates east to west.";
	uranus.data = "Mass: 86.8 Septillion (10^24) Kilograms\nDiameter: 51,118 Kilometers\nDistance From Sun: 2.872 Billion Kilometers\nDay time: 17.2 hours\nGravity: 8.7 Meters per Second\nOrbital Velocity: 6.8 Kilometer per Second\nMean Temperature: -195° C\nMoons: 27\n";
	objects.push_back(uranus);

	SpaceObjectSelector eb = SpaceObjectSelector(40, vec3(565, 30, 0), vec3(0.8, 0.8, 1), objects.size() - 1);
	fasttravel.push_back(eb);

	for (int i = 0; i < 0*27; i++) {
		SpaceObject moon = SpaceObject("Anonymous Uranian Moon " + std::to_string(i), vec3((std::rand() % 100 / 100.0)*3000000.0, 0, 0), (std::rand() % 100 / 100.0)*2000.0, (std::rand() % 100 / 100.0)*100e+21, vec3(0), vec4(1));
		moon.accelerate(vec3(0, 0, ((std::rand() % 2) * 2 - 1)*moon.getIdealSpeedToOrbit(uranus)));
		moon.orbitAround(uranus);
		objects.push_back(moon);
	}
}

void addNeptune() {
	SpaceObject neptune = SpaceObject("Neptune", vec3(4444.1e+6, 0, 0), 49528, 102e+24, vec3(0, 0, -5.4), vec4(0.1, 0.1, 0.7, 1));
	neptune.desc = "The windiest planet, with winds up to 5x the most powerful winds on Earth. It's the 8th planet from the sun. It's an ice giant, above a rocky core. Like Uranus, only the Voyager 2 has reached Neptune and no spacecraft has orbited it yet.";
	neptune.data = "Mass: 102.8 Septillion (10^24) Kilograms\nDiameter: 49,528 Kilometers\nDistance From Sun: 4.4951 Billion Kilometers\nDay time: 16.1 hours\nGravity: 11.0 Meters per Second\nOrbital Velocity: 5.4 Kilometer per Second\nMean Temperature: -200° C\nMoons: 14\n";
	objects.push_back(neptune);
	SpaceObjectSelector eb = SpaceObjectSelector(40, vec3(605, 30, 0), vec3(0.1, 0.1, 0.7), objects.size() - 1);
	fasttravel.push_back(eb);

	for (int i = 0; i < 0*14; i++) {
		SpaceObject moon = SpaceObject("Anonymous Neptunian Moon " + std::to_string(i), vec3((std::rand() % 100 / 100.0)*3000000.0, 0, 0), (std::rand() % 100 / 100.0)*2000.0, (std::rand() % 100 / 100.0)*100e+21, vec3(0), vec4(1));
		moon.accelerate(vec3(0, 0, ((std::rand() % 2) * 2 - 1)*moon.getIdealSpeedToOrbit(neptune)));
		moon.orbitAround(neptune);
		objects.push_back(moon);
	}
}

void addPluto() {
	SpaceObject pluto = SpaceObject("Pluto", vec3(7375.9e+6, 0, 0), 2370, 0.0146e+24, vec3(0, 0, -4.2), vec4(0.5));
	pluto.desc = "Used to be the ninth planet from the Sun, was classified as a dwarf planet in 2006. It's about as big as 1/2 the United States, and 2/3 as big as the moon. It's Charon is as big as 1/2 of Pluto, 603.6KM radius compared to Pluto's 1,151KM radius.";
	pluto.data = "Mass: 102.8 Septillion (10^24) Kilograms\nDiameter: 49,528 Kilometers\nDistance From Sun: 4.4951 Billion Kilometers\nDay time: 16.1 hours\nGravity: 11.0 Meters per Second\nOrbital Velocity: 5.4 Kilometer per Second\nMean Temperature: -200° C\nMoons: 14\n";
	objects.push_back(pluto);

	SpaceObjectSelector eb = SpaceObjectSelector(15, vec3(652.5, 30, 0), vec3(0.5), objects.size() - 1);
	fasttravel.push_back(eb);

	SpaceObject charon = SpaceObject("Charon", vec3(718.8, 0, 0), 2*603.6, 1.546e+21, vec3(0, 0, 718.8/60.0/60.0), vec4(1));
	charon.desc = "Charon, discovered in 1978, was discovered when James Christy saw a weird blobby shape to images of Pluto. Later, when the New Horizon's project was launched, proper iages were taken that proved it as the first official moon of Pluto.";
	charon.orbitAround(pluto);
	objects.push_back(charon); 
	eb = SpaceObjectSelector(10, vec3(660, 50, 0), vec3(0.8), objects.size() - 1);
	fasttravel.push_back(eb);

	for (int i = 0; i < 0*4; i++) {
		SpaceObject moon = SpaceObject("Anonymous Plutonian Moon " + std::to_string(i), vec3((std::rand() % 100 / 100.0)*3000000.0, 0, 0), (std::rand() % 100 / 100.0)*2000.0, (std::rand() % 100 / 100.0)*100e+21, vec3(0), vec4(1));
		moon.accelerate(vec3(0, 0, ((std::rand() % 2) * 2 - 1)*moon.getIdealSpeedToOrbit(pluto)));
		moon.orbitAround(pluto);
		objects.push_back(moon);
	}
}

void Title::tick() {
	if ((mouse::y < 120 || mouse::y > frame::height - 120) && !mouse::isLocked() || world::camera.moveMode == world::Camera::MODE_ORBIT || infoTime != 0) {
		slideTime += frame::delta_time;
		modeAccel = 1.0;
	} else {
		slideTime -= frame::delta_time;
	}
	if (modeTime > 1.5)
		modeAccel = -1.0;
	modeTime += frame::delta_time*modeAccel;
	slideTime = modeTime;
	modeTime = max(modeTime, 0);
	float modeT = mode0.get(modeTime, &mode1);
	mode.setF4(Quark::MULT_COLOR, glm::vec4(1, 1, 1, 0.15 + modeT * 0.85));
	mode.size = vec3(60*(0.75 + modeT / 4.0));
	mode.position = vec3(frame::width-45-15*modeT, 45+15*modeT, 0);

	slideTime = max(min(slideTime, slide1.getTime()+0.4), 0);
	button.setPosition(glm::vec3(60, frame::height - slide0.get(slideTime, &slide1)*0.75 + 40, 0));
	button.tick();
	easyViewMode.setPosition(glm::vec3(60, slide0.get(slideTime, &slide1)*0.75 - 40, 0));
	easyViewMode.tick();
	slider.setBodyBounds(glm::vec3(220, frame::height-slide0.get(slideTime, &slide1)*0.75+15, 0), glm::vec3(frame::width-10, frame::height - slide0.get(slideTime, &slide1)*0.75 + 65, 0));
	slider.tick();
	howfast.position = glm::vec3(165, frame::height - slide0.get(slideTime, &slide1)*0.75 + 40, 0);
	howfast.setS(Quark::TEXT, std::to_string(int(round(pow(slider.value, 2)*100.0)/100)));
	
	if(infoTime == 0)
		infoButton.setPosition(glm::vec3(-20 + slide0.get(slideTime, &slide1) / 2, frame::height-100, 0));
	else infoButton.setPosition(vec3(30 + info0.get(infoTime, &info1) * 410, frame::height - 100, 0));
	infoButton.tick();

	info.position.x = info0.get(infoTime, &info1) * 410 - 200;
	if (info.size.y != 0.5*(frame::height - 160)) {
		info.updateBounda(vec3(400, frame::height - 160, 0));
		std::cout << infoText.position.y + infoText.size.y * 2 << std::endl;
	}
	info.tick();
	//infoPanelBG.setBounds(vec3(-200, 85, 0), vec3(200, height - 82, 0));
	//infoPanelBG.position.x = info0.get(infoTime, &info1) * 410 - 200;


	if (infoButton.on)
		infoTime += frame::delta_time;
	else infoTime -= frame::delta_time;

	infoTime = max(min(infoTime, 1), 0);

	if (button.on) {
		long double v = 100 * pow(slider.value, 2) * frame::delta_time;
		for (int i = 0; i < objects.size(); i++) {
			objects[i].runAttraction(&objects[0], objects.size(), v);
			objects[i].tick(v);	
		}
	}

	if (selected != -1) {
		if(mouse::m)
			mouseGlide += vec2(mouse::x - mouse::pX, mouse::pY - mouse::y)/vec2(2);
		mouseGlide *= 0.75;
	} else {
		mouseGlide = vec2(mouse::x - mouse::pX, mouse::pY - mouse::y);
	}

	if (selected == -1) {
		cameraVelocity *= 0.75;
		world::camera.position += cameraVelocity;
	}

	world::camera.MovementSpeed = (keys::keys[341] ? keys::keys[342] ? 10.0 : keys::keys[340] ? 0.05 : 2.0 : keys::keys[340] ? 0.1 : 1.0) * (easyViewMode.on ? 100 : 1);
	if(frame::selected_component==nullptr)
		world::camera.ProcessMouseScroll(mouse::y_s_g*((selected!=-1) ? objects[selected].size/50000.0 * (easyViewMode.on ? 100 : 1) : 1));
	if (mouse::isLocked() || selected != -1)
		world::camera.ProcessMouseMovement(mouseGlide.x, mouseGlide.y);
	if (selected != -1) {
		cameraVelocity = vec3(0);
		if (mouse::m && !mouse::isLocked())
			mouse::lock();
		else if (!mouse::m && mouse::isLocked()) mouse::unlock();
		if (findingSelected) {
			world::camera.centerOn(objects[selected].position*vec3(0.00001) + world::camera.Left*vec3(info0.get(infoTime, &info1)*world::camera.OrbitDistance*0.5));
			world::camera.OrbitDistance = max(0.75+frame::delta_time, 0) * world::camera.OrbitDistance + (objects[selected].size*(easyViewMode.on ? 0.001: 0.00001))*(1-max(0.75 + frame::delta_time, 0));
			world::camera.Zoom = max(0.75 + frame::delta_time, 0) * world::camera.Zoom + 90*(1 - max(0.75 + frame::delta_time, 0));
			//std::cout << mouse::y_s << " : " << mouse::m << std::endl;
			if (glm::distance(world::camera.position, objects[selected].position*vec3(0.00001) + world::camera.Left*vec3(info0.get(infoTime, &info1)*world::camera.OrbitDistance*0.5)) < 0.01 || std::abs(mouse::y_s_g)>0.5 || mouse::m) {
				findingSelected = false;
				world::camera.OrbitDistance = objects[selected].size*(easyViewMode.on ? 0.001 : 0.00001);
				world::camera.Zoom = 90;
			}
		} else {
			world::camera.position = objects[selected].position*vec3(0.00001) + world::camera.Left*vec3(info0.get(infoTime, &info1)*world::camera.OrbitDistance*0.5);
		}
	}
	if (mouse::mClicked && selected == -1) {
		if(mouse::isLocked())
			mouse::unlock();
		else mouse::lock();
	}
	
	neutron::passes::globalTransform = neutron::processor::form(vec3(0), vec3(0.00001));

	for (int i = 0; i < fasttravel.size(); i++) {
		fasttravel[i].setPos(vec3(fasttravel[i].position.x, slide0.get(slideTime, &slide1)*0.75 - 40 + fasttravel[i].moon * 20, 0));
		fasttravel[i].tick();
		if (fasttravel[i].detect.hovered&&mouse::lClicked && !mouse::isLocked() && (frame::selected_component == nullptr))
			selected = -1;
	}

	//std::string savedid = selected == -1 ? "" : objects[selected].id;
	//std::sort(objects.begin(), objects.end(), SpaceObject::sortByDistance());
	//for (int i = 0; i < objects.size(); i++)
	//	if (selected != -1 && objects[i].id == savedid) {
	//		selected = i;
	//		break;
	//	}
	if (mouse::lClicked && !mouse::isLocked() && (frame::selected_component==nullptr)) {
		if (selected != -1 && !objects[selected].mouseHovered()) {
			selected = -1;
			world::camera.LOCK_UP_VALUES = true;
			world::camera.moveMode = world::Camera::MODE_FREE;
			world::camera.position = world::camera.position - world::camera.Front*world::camera.OrbitDistance;
			world::camera.OrbitDistance = 0.001;
			mode.setUINT(Quark::TEXTURE, neutron::processor::getTextureFrom("res/textures/free move.png"));
			modeAccel = 1.0;
			//infoText.setS(Quark::TEXT, "Click on another planet to learn about it!");
		}

		if (selected == -1)
			for (int i = objects.size()-1; i >= 0; i--) {
				if (objects[i].mouseHovered() || (i < fasttravel.size() && fasttravel[i].detect.hovered)) {
					selected = i;
					world::camera.LOCK_UP_VALUES = false;
					world::camera.moveMode = world::Camera::MODE_ORBIT;
					world::camera.OrbitDistanceMin = objects[i].size*0.000001*(easyViewMode.on*29+1);
					world::clip_near = 0;
					findingSelected = true;
					mode.setUINT(Quark::TEXTURE, neutron::processor::getTextureFrom("res/textures/orbit.png"));
					modeAccel = 1.0;
					
					info.scroll_y = 0;
					infopreview.size = glm::vec3(200, 200, 0);
					infopreview.position = glm::vec3(200, 200, 0);
					infopreview.setUINT(Quark::TEXTURE, objects[i].preview);
					infoText.position.y = 480;
					infoText.setS(Quark::TEXT, objects[i].desc);
					infoName.setS(Quark::TEXT, objects[i].id);
					infoData.setS(Quark::TEXT, objects[i].data);
					infoName.position.y = 420;
					infoSplits[0].position.y = 470;
					if (objects[i].descHeight == -1) {
						infoText.freezeToTexture();
						infoData.freezeToTexture();

						objects[i].descHeight = infoText.size.y;
						objects[i].dataHeight = infoData.size.y;

						infoText.setB(Quark::GUI, true);
						infoData.setB(Quark::GUI, true);
					}
					infoSplits[1].position.y = 490 + objects[i].descHeight;
					infoData.position.y = 500 + objects[i].descHeight;
					info.contentBounds.y = 500 + objects[i].descHeight + objects[i].dataHeight;
					std::cout << world::camera.OrbitDistanceMin << std::endl;
					return;
				}
			}
	}
}

void Title::render() {
	float pre = glfwGetTime();
	passes::cust_texures["plannorm"] = processor::getTextureFrom("res/textures/planetnorm.png");
	//glDisable(GL_DEPTH_TEST);
	modes::anti_alias = true;
	glDepthFunc(GL_LEQUAL);
	glUseProgram(passes::SHADER_SKYBOX);
	uniM4(passes::SHADER_SKYBOX, "view", world::camera.GetViewMatrix());
	uniM4(passes::SHADER_SKYBOX, "ortho", glm::perspective(glm::radians(45.f + world::camera.Zoom / 2.f), (float)frame::width / (float)frame::height, world::clip_near, world::clip_far));
	glBindVertexArray(passes::VAO_CUBE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxtex);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < objects.size(); i++) {
		Quark *p = objects[i].getGlowRender();
		if (p != nullptr)
			p->render();
		objects[i].render();
		//objects[i].trail.render();
	}

	graph.setValue("3", glfwGetTime() - pre);
}

void Title::render_gui() {

	float pre = glfwGetTime();

	neutron::passes::globalTransform = neutron::processor::form(vec3(0), vec3(0.00001));	

	neutron::passes::globalTransform = neutron::processor::form(vec3(0));
	button.render();
	howfast.render();
	slider.render();
	easyViewMode.render();
	mode.render();
	infoButton.render();
	//infoPanelBG.render();
	if (infoTime > 0)
		info.render();
	//infoText.render();
	for (int i = 0; i < fasttravel.size(); i++) {
		fasttravel[i].render();
	}

	if (frame::keys::keys[GLFW_KEY_F3]) {
		framerate.setS(Quark::TEXT, std::to_string(frame::fps));
		eframerate.setS(Quark::TEXT, std::to_string(frame::estimate_fps));
		framerate.render();
		eframerate.render();

		graph.render();
	}
	graph.setValue("2", glfwGetTime() - pre);
}