#pragma once
#ifndef MOUSEDET_H
#define MOUSEDET_H
#include <glm/glm.hpp>
class MouseDetector {
public:
	static enum {
		RECTANGLE = 0, ELLIPSE = 1,
	};
	// Between MouseDetector::RECTANGLE or MouseDetector::ELLIPSE
	unsigned int shape = MouseDetector::RECTANGLE;
	// The position of the top-left corner
	glm::vec3 start;
	// The position of the bottom-right corner
	glm::vec3 end;
	// How round the rectangle would be, if current shape is MouseDetector::RECTANGLE
	float rounding = 0;
	// Boolean that reads mouse input
	bool hovered = false, lClicked = false, lPressed = false, lReleased = false, rClicked = false, rPressed = false, rReleased = false, draggable = false, GUI = true, locked = false;
	MouseDetector(glm::vec3 start = glm::vec3(0), glm::vec3 end = glm::vec3(0));
	~MouseDetector();
	void tick();
	void tick(float mouseX, float mouseY);

	//Only affects anything if it's draggable = true;
	void constrain(glm::vec3 minimum, glm::vec3 maximum);
private:
	glm::vec3 min, max;
};

#endif