#shader vertex
#version 400 core

layout(location = 0) in vec4 position;

out vec2 coord;

uniform vec4 offset;
uniform vec4 scale;
uniform mat4 ortho;

void main() {
	coord = position.zw;

	vec4 pos = ortho*(offset + (position*scale));

	gl_Position = vec4(pos.xy, 1.0, 1.0);
}

#shader fragment
#version 400 core

in vec2 coord;

layout(location = 0) out vec4 glColor;

uniform vec4 inCol;
uniform sampler2D tex;
uniform bool useTex;

void main() {
	if (useTex) {
		//glColor = vec4(inCol.x, 1, 1, (texture(tex, coord).a));
		glColor = vec4(inCol.r, 0, 0, inCol.w);
		//glColor.w = 1.0;
		glColor.w *= (1-inCol.b) + (inCol.b * inCol.r * texture(tex, coord).a);
		//glColor.y = 1.0;
	}
	else {
		glColor = inCol;
	}
}