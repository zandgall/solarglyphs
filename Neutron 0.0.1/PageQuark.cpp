#include "PageQuark.h"

PageQuark::~PageQuark() {}

void PageQuark::render() {
	set();
	passes::cust_float4s["bounds"] = glm::vec4(size, 1);
	passes::cust_floats["rounding"] = getF(CORNERROUNDING);
	passes::cust_floats["bevel"] = getF(BEVEL);

	if (lookType == FLAT) {
		passes::shader = passes::SHADER_BUTTON;
		draw(0, getF4(COLOR), layer, 6);
	}
	else if (lookType == JARVOPIA) {
		passes::shader = passes::SHADER_BUTTON;
		passes::mult_color = getF4(MULT_COLOR)*glm::vec4(2, 2, 2, 1);
		draw(neutron::processor::getTextureFrom("res/textures/JARvopiaButton.png", GL_NEAREST), getF4(COLOR), layer, 6);
	}
	else if (lookType == OUTLINE) {
		passes::shader = neutron::processor::LoadShader("res/shaders/buttonOutline.shader");
		draw(0, getF4(COLOR), layer, 6);
	}
	if(roundingMask==0)
		_updateContentMask();
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glViewport(0, 0, size.x*2, size.y*2);				// Assign viewport to match the framebuffer size
	glClearColor(0.0, 0.0, 0.0, 0.0);					// Set background color to clear
	glClear(GL_COLOR_BUFFER_BIT);						// Use background color as background
	passes::globalTransform = glm::translate(passes::globalTransform, glm::vec3(scroll_x, scroll_y, 0));
	passes::shader = passes::SHADER_FLAT;
	modes::draw = modes::DRAW_FLAT;
	for (std::map<std::string, Quark*>::iterator q = objects.begin(); q != objects.end(); q++) {
		q->second->position.y = size.y*2 - q->second->position.y;
		q->second->render();
		q->second->position.y = size.y*2 - q->second->position.y;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	set();
	passes::globalTransform = glm::translate(passes::globalTransform, glm::vec3(-scroll_x, -scroll_y, 0));
	modes::draw = modes::DRAW_FLAT;
	modes::gui = true; /// Maybe not in the future
	passes::shader = passes::SHADER_FLAT;
	//modes::flip_vertically = true;
	passes::mask_texture = roundingMask;
	passes::transform = processor::form(position, glm::vec3(1,1,1)*size, rotation);
	draw(texture, glm::vec4(0));
	//modes::flip_vertically = false;
}

void PageQuark::tick() {
	if (mouse::x >= position.x - size.x && mouse::x <= position.x + size.x && mouse::y >= position.y - size.y && mouse::y <= position.y + size.y) {
		selected_component = this;
		if (mouse::y_s_g != 0) {
			if (contentBounds.y <= size.y*2 || keys::keys[340])
				scroll_x -= mouse::y_s_g*(keys::keys[341] ? 16.0 : keys::keys[342] ? 1 : 4.0);
			else scroll_y -= mouse::y_s_g * (keys::keys[341] ? 16.0 : keys::keys[342] ? 1 : 4.0);
		}
		if (mouse::x_s_g != 0) {
			if (contentBounds.x <= size.x*2 || keys::keys[340])
				scroll_y -= mouse::y_s_g*keys::keys[341] ? 16.0 : keys::keys[342] ? 1 : 4.0;
			else scroll_x -= mouse::y_s_g*keys::keys[341] ? 16.0 : keys::keys[342] ? 1 : 4.0;
		}

		scroll_y = max(min(scroll_y, contentBounds.y - size.y*2), 0);
		scroll_x = max(min(scroll_x, contentBounds.x - size.x*2), 0);
	}
}

void PageQuark::calculateAutoContentBounds() {
	contentBounds = glm::vec2(0);
	for (std::map<std::string, Quark*>::iterator q = objects.begin(); q != objects.end(); q++) {
		glm::vec2 s = q->second->position + q->second->size;
		std::cout << q->first << ": " << glm::to_string(s) << std::endl;
		contentBounds.x = max(contentBounds.x, s.x);
		contentBounds.y = max(contentBounds.y, s.y);
	}
}

void PageQuark::_updateContentMask() {
	processor::drawToTexture2D(roundingMask, size.x*2, size.y*2, GL_LINEAR, GL_RGB);

	passes::transform = processor::form(size, size, rotation);

	passes::cust_float4s["bounds"] = glm::vec4(size, 1);
	passes::cust_floats["rounding"] = getF(CORNERROUNDING);
	passes::cust_floats["bevel"] = getF(BEVEL);

	if (lookType == FLAT) {
		passes::shader = passes::SHADER_BUTTON;
		draw(0, glm::vec4(1), 0, 6);
	}
	else if (lookType == JARVOPIA) {
		passes::shader = passes::SHADER_BUTTON;
		passes::add_color = glm::vec4(1, 1, 1, 0);
		draw(neutron::processor::getTextureFrom("res/textures/JARvopiaButton.png"), glm::vec4(1), 0, 6);
	}
	else if (lookType == OUTLINE) {
		passes::shader = neutron::processor::LoadShader("res/shaders/buttonOutline.shader");
		draw(0, glm::vec4(1), 0, 6);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}