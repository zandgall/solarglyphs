#pragma once
#include "Quark.h"
#include "SliderQuark.h"
class PageQuark : public Quark {
public:
	static enum {
		// Look Type
		FLAT, JARVOPIA, OBJECTSONLY, OUTLINE,
		// Float
		CORNERROUNDING = LAST_FLOAT + 1, BEVEL = LAST_FLOAT + 2,
	};
	int lookType = FLAT;
	PageQuark() {
		
	}
	PageQuark(glm::vec3 position, glm::vec3 bounds, glm::vec3 contentBounds) {
		this->position = position+bounds*glm::vec3(.5); this->size = bounds*glm::vec3(.5); this->contentBounds = contentBounds;
		setF(CORNERROUNDING, 15.f);
		setF(BEVEL, 0.1f);
		lookType = FLAT;
		setF4(COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
		setB(GUI, true);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texture);				// gen texture,
		glBindTexture(GL_TEXTURE_2D, texture);  // and then bind it
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bounds.x, bounds.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use set interpolation close up
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// ..and far away
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);	// Set a Color attachment to the frame buffer
		glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	~PageQuark();

	SliderQuark page_scroll_y, page_scroll_x;

	glm::vec2 contentBounds = glm::vec2();

	float scroll_y = 0, scroll_x = 0;
	
	void put(std::string id, Quark *object) {
		objects.insert(std::make_pair(id, object));
	}

	Quark* at(std::string id) {
		return objects[id];
	}
	
	void updateBounda(glm::vec3 bounds) {
		position = position - size + bounds * glm::vec3(0.5);
		this->size = bounds * glm::vec3(0.5);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glBindTexture(GL_TEXTURE_2D, texture);  // and then bind it

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bounds.x, bounds.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use set interpolation close up
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// ..and far away
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);	// Set a Color attachment to the frame buffer
		glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		updateContentMask();
	}

	void calculateAutoContentBounds();

	void tick();

	void render();

	void updateContentMask() {
		roundingMask = 0;
	}


	// Default page elements

	class Splitter: public Quark {

	public: 
		Splitter() : Quark(glm::vec3(0), 0, glm::vec3(1000000, 3, 0)){
			setF4(COLOR, glm::vec4(0, 0, 0, 1));
			setB(GUI, true);
		}

		Splitter(float y, float thickness = 1.0, float width = 10000000.f, float offset = 0) : Quark(glm::vec3(offset, y, 0), 0, glm::vec3(width/2, thickness, 0)) {
			setF4(COLOR, glm::vec4(0, 0, 0, 1));
			setB(GUI, true);
		}
	};

private: 
	unsigned int texture = 0, framebuffer = 0, roundingMask = 0;
	std::map<std::string, Quark*> objects;
	void _updateContentMask();
};

