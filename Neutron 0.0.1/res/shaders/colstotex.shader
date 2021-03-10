#shader vertex
#version 400 core

layout(location = 0) in vec4 position;

out vec2 coord;

uniform vec4 offset;
uniform vec4 scale;

void main() {
	coord = position.zw;

	vec4 pos = offset+(position*scale);

	gl_Position = vec4(pos.xy, 0.0, 1.0);
}

#shader fragment
#version 400 core

in vec2 coord;

layout(location = 0) out vec4 glColor;

uniform vec4 inCol;
uniform sampler2D tex;

void main() {
	glColor = inCol;
	//glColor+=
	glColor = vec4(0, 0, 1, 1);
}