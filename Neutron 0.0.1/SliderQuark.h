#pragma once
#include "Quark.h"
#include "Neutron.h"
#include "MouseDetector.h"
using namespace neutron::frame;

class SliderQuark : public Quark {
public:
	static enum {
		// Orientation
		VERTICAL = 0, HORIZONTAL = 1,
		// Look Type
		FLAT, JARVOPIA,
		// Float4
		PRIMARYCOLOR = LAST_FLOAT4 + 1, LOCKEDCOLOR = LAST_FLOAT4 + 2, HOVEREDCOLOR = LAST_FLOAT4 + 3, DRAGGEDCOLOR = LAST_FLOAT4 + 4, BODYCOLOR = LAST_FLOAT4 + 5,
		//Float
		CORNERROUNDING = LAST_FLOAT + 1, BEVEL = LAST_FLOAT + 2,
	};
	
	unsigned char lookType; 
	unsigned char orientation = 1;

	double value = 0, maxVal, minVal;

	SliderQuark();
	SliderQuark(glm::vec3 position, glm::vec3 bounds, double minValue, double maxValue, double standardValue, unsigned const char orientation = HORIZONTAL) : Quark(position + bounds * glm::vec3(0.5), 0.f, bounds*glm::vec3(0.5), glm::vec3(0)) {
		this->position.z = position.z;
		lookType = FLAT;
		this->value = standardValue;
		maxVal = maxValue;
		minVal = minValue;
		this->orientation = orientation;

		body = MouseDetector(position, position + bounds);

		glm::vec3 startOffset;
		float minS = min(bounds.x, bounds.y);
		if (orientation)
			startOffset = glm::vec3(processor::mapTo(value, maxVal, minVal, 2, 0)*(size.x - minS / 2) , 0, 0);
		else
			startOffset = glm::vec3(0, processor::mapTo(value, maxVal, minVal, 2, 0)*(size.y - minS / 2), 0);
		knob = MouseDetector(position + startOffset, position + startOffset + glm::vec3(minS, minS, 1));
		knob.draggable = true;
		updateKnobConstraints();
		
		setB(GUI, true);
		setColorPallete(glm::vec4(0.5, 0.5, 0.5, 1));

		setF(CORNERROUNDING, size.y * orientation + size.x * (1-orientation));
		knob.rounding = getF(CORNERROUNDING);
		setF(BEVEL, 0.15f);
	}
	void setBodyBounds(glm::vec3 start, glm::vec3 end) {
		float knbX = ((knob.start.x + knob.end.x) / 2);
		float knSx = ((knob.end.x - knob.start.x) / 2);
		float knbY = ((knob.start.y + knob.end.y) / 2);
		float knSy = ((knob.end.y - knob.start.y) / 2);
		if (orientation == 1)
			value = ((knbX - position.x - knSx + size.x) / (size.x * 2 - knSx * 2))*(maxVal - minVal) + minVal;
		else value = ((knbY - position.y - knSy + size.y) / (size.y * 2 - knSy * 2))*(maxVal - minVal) + minVal;

		glm::vec3 off = knob.end-knob.start;
		body.start = start;
		body.end = end;
		position = (start + end)*glm::vec3(0.5);
		size = (end - start)*glm::vec3(0.5);

		glm::vec3 startOffset;
		if (orientation)
			startOffset = glm::vec3(processor::mapTo(value, maxVal, minVal, 1, -1), 0, 0);
		else
			startOffset = glm::vec3(0, processor::mapTo(value, maxVal, minVal, 1, -1), 0);

		knob.end = position + startOffset*(size-off / glm::vec3(2.0)) + off / glm::vec3(2.0);
		knob.start = position + startOffset*(size - off / glm::vec3(2.0)) - off / glm::vec3(2.0);

		updateKnobConstraints();
	}
	void changeKnobBounds(glm::vec3 start, glm::vec3 size) {
		glm::vec3 offset = knob.start - position;
		knob.constrain(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
		knob.start = position + start + offset;
		knob.end = position + start + size + offset;

		updateKnobConstraints();
	}
	void setPosition(glm::vec3 pos) {
		setBodyBounds(pos-size, pos + size);
	}
	void changeKnobRounding(float knobRounding) {
		knob.rounding = knobRounding;
	}
	void setLockedTo(bool locked) {
		body.locked = locked;
		knob.locked = locked;
	}
	void tick() {
		body.rounding = getF(CORNERROUNDING);
		//knob.rounding = getF(CORNERROUNDING);
		body.GUI = getB(GUI);
		knob.GUI = getB(GUI);
		body.tick();
		knob.tick();

		if (strafe!=0)
			knob.lPressed = false;
		
		if (body.hovered)
			frame::selected_component = this;

		float knbX = ((knob.start.x + knob.end.x) / 2);
		float knSx = ((knob.end.x - knob.start.x) / 2);
		float knbY = ((knob.start.y + knob.end.y) / 2);
		float knSy = ((knob.end.y - knob.start.y) / 2);
		if (orientation == 1)
			value = ((knbX - position.x - knSx + size.x) / (size.x*2-knSx*2))*(maxVal-minVal) + minVal;
		else value = ((knbY - position.y - knSy + size.y) / (size.y * 2 - knSy * 2))*(maxVal - minVal) + minVal;

		if (body.lReleased && strafe<8)
			if (orientation == 1) {
				float s = knob.end.x - knob.start.x;
				knob.start.x = (mouse::x - (knob.end.x - knob.start.x) / 2);
				knob.end.x = knob.start.x + s;
				knob.tick();
			}
			else {
				float s = knob.end.y - knob.start.y;
				knob.start.y = (mouse::y - (knob.end.y - knob.start.y) / 2);
				knob.end.y = knob.start.y + s;
				knob.tick();
			}

		if (body.lPressed && !knob.lPressed) {
			++strafe;
			if (orientation == 1) {
				float s = knob.end.x - knob.start.x;
				knob.start.x += (mouse::x - (knob.start.x + knob.end.x) / 2)*0.005;
				knob.end.x = knob.start.x + s;
				knob.tick();
				if (abs(mouse::x - knob.start.x)<5)
					strafe = 0;
			}
			else {
				float s = knob.end.y - knob.start.y;
				knob.start.y += (mouse::y - (knob.start.y + knob.end.y) / 2)*0.005;
				knob.end.y = knob.start.y + s;
				knob.tick();
				if (abs(mouse::y - knob.start.y) < 5)
					strafe = 0;
			}
		}
		else
			strafe = 0;
	}
	void render() {
		if (body.locked) {
			setF4(ADD_COLOR, glm::vec4(0.1, 0.1, 0.1, 0));
			setF4(MULT_COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
			setF4(COLOR, getF4(LOCKEDCOLOR));
		}
		else if (body.lPressed||knob.lPressed) {
			setF4(ADD_COLOR, glm::vec4(0.075, 0.075, 0.075, 0));
			setF4(MULT_COLOR, glm::vec4(1.1));
			setF4(COLOR, getF4(DRAGGEDCOLOR));
		}
		else if (body.hovered||knob.hovered) {
			setF4(ADD_COLOR, glm::vec4(0.075, 0.075, 0.075, 0));
			setF4(MULT_COLOR, glm::vec4(1));
			setF4(COLOR, getF4(HOVEREDCOLOR));
		}
		else {
			setF4(ADD_COLOR, glm::vec4(0));
			setF4(MULT_COLOR, glm::vec4(1));
			setF4(COLOR, getF4(PRIMARYCOLOR));
		}
		set();

		passes::cust_float4s["bounds"] = glm::vec4(size, 1);
		passes::cust_floats["rounding"] = getF(CORNERROUNDING);
		passes::cust_floats["bevel"] = getF(BEVEL);
		if (orientation == 1)
			passes::cust_floats["bevel"] = getF(BEVEL) * (size.y / size.x);

		if (lookType == FLAT) {
			passes::shader = passes::SHADER_BUTTON;
			passes::add_color = glm::vec4(-0.2, -0.2, -0.2, 0);
			passes::mult_color = glm::vec4(0.9, 0.9, 0.9, 1);
			draw(0, getF4(BODYCOLOR), layer, 6);
			passes::transform = processor::rect(knob.start.x, knob.start.y, knob.end.x - knob.start.x, knob.end.y - knob.start.y);
			passes::cust_float4s["bounds"] = glm::vec4(knob.end.x - knob.start.x, knob.end.y - knob.start.y, 0, 1);
			passes::cust_floats["bevel"] = getF(BEVEL);
			passes::cust_floats["rounding"] = knob.rounding*2;
			passes::add_color = getF4(ADD_COLOR);
			passes::mult_color = getF4(MULT_COLOR);
			draw(0, getF4(COLOR), layer, 6);
		}
		else if (lookType == JARVOPIA) {
			passes::shader = passes::SHADER_BUTTON;
			passes::add_color = glm::vec4(-0.2, -0.2, -0.2, 0);
			passes::mult_color = glm::vec4(1.8, 1.8, 1.8, 1);
			draw(processor::getTextureFrom("res/textures/JARvopiaRect.png"), getF4(BODYCOLOR), layer, 6);
			passes::transform = processor::rect(knob.start.x, knob.start.y, knob.end.x - knob.start.x, knob.end.y - knob.start.y);
			passes::cust_float4s["bounds"] = glm::vec4(knob.end.x - knob.start.x, knob.end.y - knob.start.y, 0, 1);
			passes::cust_floats["bevel"] = getF(BEVEL);
			passes::cust_floats["rounding"] = knob.rounding * 2;
			passes::add_color = getF4(ADD_COLOR);
			passes::mult_color = getF4(MULT_COLOR)*glm::vec4(2, 2, 2, 1);
			draw(processor::getTextureFrom("res/textures/JARvopiaRect.png"), getF4(COLOR), layer, 6);
		}
		passes::shader = passes::SHADER_FLAT;
	}
	void setColorPallete(glm::vec4 color) {
		setF4(PRIMARYCOLOR, color);
		setF4(LOCKEDCOLOR, color);
		setF4(HOVEREDCOLOR, color);
		setF4(DRAGGEDCOLOR, color);
		setF4(BODYCOLOR, color);
	}

	void useClassicJarvopiaTemplate() {
		lookType = SliderQuark::JARVOPIA;
		setF(SliderQuark::CORNERROUNDING, 8);
		setF4(SliderQuark::BODYCOLOR, glm::vec4(0.7, 0.7, 0.7, 1));
		changeKnobBounds(glm::vec3(10, -10, 0), glm::vec3(30, 70, 0));
		changeKnobRounding(8);
		setColorPallete(glm::vec4(0.7, 0.7, 0.7, 1));
		rotation = glm::vec3(0, 0, 3.14159265);
	}
private:
	MouseDetector body = MouseDetector(glm::vec3(0), glm::vec3(0)), knob = MouseDetector(glm::vec3(0), glm::vec3(0));
	int strafe = 0;
	void updateKnobConstraints() {
		if (orientation == 1) {
			glm::vec3 n = glm::vec3(position.x - size.x, knob.start.y, 0);
			glm::vec3 m = glm::vec3(position.x + size.x, knob.end.y, 0);
			knob.constrain(n, m);
		}
		else {
			glm::vec3 n = glm::vec3(knob.start.x, position.y - size.y, 0);
			glm::vec3 m = glm::vec3(knob.end.x, position.y + size.y, 0);
			knob.constrain(n, m);
		}
	}
};

