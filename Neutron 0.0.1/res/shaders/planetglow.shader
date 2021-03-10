#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 uv;
in vec3 _normal;

out vec2 TexCoord;
out vec4 wPos;
out mat4 vie;

uniform mat4 model;
uniform mat4 view;
uniform mat4 ortho;
uniform float layer;
uniform bool ui;

void main() {
	TexCoord = uv;
	vec4 pos = ortho * vec4((view * (model * position)).xyz, 1.0);
	gl_Position.w = pos.w - (layer / 100.0);
	if (ui)
		gl_Position.w = 1;
	
	//gl_Position.w *= gl_Position.w;

	float w = 2*pow(gl_Position.w, 0.1);
	//w = 1;
	mat4 nmod = mat4(1);
	nmod[0].xyz = vec3(w, 0, 0);
	nmod[1].xyz = vec3(0, w, 0);
	nmod[2].xyz = vec3(0, 0, w);
	
	pos = ortho * vec4((view* (model * nmod * (position))).xy, 0.0, 1.0);
	if (ui)
		pos = (ortho*vec4((model*position).xy, 0, 0)) + vec4(-1, 1, 0, 0);
	
	gl_Position.xyz = pos.xyz;
	//*inverse(view)
	mat4 toFrag = (ortho*model);
	wPos = model * position;
	vie = inverse(view);
}

#shader fragment
#version 400 core

in vec2 TexCoord;
//in vec3 normal;
in vec4 wPos;
in mat4 vie;

layout(location = 0) out vec4 glColor;

uniform vec2 screen;
uniform vec2 pixel;

uniform sampler2D diffuse;
uniform mat4 uvTransform;

uniform bool useCol;
uniform vec4 inCol;

uniform bool useMask;
uniform sampler2D mask;

uniform bool isGlyph;
uniform sampler2D distance;
uniform float glyphClip;
uniform float softEdge;

uniform vec4 colMult;
uniform vec4 colAdd;
uniform bool useAddTexture;
uniform sampler2D addTex;
uniform bool useMultTexture;
uniform sampler2D multTex;

uniform bool alphaTest;

uniform sampler2D planet;

vec4 cubic(float v) {
	vec4 n = vec4(1, 2, 3, 4) - v;
	vec4 s = n * n * n;
	float x = s.x;
	float y = s.y - 4 * s.x;
	float z = s.z - 4.0 * s.y + 6.0 * s.x;
	float w = 6.0 - x - y - z;
	return vec4(x, y, z, w) * (1.0 / 6.0);
}

vec4 textureBi(sampler2D sampler, vec2 coords) {
	vec2 texSize = textureSize(sampler, 0);
	vec2 invTexSize = 1.0 / texSize;

	coords = coords * texSize - 0.5;

	vec2 fxy = fract(coords);
	coords -= fxy;

	vec4 xcubic = cubic(fxy.x);
	vec4 ycubic = cubic(fxy.y);

	vec4 c = coords.xxyy + vec2(-0.5, 1.5).xyxy;

	vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
	vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s;

	offset *= invTexSize.xxyy;

	vec4 sample0 = texture(sampler, offset.xz);
	vec4 sample1 = texture(sampler, offset.yz);
	vec4 sample2 = texture(sampler, offset.xw);
	vec4 sample3 = texture(sampler, offset.yw);

	float sx = s.x / (s.x + s.y);
	float sy = s.z / (s.z + s.w);

	return mix(
		mix(sample3, sample2, sx), mix(sample1, sample0, sx)
		, sy);
}

float glyph(vec2 coord, vec2 TexCoord) {
	float alpha = textureBi(distance, coord).r;
	return smoothstep(glyphClip - softEdge, glyphClip + softEdge, alpha) * smoothstep(glyphClip - softEdge, glyphClip + softEdge, textureBi(planet, TexCoord));
}

void main() {

	vec3 normal = normalize((vec4((TexCoord.x * 2 - 1) * (sqrt(pow(2 * TexCoord.y - 1, 2) + 1)*0.7), TexCoord.y * 2 - 1, -0.2, 0) * vie).xyz);
	//normal = normal * normalize(wPos-inverse(vie)[3].xyz);

	vec3 front = normalize((vec4(0, 0, -0.5, 0) * vie).xyz);
	float xAng = -atan(front.x, front.z) / 3.14159265;

	vec2 coord = (uvTransform * vec4(TexCoord, 0, 1)).xy + vec2(xAng * 0, 0);

	glColor = textureBi(diffuse, coord)*colMult + colAdd;
	if (useCol)
		glColor = inCol;
	if (useMask)
		glColor.w = texture2D(mask, coord).r;

	if (isGlyph) {
		float s = 0.01;
		vec2 size = pixel;
		vec2 c0 = coord - (size * vec2(0.4, 0.1));
		vec2 v0 = TexCoord - (size * vec2(0.4, 0.1));
		vec2 c1 = coord + (size * vec2(0.4, 0.1));
		vec2 v1 = TexCoord + (size * vec2(0.4, 0.1));
		vec2 c2 = coord - (size * vec2(0.1, 0.4));
		vec2 v2 = TexCoord - (size * vec2(0.1, 0.4));
		vec2 c3 = coord + (size * vec2(0.1, 0.4));
		vec2 v3 = TexCoord + (size * vec2(0.1, 0.4));
		float alpha = 2*(1-distance(coord, vec2(0.5)))*(1-distance(TexCoord, vec2(0.5)));
		alpha = 2*max(0.5-distance(TexCoord, vec2(0.5)), 0);
		alpha = floor(alpha * 5) / 5.0;
		glColor.w *= alpha;
	}

	if (useAddTexture)
		glColor += texture2D(addTex, coord);
	if (useMultTexture)
		glColor *= texture2D(multTex, coord);

	if (alphaTest) {
		if (glColor.a < 0.8)
			discard;
		glColor.a = 1;
	}
	if (glColor.a < 0.2)
		discard;
}