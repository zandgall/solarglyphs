#include "Quark.h"

#include "FileIO/FileChooser.h"

Quark::Quark(glm::vec3 position, float layer, glm::vec3 size, glm::vec3 rotation) {
	this->position = position;
	this->layer = layer;
	this->size = size;
	this->rotation = rotation;
	UINTS = std::vector<unsigned int>();
	FLOATS = std::vector<float>();
	BOOLS = std::vector<bool>();
	FLOAT4S = std::vector<glm::vec4>();
	setUINT(SHADER, passes::SHADER_FLAT);
	setUINT(DRAW, modes::DRAW_FLAT);
	setVao(passes::VAO_FLAT, 6);
	setF4(COLOR, glm::vec4(1));
	setF4(MULT_COLOR, glm::vec4(1));
	setF4(ADD_COLOR, glm::vec4(0));
	setF(GLYPH_CLIP, 0.5f);
	setF(GLYPH_SOFT_EDGE, 0.1f);
	setM4(TEXTURE_TRANSFORM, processor::form(glm::vec4(0)));
	setB(ALPHA_TEST, true);
}

Quark::~Quark()
{
}

void Quark::tick() {}

void Quark::set() {
	using namespace neutron::passes;
	mult_texture = getUINT(MULT_TEXTURE);
	add_texture = getUINT(ADD_TEXTURE);
	distance_texture = getUINT(DISTANCE_TEXTURE);
	vao = getUINT(VAO);
	shader = getUINT(SHADER);
	neutron::modes::draw = getUINT(DRAW);

	mult_color = getF4(MULT_COLOR);
	add_color = getF4(ADD_COLOR);

	glyph_clip = getF(GLYPH_CLIP);
	glyph_soft_edge = getF(GLYPH_SOFT_EDGE);

	neutron::modes::gui = getB(GUI);
	neutron::modes::alpha_test = getB(ALPHA_TEST);
	neutron::passes::rotation = this->rotation;
	face_camera = getB(FACE_CAMERA);
	transform = neutron::processor::form(position, size, this->rotation);
	uvTransform = getM4(TEXTURE_TRANSFORM);
}

void Quark::render() {
	set();
	draw(getUINT(TEXTURE), getF4(COLOR), layer, vertCount);
	set();
	drawText(getS(Quark::TEXT), getS(Quark::FONT).c_str(), getUINT(TEXTURE), getF4(COLOR), layer, vertCount);
}

int sti(std::string in) {
	if (in == "")
		return 1;
	else return std::stoi(in);
}

void printFa(float a[], int size = 9) {
	std::cout << "{";
	std::cout << "Size: " << size << ", ";
	for (int i = 0; i < size-1; i++) {
		std::cout << *(a+i) << ", ";
	}
	std::cout << a[size - 1] << "}" << std::endl;
}

void Quark::loadOBJ(std::string filepath) {
	std::vector<glm::vec4> positions = std::vector<glm::vec4>();
	std::vector<glm::vec3> normal = std::vector<glm::vec3>();
	std::vector<glm::vec2> uv = std::vector<glm::vec2>();
	std::vector<glm::mat3> faces = std::vector<glm::mat3>();

	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss[2];

	std::vector<std::vector<float>> data = std::vector<std::vector<float>>();

	std::cout << "Loading object     " << filepath << std::endl;

	while (std::getline(stream, line)) {
		if (line.find("vn ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			normal.push_back(glm::vec3(std::stod(values[1]), std::stod(values[2]), std::stod(values[3])));
		}
		else if (line.find("vt ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			uv.push_back(glm::vec2(std::stod(values[1]), std::stod(values[2])));
		}
		else if (line.find("v ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			positions.push_back(glm::vec4(std::stod(values[1]), std::stod(values[2]), std::stod(values[3]), 1));
		}
		else if (line.find("f ") != std::string::npos) {
			std::vector<std::string> vertices = neutron::processor::split(line, ' ');
			std::vector<std::string> v1 = neutron::processor::split(vertices[1], '/');
			std::vector<std::string> v2 = neutron::processor::split(vertices[2], '/');
			std::vector<std::string> v3 = neutron::processor::split(vertices[3], '/');
			faces.push_back(glm::mat3(
				glm::vec3(sti(v1[0]), sti(v1[1]), sti(v1[2])), 
				glm::vec3(sti(v2[0]), sti(v2[1]), sti(v2[2])), 
				glm::vec3(sti(v3[0]), sti(v3[1]), sti(v3[2]))
			));
		}
	}

	std::vector<unsigned int> ind = std::vector<unsigned int>();
	std::cout << "Optimizing indices " << filepath << std::endl;

	for (int i = 0; i < faces.size(); i++)
		for (short j = 0; j < 3; j++) {
			glm::vec4 pos = positions[faces[i][j].x - 1];
			glm::vec3 norm = glm::vec3(0, 0, -1);
			if (faces[i][j].y <= normal.size()) norm = normal[faces[i][j].y - 1];
			glm::vec2 tex = glm::vec2(0);
			if (faces[i][j].z <= uv.size()) tex = uv[faces[i][j].z - 1];
			std::vector<float> con = { pos.x, pos.y, pos.z, pos.w, norm.x, norm.y, norm.z, tex.x, tex.y };

			int found = -1;
			for (int h = 0; h < data.size(); h++) {
				if (data[h] == con) {
					found = h;
					break;
				}
			}
			if (found == -1) {
				ind.push_back(data.size());
				data.push_back(con);
			}
			else
				ind.push_back(found);
		}

	float *pos = new float[data.size()*9];
	for (int i = 0; i < data.size(); i++)
		for (int j = 0; j < data[i].size(); j++)
			pos[i * 9 + j] = data[i][j];
	unsigned int vao = neutron::processor::VAO();
	unsigned int vbo = neutron::processor::VBO();
	unsigned int ebo = neutron::processor::EBO();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size()*9*sizeof(float), pos, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size()*sizeof(unsigned int), &ind[0], GL_STATIC_DRAW);

	std::cout << ind.size() << std::endl;

	processor::attribute(0, 4, 9, 0);
	processor::attribute(1, 2, 9, 7);
	processor::attribute(2, 3, 9, 4);

	setVao(vao, ind.size());
}

void Quark::writeQuarkFromOBJ(std::string objpath) {
	std::vector<glm::vec4> positions = std::vector<glm::vec4>();
	std::vector<glm::vec3> normal = std::vector<glm::vec3>();
	std::vector<glm::vec2> uv = std::vector<glm::vec2>();
	std::vector<glm::mat3> faces = std::vector<glm::mat3>();

	std::ifstream stream(objpath);

	std::string line;
	std::stringstream ss[2];

	std::vector<std::vector<float>> data = std::vector<std::vector<float>>();

	std::cout << "Loading object     " << objpath << std::endl;

	while (std::getline(stream, line)) {
		if (line.find("vn ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			normal.push_back(glm::vec3(std::stod(values[1]), std::stod(values[2]), std::stod(values[3])));
		}
		else if (line.find("vt ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			uv.push_back(glm::vec2(std::stod(values[1]), std::stod(values[2])));
		}
		else if (line.find("v ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			positions.push_back(glm::vec4(std::stod(values[1]), std::stod(values[2]), std::stod(values[3]), 1));
		}
		else if (line.find("f ") != std::string::npos) {
			std::vector<std::string> vertices = neutron::processor::split(line, ' ');
			std::vector<std::string> v1 = neutron::processor::split(vertices[1], '/');
			std::vector<std::string> v2 = neutron::processor::split(vertices[2], '/');
			std::vector<std::string> v3 = neutron::processor::split(vertices[3], '/');
			faces.push_back(glm::mat3(
				glm::vec3(sti(v1[0]), sti(v1[1]), sti(v1[2])),
				glm::vec3(sti(v2[0]), sti(v2[1]), sti(v2[2])),
				glm::vec3(sti(v3[0]), sti(v3[1]), sti(v3[2]))
			));
		}
	}

	std::vector<unsigned int> ind = std::vector<unsigned int>();
	std::cout << "Optimizing indices " << objpath << std::endl;

	for (int i = 0; i < faces.size(); i++)
		for (short j = 0; j < 3; j++) {
			glm::vec4 pos = positions[faces[i][j].x - 1];
			glm::vec3 norm = glm::vec3(0, 0, -1);
			if (faces[i][j].z <= normal.size()) norm = normal[faces[i][j].z - 1];
			glm::vec2 tex = glm::vec2(0);
			if (faces[i][j].y <= uv.size()) tex = uv[faces[i][j].y - 1];
			std::vector<float> con = { pos.x, pos.y, pos.z, pos.w, norm.x, norm.y, norm.z, tex.x, tex.y };

			int found = -1;
			for (int h = 0; h < data.size(); h++) {
				if (data[h] == con) {
					found = h;
					break;
				}
			}
			if (found == -1) {
				ind.push_back(data.size());
				data.push_back(con);
			}
			else
				ind.push_back(found);
		}
	
	byte* out = new byte[15+data.size()*36+ind.size()*4];

	out[0] = ((data.size()) & 0xff'00'00'00) >> 24; // Number of points f000
	out[1] = ((data.size()) & 0x00'ff'00'00) >> 16; // Number of points 0f00
	out[2] = ((data.size()) & 0x00'00'ff'00) >> 8;  // Number of points 00f0
	out[3] = ((data.size()) & 0x00'00'00'ff) >> 0;  // Number of points 000f

	out[4] = 3; // 3 elements: pos, norm, uv
	out[5] = 4; // 4 floats in first element
	out[6] = 0; // Id 0 in first element
	out[7] = 3; // 2 floats in second element
	out[8] = 2; // Id 2 in second element
	out[9] = 2; // 3 floats in third element
	out[10] = 1; // Id 1 in third element
		
	union u_float {
		float f;
		char data[sizeof(float)];
	};

	for (int i = 0; i < data.size(); i++)
		for (int j = 0; j < data[i].size(); j++) {
			u_float f;
			f.f = data[i][j];
			out[i * 36 + j * 4 + 11] = f.data[3];
			out[i * 36 + j * 4 + 12] = f.data[2];
			out[i * 36 + j * 4 + 13] = f.data[1];
			out[i * 36 + j * 4 + 14] = f.data[0];
		}

	out[data.size() * 36 + 11] = ((ind.size() / 3) & 0xff'00'00'00) >> 24; // Number of indices f000
	out[data.size() * 36 + 12] = ((ind.size() / 3) & 0x00'ff'00'00) >> 16; // Number of indices 0f00
	out[data.size() * 36 + 13] = ((ind.size() / 3) & 0x00'00'ff'00) >> 8;  // Number of indices 00f0
	out[data.size() * 36 + 14] = ((ind.size() / 3) & 0x00'00'00'ff) >> 0;  // Number of indices 000f
	for (int i = 0; i < ind.size(); i++) {
		out[data.size() * 36 + 15 + i * 4] = (ind[i] & 0xff'00'00'00) >> 24;
		out[data.size() * 36 + 16 + i * 4] = (ind[i] & 0x00'ff'00'00) >> 16;
		out[data.size() * 36 + 17 + i * 4] = (ind[i] & 0x00'00'ff'00) >> 8;
		out[data.size() * 36 + 18 + i * 4] = (ind[i] & 0x00'00'00'ff) >> 0;
		std::cout << i << (i < ind.size() - 1 ? ", " : "") << std::endl;
	}

	writeBinary((objpath.replace(objpath.end() - 4, objpath.end(), ".qrk")).c_str(), out, 15+data.size()*36+ind.size()*4);

}

void Quark::loadQuark(std::string filepath) {
	std::vector<byte> data = readAsBytes(filepath.c_str());

	unsigned int vertNum = *(unsigned int*)new byte[4]{ data[3], data[2], data[1], data[0] };
	int elementNum = (int)data[4];
	int *elementLengths = new int[elementNum];
	int *elementIds = new int[elementNum];
	int vertexSize = 0;

	for (int i = 0; i < elementNum; i++) {
		vertexSize += data[5 + i * 2];
		elementLengths[i] = data[5 + i * 2];
		elementIds[i] = data[6 + i * 2];
	}

	float *vertices = new float[vertexSize*vertNum];
	for (int i = 0; i < vertNum*vertexSize; i++)
		vertices[i] = *(float*) new byte[4]{ data[8 + elementNum * 2 + i * 4], data[7 + elementNum * 2 + i * 4], data[6 + elementNum * 2 + i * 4], data[5 + elementNum * 2 + i * 4] };
	
	int offset = 5 + elementNum * 2 + vertNum * vertexSize * 4;
	unsigned int indiceNum = *(unsigned int*) new byte[4]{data[offset+3], data[offset+2], data[offset+1], data[offset]};
	unsigned int *indices = new unsigned int[indiceNum * 3];

	for (int i = 0; i < indiceNum * 3; i++)
		indices[i] = *(unsigned int*) new byte[4]{ data[offset + 7 + i * 4], data[offset + 6 + i * 4], data[offset + 5 + i * 4], data[offset + 4 + i * 4] };

	unsigned int vao = processor::VAO();
	unsigned int vbo = processor::VBO();
	unsigned int ebo = processor::EBO();

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER_BINDING, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertNum * vertexSize * 4, &(vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceNum * 3 * 4, &(indices[0]), GL_STATIC_DRAW);

	int arVert = 0;
	for (int i = 0; i < elementNum; i++) {
		processor::attribute(elementIds[i], elementLengths[i], vertexSize, arVert);
		arVert += elementLengths[i];
	}
	
	glBindVertexArray(0);

	setVao(vao, indiceNum*3);

	delete[] vertices;
	delete[] indices;
	delete[] elementIds;
	delete[] elementLengths;

}