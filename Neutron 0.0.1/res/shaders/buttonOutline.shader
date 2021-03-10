#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 uv;
in vec3 _normal;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 ortho;
uniform float layer;
uniform bool ui;

void main() {
	TexCoord = uv;
	vec4 pos = ortho * vec4((view* (model * position)).xy, 0.0, 1.0);
	if (ui)
		pos = (ortho*vec4((model*(position)).xy, 0, 0)) + vec4(-1, 1, 0, 0);
	gl_Position = pos;
	pos = ortho * vec4((view * (model * position)).xyz, 1.0);
	gl_Position.w = pos.w - (layer / 100.0);
	if (ui)
		gl_Position.w = 1;
}

#shader fragment
#version 400 core

in vec2 TexCoord;

layout(location = 0) out vec4 glColor;

uniform sampler2D diffuse;
uniform mat4 uvTransform;

uniform bool useCol;
uniform vec4 inCol;

uniform vec4 colMult;
uniform vec4 colAdd;

uniform vec4 bounds;
uniform float rounding;
uniform float bevel;

uniform bool alphaTest;

float cMix(float start, float end, float a) {
	return (max(min(a, end), start)-start)/(end-start)-0.5;
}

void main() {
	vec2 coord = (vec4(TexCoord, 0, 1) * uvTransform).xy;
	if(useCol)
		glColor = (inCol * colMult) + colAdd;

	float rX = (rounding / 2) / bounds.x;
	coord.x = ((cMix(0, rX, coord.x)) + (cMix(1 - rX, 1, coord.x)))/2 + 0.5;
	float rY = (rounding / 2) / bounds.y;
	coord.y = ((cMix(0, rY, coord.y)) + (cMix(1 - rY, 1, coord.y)))/2 + 0.5;

	if(!useCol)
		glColor = (inCol * texture2D(diffuse, coord) * colMult) + colAdd;
	else {
		float cx = (coord.x - 0.5)*2;
		float cy = (coord.y - 0.5)*2;
		glColor.w = 2*smoothstep(1.0, 1.0 - (10.0 / (bounds.x + bounds.y)), cx * cx + cy * cy) * (1-smoothstep(0.8, 0.8 - (10.0 / (bounds.x + bounds.y)), cx * cx + cy * cy));
	}

	if (alphaTest) {
		if (glColor.a < 0.8)
			discard;
		glColor.a = 1;
	}
}