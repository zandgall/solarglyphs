#include "Neutron.h"
#include "MouseDetector.h"
#include "main.h"
using namespace neutron::frame;

MouseDetector::MouseDetector(glm::vec3 start, glm::vec3 end) {
	this->start = start;
	this->end = end;
	this->min = glm::vec3(0, 0, 0);
	this->max = glm::vec3(0, 0, 0);
}


MouseDetector::~MouseDetector() {

}

void MouseDetector::tick() {
	tick(mouse::x, mouse::y);
}

void MouseDetector::tick(float mouseX, float mouseY) {
	if (locked)
		return;
	glm::vec3 start = this->start;
	glm::vec3 end = this->end;
	if (!GUI) {
		neutron::passes::face_camera = true;
		neutron::modes::gui = false;

		glm::vec3 size = glm::abs(end - start);
		neutron::passes::transform = neutron::processor::form((start+end)/glm::vec3(2), glm::vec3(size.x/2, size.y/2, 0));

		glm::vec4 p = neutron::processor::useMatrices(glm::vec4(1, 1, 0, 1));
		p.x = (p.x / p.w + 1) * neutron::frame::width / 2;
		p.y = (1 - p.y / p.w) * neutron::frame::height / 2;
		glm::vec4 s = neutron::processor::useMatrices(glm::vec4(-1, -1, 0, 1));
		s.x = (s.x / s.w + 1) * neutron::frame::width / 2;
		s.y = (1 - s.y / s.w) * neutron::frame::height / 2;
		if (p.z < 0 || s.z < 0) {
			return;
		}

		//std::cout << glm::to_string(start) << " " << glm::to_string(end) << " (" << mouse::x << ", " << mouse::y << ")" << std::endl;
		end = glm::vec3(max(p.x, s.x), max(p.y, s.y), s.z);
		start = glm::vec3(min(p.x, s.x), min(p.y, s.y), p.z);
	}

	float w = (end.x - start.x);
	float h = (end.y - start.y);
	if (draggable&&lPressed&&GUI) {
		this->start.x = mouseX - w / 2;
		this->start.y = mouseY - h / 2;
		this->end.x = mouseX + w / 2;
		this->end.y = mouseY + h / 2;
	}
	if (mouseX >= start.x&&mouseY >= start.y&&mouseX < end.x && mouseY < end.y) {
		if (shape == RECTANGLE) {
			if (mouseX < start.x + rounding) {
				if (mouseY < start.y + rounding)
					hovered = (std::pow(2 * ((mouseX - start.x - (rounding * 2)) / (rounding * 2) + 0.5), 2.f) + std::pow(2 * ((mouseY - start.y - (rounding * 2)) / (rounding * 2) + 0.5), 2)) <= 1;
				else if (mouseY > end.y - rounding)
					hovered = (std::pow(2 * ((mouseX - start.x - (rounding * 2)) / (rounding * 2) + 0.5), 2.f) + std::pow(2 * ((mouseY - end.y + (rounding * 2)) / (rounding * 2) - 0.5), 2)) <= 1;
				else
					hovered = true;
			}
			else if (mouseX > end.x - rounding) {
				if (mouseY < start.y + rounding)
					hovered = (std::pow(2 * ((mouseX - end.x + rounding * 2) / (rounding * 2) - 0.5), 2.f) + std::pow(2 * ((mouseY - start.y - rounding * 2) / (rounding * 2) + 0.5), 2)) <= 1;
				else if (mouseY > end.y - rounding)
					hovered = (std::pow(2 * ((mouseX - end.x + rounding * 2) / (rounding * 2) - 0.5), 2.f) + std::pow(2 * ((mouseY - end.y + rounding * 2) / (rounding * 2) - 0.5), 2)) <= 1;
				else
					hovered = true;
			}
			else hovered = true;
		}
		else {
			double mX = 2.0 * ((mouseX - start.x) / (end.x - start.x) - 0.5);
			double mY = 2.0 * ((mouseY - start.y) / (end.y - start.y) - 0.5);
			hovered = (mX*mX + mY * mY) <= 1;
		}
	}
	else hovered = false;
	lPressed = (hovered && (mouse::l));
	lClicked = (hovered && (mouse::lClicked));
	lReleased = (hovered && (mouse::lReleased));
	rPressed = (hovered && (mouse::r));
	rClicked = (hovered && (mouse::rClicked));
	rReleased = (hovered && (mouse::rReleased));

	if (draggable&&GUI) {
		if (this->end.x > max.x) {
			this->end.x = max.x;
			this->start.x = max.x - w;
		}
		if (this->start.x < min.x) {
			this->start.x = min.x;
			this->end.x = min.x + w;
		}
		if (this->end.y > max.y) {
			this->end.y = max.y;
			this->start.y = max.y - h;
		}
		if (this->start.y < min.y) {
			this->start.y = min.y;
			this->end.y = min.y + h;
		}
	}
}

void MouseDetector::constrain(glm::vec3 minimum, glm::vec3 maximum) {
	min = minimum;
	max = maximum;
}