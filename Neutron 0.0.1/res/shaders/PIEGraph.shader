#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 uv;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 ortho;
uniform float layer;

void main() {
	TexCoord = uv;
	
	vec4 pos = (ortho*vec4((model*(position)).xy, 0, 0)) + vec4(-1, 1, 0, 0);
	gl_Position = pos;
	gl_Position.w = 1;
}

#shader fragment
#version 400 core

in vec2 TexCoord;

out vec4 glColor;

uniform int slices;
uniform float values[64];
uniform vec3 colors[64];
void main() {
	if (distance(TexCoord, vec2(0.5)) > 0.5)
		discard;
	
	float angle = atan(TexCoord.y-0.5, TexCoord.x-0.5);
	glColor = vec4(0, ((angle/3.14159265)+1)/2, 0, 1);
	for (int i = 0; i < slices&&i < 64; i++) {
		if (((angle / 3.14159265) + 1) / 2 <= values[i]) {
			glColor.xyz = colors[i];
			return;
		}
	}
}