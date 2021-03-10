#pragma once
#include "Quark.h"
#include "ParticleQuark.h"
class ParagraphQuark : public Quark {
public:
	static enum {
		// Floats
		WIDTH = LAST_FLOAT + 1, FONT_SIZE = LAST_FLOAT + 2, INDENT = LAST_FLOAT + 3, LINE_SPACING = LAST_FLOAT + 4,
		LAST_FLOAT = LINE_SPACING,
		// Ints
		ALIGNMENT = LAST_UINT + 1, LAST_UINT = ALIGNMENT,
	};

	ParagraphQuark() {
		setS(TEXT, "");
		setS(FONT, "arial");
	}
	static const char ALIGN_CENTER = 0, ALIGN_LEFT = 1, ALIGN_RIGHT = 2;
	ParagraphQuark(glm::vec3 position, std::string text, float width, float size = 12.0f, float indent = 25.0f, std::string font = "arial");
	~ParagraphQuark();
	void setPosition(glm::vec3 position) {
		this->position = position+size*glm::vec3(0.5);
	}

	void freezeToTexture();

	virtual void render();

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
	std::vector<std::string> lines;
	unsigned int texture = 0;
	bool frozen = false;

	void updateBounds();
};

