#include "ParticleQuark.h"
#include <algorithm>

unsigned int iVBO = 0;

ParticleQuark::ParticleQuark() {
	instances = std::vector<glm::mat4>();
	if (!neutron::isSet())
		return;
	setUINT(SHADER, passes::SHADER_INSTANCE);
	setUINT(DRAW, modes::DRAW_GLYPH);
	setUINT(DISTANCE_TEXTURE, processor::getTextureFrom("res/textures/circleDist.png"));
	setB(GUI, false);
}


ParticleQuark::~ParticleQuark() {
}

void ParticleQuark::addInstance(vec3 position, vec3 scale, vec3 rotation) {
	addInstance(processor::form(position, scale, rotation));
}

void ParticleQuark::addInstance(mat4 matrix) {
	instances.push_back(matrix);
	unsigned int vao = getUINT(VAO);
	if(iVBO == 0)
		glGenBuffers(1, &iVBO);
	glBindBuffer(GL_ARRAY_BUFFER, iVBO);
	glBufferData(GL_ARRAY_BUFFER, instances.size()*sizeof(mat4), instances.data(), GL_STATIC_DRAW);

	glBindVertexArray(vao);
	int vec4Size = sizeof(vec4);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	setVao(vao, getVertCount());
	glBindVertexArray(0);

}

bool sortParticles(mat4 a, mat4 b) {
	return (distance(vec3(a[3]), world::camera.position) > distance(vec3(b[3]), world::camera.position));
}

void ParticleQuark::render() {
	if (instances.size() == 0)
		return;
	if (!getB(GUI)) {
		std::sort(instances.begin(), instances.end(), &sortParticles);
		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(mat4), instances.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	set();
	neutron::drawInstances(instances, getUINT(TEXTURE), getF4(COLOR), layer, getVertCount());
}