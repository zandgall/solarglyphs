#shader vertex
#version 400 core
layout(location = 0) in vec4 pos;
in vec2 uv;
in vec3 _normal;

out vec3 textureDir;

uniform mat4 ortho;
uniform mat4 view;

void main() {
	textureDir = pos.xyz;
	mat4 nview = view;
	nview[3].xyz *= 0.00001;
	vec4 npos = ortho * nview * pos;
	gl_Position = npos.xyww;
}

#shader fragment
#version 400 core
in vec3 textureDir;
uniform samplerCube cubemap;

out vec4 outcol;

void main() {
	outcol = texture(cubemap, textureDir);
}