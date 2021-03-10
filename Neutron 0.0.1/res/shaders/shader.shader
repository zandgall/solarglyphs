#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 uv;
in vec3 _normal;

out vec2 TexCoord;
out vec3 normal;
out vec4 zoom;

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
	normal = _normal;
	mat4 toFrag = (ortho*view*model);
	zoom = vec4(toFrag[0].x, toFrag[1].y, toFrag[2].z, 1);
}

#shader fragment
#version 400 core

in vec2 TexCoord;
in vec3 normal;
in vec4 zoom;

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
uniform bool useNearestMask;
uniform bool textAnaglyph;
uniform sampler2D nearestMask;
uniform float glyphClip;
uniform float softEdge;

uniform vec4 colMult;
uniform vec4 colAdd;
uniform bool useAddTexture;
uniform sampler2D addTex;
uniform bool useMultTexture;
uniform sampler2D multTex;

uniform bool alphaTest;
uniform bool antiAlias;

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

float glyph(vec2 coord) {
	float alpha = textureBi(distance, coord).r;
	if (useNearestMask && texture2D(nearestMask, coord).r == 0)
		discard;
	if (useNearestMask && texture2D(nearestMask, coord).r == 1)
		alpha = 10;
	return smoothstep(glyphClip - softEdge, glyphClip + softEdge, alpha);
}

void main() {
	vec2 coord = (vec4(TexCoord, 0, 1) * uvTransform).xy;
	glColor = texture2D(diffuse, coord)*colMult+colAdd;
	if (useCol)
		glColor = inCol;
	if (useMask)
		glColor.w *= texture2D(mask, coord).r;

	if (isGlyph) {
		if (antiAlias) {
			vec2 c0 = coord - (pixel * vec2(0.4, 0.1));
			vec2 c1 = coord + (pixel * vec2(0.4, 0.1));
			vec2 c2 = coord - (pixel * vec2(0.1, 0.4));
			vec2 c3 = coord + (pixel * vec2(0.1, 0.4));
			glColor.w *= (glyph(c0) + glyph(c1) + glyph(c2) + glyph(c3)) / 4;
		}
		else {
			glColor.w *= glyph(coord);
		}
		
		if (textAnaglyph) {
			vec3 pre;
			vec3 textAnaglyph;
			pre.r = glyph(coord - pixel * vec2(0.5, 0.2));
			pre.b = glyph(coord + pixel * vec2(0.5, 0.2));
			pre.g = glyph(coord + pixel * vec2(0.3, 0.3));
			textAnaglyph.r = pre.r * glColor.g + pre.b * (1 - glColor.g);
			textAnaglyph.b = pre.b * glColor.g + pre.r * (1 - glColor.g);
			textAnaglyph.g = pre.g;
			float fac = (textAnaglyph.r + textAnaglyph.b)/2;
			glColor.xyz = glColor.xyz*fac + textAnaglyph * (1 - fac);
		}
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
	if (glColor.a == 0)
		discard;
}