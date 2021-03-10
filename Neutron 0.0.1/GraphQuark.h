#pragma once
#include "Quark.h"
class GraphQuark :
	public Quark
{
public:
	static enum {
		//Type of graph
		PIE = 0
	};

	std::vector<glm::vec3> colors = std::vector<glm::vec3>();
	std::vector<float> values = std::vector<float>();
	std::vector<std::string> label = std::vector<std::string>();

	int type = 0;

	GraphQuark(int type = 0);
	~GraphQuark();

	void add(std::string name, float value, glm::vec3 color) {
		label.push_back(name);
		values.push_back(value);
		colors.push_back(color);
	}

	float getValue(std::string name) {
		for (int i = 0; i < label.size(); i++)
			if (label[i] == name)
				return values[i];
		return 0.0f;
	}

	glm::vec3 getColor(std::string name) {
		for (int i = 0; i < label.size(); i++)
			if (label[i] == name)
				return colors[i];
		return glm::vec3(0.0f);
	}

	void setValue(std::string name, float value) {
		for (int i = 0; i < label.size(); i++)
			if (label[i] == name)
				values[i] = value;
	}

	void setColor(std::string name, glm::vec3 color) {
		for (int i = 0; i < label.size(); i++)
			if (label[i] == name)
				colors[i] = color;
	}

	void setName(std::string name, std::string newName) {
		for (int i = 0; i < label.size(); i++)
			if (label[i] == name)
				label[i] = newName;
	}

	void remove(std::string name) {
		for (int i = 0; i < label.size(); i++) {
			if (label[i] == name) {
				label.erase(label.begin() + i);
				values.erase(values.begin() + i);
				colors.erase(colors.begin() + i);
			}
		}
	}

	void tick();
	void render();
private:
	float getTotalPIEValue();
	unsigned int PIEshader = 0;
};

