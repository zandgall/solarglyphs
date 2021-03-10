#include "GraphQuark.h"



GraphQuark::GraphQuark(int type) : Quark(glm::vec3(0), 0.f, glm::vec3(1), glm::vec3(0)) {
	this->type = type;
	if(neutron::isSet())
		PIEshader = neutron::processor::LoadShader("res/shaders/PIEGraph.shader");
	setB(GUI, true);
}


GraphQuark::~GraphQuark() {
	
}


void GraphQuark::tick() {

}

void GraphQuark::render() {
	if (type == PIE) {
		setUINT(SHADER, PIEshader);
		set();
		float full = getTotalPIEValue();
		passes::cust_texures["slices"] = values.size();
		float off = 0;
		for (int i = 0; i < values.size(); i++) {
			passes::cust_floats["values[" + std::to_string(i) + "]"] = values[i] / full + off;
			off += values[i] / full;
			passes::cust_float3s["colors[" + std::to_string(i) + "]"] = colors[i];
		}

		glCheckError();
		neutron::draw();
		glCheckError();
	}
}

float GraphQuark::getTotalPIEValue() {
	float out = 0;
	for (float i : values)
		out += i;
	return out;
}