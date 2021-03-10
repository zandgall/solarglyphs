#pragma once
#ifndef BUTTONQUARK_H
#define BUTTONQUARK_H

#include "Quark.h"
#include "Neutron.h"
#include "MouseDetector.h"
#include <iostream>
using namespace neutron;
class ButtonQuark : public Quark {
public:
	static enum {
		// Interaction Type
		STANDARD, TOGGLE, 
		// Look Type
		FLAT, JARVOPIA, TEXTONLY, OUTLINE,
		// Shape
		RECT = 0, ELLIPSE = 1,
		// Float4
		PRIMARYCOLOR = LAST_FLOAT4 + 1, LOCKEDCOLOR = LAST_FLOAT4+2, HOVEREDCOLOR = LAST_FLOAT4+3, ONCOLOR = LAST_FLOAT4+4, HOVEREDONCOLOR = LAST_FLOAT4+5,		
		// Float4
		PRIMARYTEXTCOLOR = LAST_FLOAT4 + 6, LOCKEDTEXTCOLOR = LAST_FLOAT4 + 7, HOVEREDTEXTCOLOR = LAST_FLOAT4 + 8, ONTEXTCOLOR = LAST_FLOAT4 + 9, HOVEREDONTEXTCOLOR = LAST_FLOAT4 + 10,
		LAST_FLOAT4 = HOVEREDONTEXTCOLOR,
		// Float
		CORNERROUNDING = LAST_FLOAT+1, BEVEL = LAST_FLOAT + 2,
		LAST_FLOAT = BEVEL,
	};
	// Whether or not the button has been pressed, or is toggled
	bool on = false;
	// How the button will render
	unsigned char lookType;
	// How the button will be interacted with
	unsigned char interactionType;

	// Empty Constructor
	ButtonQuark() {

	}
	 //Constructor with Parameters
	ButtonQuark(glm::vec3 position, glm::vec3 bounds, glm::vec3 rotation = glm::vec3(0)) : Quark(position+bounds*glm::vec3(0.5), 0.f, bounds*glm::vec3(0.5), rotation) {
		this->position.z = position.z;	// Keep the passed position's z value, but with changes to x and y based on size
		input = MouseDetector(position, position + bounds);	// Create a mouse detector, used for interaction
		
		prePos = this->position;	// Make a prePos variable to check if the user has tried to move the button this way
		preSize = size;				// Make a preSize variable, to check if the user has tried to rescale the button this way
		preRot = rotation;			// Make a preRot variable, to check if the user has tried to rotate the button this way
		
		lookType = FLAT;			// Default look type
		interactionType = STANDARD;	// and interaction type
		setB(GUI, true);			// Set GUI to true
		
		setColorPallete(glm::vec4(0.5, 0.5, 0.5, 1));	// Set default colors
		setTextPallete(glm::vec4(0, 0, 0, 1));			// For text too

		setF(CORNERROUNDING, 15.f);	// Default rounded corners,
		setF(BEVEL, 0.1f);			// bevel,
		setS(TEXT, "Sample Text");	// text,
		setS(FONT, "arial");		// and font
	}
	// Set the bounds and/or position
	void setBounds(glm::vec3 start, glm::vec3 end){
		input.start = start;					// Change the input begin
		input.end = end;						// and end point,
		position = (start + end)*glm::vec3(0.5);// Move the position
		size = (end - start)*glm::vec3(0.5);	// Change the size,
		prePos = position;						// Update the prePos
		preSize = size;							// and preSize
	}
	// Set specifically the position
	void setPosition(glm::vec3 pos) {
		setBounds(pos-size, pos+size);	// Update bounds based on size

	}
	// Set the shape of the input
	void setShape(unsigned int type) {
		input.shape = type;
	}
	// Change if the button is locked or not
	void setLockedTo(bool locked) {
		input.locked = locked;
	}
	// Update the button and check for input
	void tick() {
		input.rounding = getF(CORNERROUNDING);	// Update input's rounding
		input.GUI = getB(GUI);					// And gui flag,
		input.tick();							// Then check input
		if (input.hovered)						// if the input is hovered,
			frame::selected_component = this;	// select this component
		if (interactionType == TOGGLE) {		// if the interaction type is toggle,
			if (input.lClicked)					// if there's input,
				on = !on;						// invert on
		}
		else
			on = input.lClicked;				// Otherwise set on equal to the given input
		displayOn = on ? 10 : displayOn;		// Set a display delay, so the 'on' state renders a few more times
		if (input.lClicked && onClicked != nullptr)	// If there is input, and a function to run when clicked,
			onClicked();							// run the "onClicked" function
	}
	// Render the button
	void render() {
		if (!frozen)					// If it's not frozen,
			freezeTextures();			//		freeze it
		set();							// Set the quark variables
		passes::mult_color = glm::vec4(1);	// Make sure colors aren't altered
		passes::add_color = glm::vec4(0);	// Make sure colors aren't altered
		if (input.locked)				
			draw(frozenLocked);			
		else if (on && input.hovered)	// Renders when in toggle state, and button is hovered while it's toggled
			draw(frozenToggled);
		else if (displayOn)
			draw(frozenOn);
		else if (input.hovered)
			draw(frozenHovered);
		else 
			draw(frozenNorm);
		if(displayOn)
			displayOn--;				// Reduce the display on delay every loop
	}
	// Set the "on clicked" function
	void onClickedDo(void(*function)()) {
		onClicked = function;
	}
	// Set the color pallete based on one color
	void setColorPallete(glm::vec4 color) {
		setF4(PRIMARYCOLOR, color);
		setF4(LOCKEDCOLOR, color);
		setF4(HOVEREDCOLOR, color);
		setF4(ONCOLOR, color);
		setF4(HOVEREDONCOLOR, color);
		frozen = false;
	}
	// Set the text's pallete based on one color
	void setTextPallete(glm::vec4 color) {
		setF4(PRIMARYTEXTCOLOR, color);
		setF4(LOCKEDTEXTCOLOR, color);
		setF4(HOVEREDTEXTCOLOR, color);
		setF4(ONTEXTCOLOR, color);
		setF4(HOVEREDONTEXTCOLOR, color);
		frozen = false;
	}

	// Whenever someone changes something, unfreeze the button in order to apply the change

	virtual void setVao(unsigned int value, unsigned int vertCount) {
		while (UINTS.size() <= VAO)
			UINTS.push_back(0);
		UINTS[VAO] = value;
		this->vertCount = vertCount;
		frozen = false;
	}

	virtual void setUINT(unsigned int id, unsigned int value) {
		while (UINTS.size() <= id)
			UINTS.push_back(0);
		if (id == VAO) {
			std::cout << "SET VAO THROUGH SETVAO" << std::endl;
			throw EXCEPTION_INVALID_HANDLE;
		}

		UINTS[id] = value;
		frozen = false;
	}

	virtual void setF(unsigned int id, float value) {
		while (FLOATS.size() <= id)
			FLOATS.push_back(0);
		FLOATS[id] = value;
		frozen = false;
	}

	virtual void setF4(unsigned int id, glm::vec4 value) {
		while (FLOAT4S.size() <= id)
			FLOAT4S.push_back(glm::vec4(0));
		FLOAT4S[id] = value;
		frozen = false;
	}

	virtual void setB(unsigned int id, bool value) {
		while (BOOLS.size() <= id)
			BOOLS.push_back(false);
		BOOLS[id] = value;
		frozen = false;
	}

	virtual void setM4(unsigned int id, glm::mat4 value) {
		while (MATS.size() <= id)
			MATS.push_back(glm::mat4(0));
		MATS[id] = value;
		frozen = false;
	}

	virtual void setS(unsigned int id, const std::string &value) {
		while (STRINGS.size() <= id)
			STRINGS.push_back("");
		STRINGS[id] = value;
		frozen = false;
	}
private:
	glm::vec3 prePos = glm::vec3(0);
	glm::vec3 preSize = glm::vec3(0);
	glm::vec3 preRot = glm::vec3(0);
	MouseDetector input = MouseDetector(glm::vec3(0), glm::vec3(0));
	void(*onClicked)() = nullptr;
	bool frozen = false;
	unsigned int frozenNorm = 0, frozenLocked = 0, frozenToggled = 0, frozenOn = 0, frozenHovered = 0;
	int displayOn = 0;

	// Freeze a texture
	void freeze(unsigned int *texture) {
		// Start drawing to the texture
		processor::drawToTexture2D(*texture, size.x*2, size.y*2, GL_LINEAR, GL_RGBA);
		modes::flip_vertically = !modes::flip_vertically;		// Flip vertically for fix
		if (texture == &frozenLocked) {							// If drawing locked state, use locked colors
			setF4(ADD_COLOR, glm::vec4(0.1, 0.1, 0.1, 0));
			setF4(MULT_COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
			setF4(COLOR, getF4(LOCKEDCOLOR));
		}
		else if (texture == &frozenToggled) {					// If drawing toggled state, use toggled colors
			setF4(ADD_COLOR, glm::vec4(-0.2, -0.2, -0.2, 0));
			setF4(MULT_COLOR, glm::vec4(0.9, 0.9, 0.9, 1));
			setF4(COLOR, getF4(HOVEREDONCOLOR));
		}
		else if (texture == &frozenOn) {						// If drawing on state, use on colors
			setF4(ADD_COLOR, glm::vec4(-0.05, -0.05, -0.05, 0));
			setF4(MULT_COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
			setF4(COLOR, getF4(ONCOLOR));
		}
		else if (texture == &frozenHovered) {					// If drawing hovered state, use hovered colors
			setF4(ADD_COLOR, glm::vec4(0.075, 0.075, 0.075, 0));
			setF4(MULT_COLOR, glm::vec4(1));
			setF4(COLOR, getF4(HOVEREDCOLOR));
		}
		else {													// If drawing default state, use default colors
			setF4(ADD_COLOR, glm::vec4(0));
			setF4(MULT_COLOR, glm::vec4(1));
			setF4(COLOR, getF4(PRIMARYCOLOR));
		}
		passes::transform = processor::form(glm::vec3(1, -1, 0)*size, this->size);	// Set transformation, translating up for fixed vertical flip
		passes::add_color = getF4(ADD_COLOR);	// Set add
		passes::mult_color = getF4(MULT_COLOR);	// And mult color

		passes::cust_float4s["bounds"] = glm::vec4(size, 1);	// Set shader's bounds variable size
		passes::cust_floats["rounding"] = getF(CORNERROUNDING);	// Set the shader's rounding
		passes::cust_floats["bevel"] = getF(BEVEL);				// Set shader's bevel

		if (lookType == FLAT) {				// Draw flat
			passes::shader = passes::SHADER_BUTTON;
			draw(0, getF4(COLOR), layer, 6);
		}
		else if (lookType == JARVOPIA) {	// Draw JARvopia
			passes::shader = passes::SHADER_BUTTON;
			passes::mult_color = getF4(MULT_COLOR)*glm::vec4(2, 2, 2, 1); // Correct for darker texture
			draw(processor::getTextureFrom("res/textures/JARvopiaButton.png", GL_NEAREST), getF4(COLOR), layer, 6);
		}
		else if (lookType == OUTLINE) {		// Draw only outline
			passes::shader = neutron::processor::LoadShader("res/shaders/buttonOutline.shader");
			draw(0, getF4(COLOR), layer, 6);
		}


		// Select text colors
		if (texture == &frozenLocked)
			setF4(COLOR, getF4(LOCKEDTEXTCOLOR));
		else if (texture == &frozenToggled)
			setF4(COLOR, getF4(HOVEREDONTEXTCOLOR));
		else if (texture == &frozenOn)
			setF4(COLOR, getF4(ONTEXTCOLOR));
		else if (texture == &frozenHovered)
			setF4(COLOR, getF4(HOVEREDTEXTCOLOR));
		else
			setF4(COLOR, getF4(PRIMARYTEXTCOLOR));

		// Draw flat, as a glyph, at a different height
		passes::shader = passes::SHADER_FLAT;
		modes::draw = modes::DRAW_GLYPH;
		passes::transform = processor::form(glm::vec3(1, -1.25, 0)*size, this->size);
		drawText(getS(TEXT), getS(FONT).c_str(), 0, getF4(COLOR), layer, 6);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);				// Unbind texture, set new draw path to screen
		modes::flip_vertically = !modes::flip_vertically;	// Set vertical flip back
	}

	// Freeze all textures
	void freezeTextures() {
		set();
		freeze(&frozenNorm);
		freeze(&frozenHovered);
		freeze(&frozenLocked);
		freeze(&frozenOn);
		freeze(&frozenToggled);
		frozen = true;
	}

};

#endif