#include "Neutron.h"";
bool hasBeenSet = false;

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_INVALID_INDEX:                 error = "INVALID_INDEX"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
namespace neutron {
	// Sets every value to it's given default, useful for customizing your default gamemode/drawmode : like a soft reset
	void reset() {
		modes::reset();
		passes::reset();
		world::reset();
	}

	// Sets every default value, required on startup : like a hard reset
	void set(std::string name, int width, int height) {
		frame::set(name, width, height);
		glCheckError();
		modes::set();
		glCheckError();
		passes::set();
		glCheckError();
		world::set();
		glCheckError();

		neutron::reset();
		glCheckError();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_MULTISAMPLE);

		hasBeenSet = true;
	}

	bool isSet() {
		return hasBeenSet;
	}

	int setUniforms(unsigned int texture, glm::vec4 color, float layer, unsigned int vertices) {
		using namespace passes;
		if (vertices == 0)
			vertices = ((vao == VAO_FLAT) ? 6 : (vao == VAO_CUBE) ? 36 : (vao == VAO_SPHERE) ? 192 : 0);
		unsigned int s = shader;
		using namespace processor;
		glUseProgram(s);
		// View from the camera
		uniM4(s, "view", world::camera.GetViewMatrix());
		// Perspective or ortho
		uniB(s, "ui", modes::gui);
		frame::width = max(frame::width, 1);
		frame::height = max(frame::height, 1);
		glm::mat4 ortho;
		if (modes::gui) {
			int viewSize[4];
			glGetIntegerv(GL_VIEWPORT, viewSize);
			ortho = glm::ortho<float>(modes::flip_horizontally ? viewSize[2] : 0, modes::flip_horizontally ? 0 : viewSize[2], modes::flip_vertically ? 0 : viewSize[3], modes::flip_vertically ? viewSize[3] : 0, -1, 1);
		}
		else
			ortho = glm::perspective(glm::radians(world::camera.Zoom), (float)frame::width/ (float)frame::height, world::clip_near, world::clip_far);
		uniM4(s, "ortho", ortho);
		// Local Position		

		glm::mat4 transform = passes::globalTransform*passes::transform;
		if (passes::face_camera && !modes::gui) {
			glm::mat4 fac = glm::mat4(
				glm::vec4(world::camera.Right, 0.f),
				glm::vec4(world::camera.Up, 0.f),
				glm::vec4(world::camera.Front, 0.f),
				glm::vec4(0.f, 0.f, 0.f, 1.f)
			);
			glm::mat4 mult = transform;
			mult[3] = glm::vec4(0, 0, 0, 1);
			glm::mat4 tran = glm::mat4(1);
			tran[3] = glm::vec4((transform)[3]);
			transform = tran*fac*mult;
		}
		uniM4(s, "model", transform);
		uniF1(s, "layer", layer);

		uniB(s, "useCol", texture == 0);
		uniF4(s, "inCol", color);

		uniF4(s, "colMult", passes::mult_color);
		uniF4(s, "colAdd", passes::add_color);

		uniB(s, "useMask", passes::mask_texture != 0);
		uniB(s, "isGlyph", modes::draw == modes::DRAW_GLYPH);
		uniB(s, "useAddTexture", passes::add_texture != 0);
		uniB(s, "useMultTexture", passes::mult_texture != 0);
		uniF1(s, "glyphClip", passes::glyph_clip);
		uniF1(s, "softEdge", passes::glyph_soft_edge);

		uniF2(s, "screen", glm::vec2(frame::width, frame::height));

		if (modes::anti_alias) {
			if (modes::gui) {
				glm::vec4 p = ortho * glm::vec4(((transform * glm::vec4(1, 1, 1, 1))).xy(), 0, 0) + glm::vec4(-1, 1, 0, 0);
				glm::vec4 m = ortho * glm::vec4(((transform * glm::vec4(-1, -1, -1, 1))).xy(), 0, 0) + glm::vec4(-1, 1, 0, 0);
				glm::vec2 pixel = glm::vec2(passes::glyph_anti_alias_spread) / (glm::vec2(0.5f*frame::width, 0.5f*frame::height) * glm::abs(p.xy() - m.xy()));
				uniF2(s, "pixel", pixel);
			}
			else {
				glm::vec4 p = ortho * glm::vec4((world::camera.GetViewMatrix() * (transform * glm::vec4(1, 1, 1, 1))).xy(), 0, 1);
				glm::vec2 pw = (ortho * glm::vec4((world::camera.GetViewMatrix() * (transform * glm::vec4(1, 1, 0, 1))).xyz(), 1)).ww();
				glm::vec4 m = ortho * glm::vec4((world::camera.GetViewMatrix() * (transform * glm::vec4(-1, -1, -1, 1))).xy(), 0, 1);
				glm::vec2 mw = (ortho * glm::vec4((world::camera.GetViewMatrix() * (transform * glm::vec4(-1, -1, 0, 1))).xyz(), 1)).ww();
				glm::vec2 pixel = glm::vec2(passes::glyph_anti_alias_spread) / (glm::vec2(0.5f*frame::width, 0.5f*frame::height) * glm::abs((p.xy() / pw) - (m.xy() / mw)));
				uniF2(s, "pixel", pixel);
			}
		}
		uniB(s, "alphaTest", modes::alpha_test);
		uniB(s, "antiAlias", modes::anti_alias);

		uniM4(s, "uvTransform", passes::uvTransform);

		// Textures
		uniI1(s, "diffuse", 0);
		uniI1(s, "mask", 1);
		uniI1(s, "distance", 2);
		uniI1(s, "nearestMask", 3);
		uniI1(s, "addTex", 4);
		uniI1(s, "multTex", 5);

		int CURRTEX = 6;
		for (std::map<std::string, unsigned int>::iterator i = passes::cust_texures.begin(); i != passes::cust_texures.end(); ++i) {
			uniI1(s, (i->first).c_str(), CURRTEX);
			glActiveTexture(GL_TEXTURE0 + CURRTEX);
			glBindTexture(GL_TEXTURE_2D, passes::cust_texures.at(i->first));
			CURRTEX++;
		}
		for (std::map<std::string, float>::iterator i = passes::cust_floats.begin(); i != passes::cust_floats.end(); ++i)
			uniF1(s, (i->first).c_str(), passes::cust_floats.at(i->first));
		for (std::map<std::string, glm::vec4>::iterator i = passes::cust_float4s.begin(); i != passes::cust_float4s.end(); ++i)
			uniF4(s, (i->first).c_str(), passes::cust_float4s.at(i->first));
		for (std::map<std::string, glm::vec3>::iterator i = passes::cust_float3s.begin(); i != passes::cust_float3s.end(); ++i)
			uniF3(s, (i->first).c_str(), passes::cust_float3s.at(i->first));
		for (std::map<std::string, glm::mat4>::iterator i = passes::cust_mat4s.begin(); i != passes::cust_mat4s.end(); ++i)
			uniM4(s, (i->first).c_str(), passes::cust_mat4s.at(i->first));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, passes::mask_texture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, passes::distance_texture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, passes::glyph_nearest_mask);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, passes::add_texture);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, passes::mult_texture);
		return CURRTEX;
	}

	void closeTextures(unsigned int number) {
		for (int i = 0; i < number; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void draw(unsigned int texture, glm::vec4 color, float layer, unsigned int vertices) {
		int CURRTEX = setUniforms(texture, color, layer, vertices);

		if (vertices == 0)
			vertices = ((passes::vao == passes::VAO_FLAT) ? 6 : (passes::vao == passes::VAO_CUBE) ? 36 : (passes::vao == passes::VAO_SPHERE) ? 192 : 0);

		glBindVertexArray(passes::vao);
		glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, 0);

		closeTextures(CURRTEX);

		if (modes::reset_on_draw)
			neutron::reset();
	}

	void drawInstances(std::vector<glm::mat4> instances, unsigned int texture, glm::vec4 color, float layer, unsigned int vertices) {
		int CURRTEX = setUniforms(texture, color, layer, vertices);

		processor::uniM4(passes::shader, "parent", passes::globalTransform * passes::transform);
		
		glBindVertexArray(passes::vao);
		glDrawElementsInstanced(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, 0, instances.size());
		
		closeTextures(CURRTEX);

		if (modes::reset_on_draw)
			neutron::reset();
	}	

	void drawText_FLAT(const std::string &text, const char* font, unsigned int texture, glm::vec4 color, float layer, unsigned int vertices, float xspacing, float yspacing) {
		float x = 0, y = 0;
		glm::vec2 dim = world::measureString(text, font);
		dim.x += 10;
		dim.y += 10 + world::sizeMap[font] * 0.3;
		glm::mat4 newTrans = passes::transform;
		newTrans[1].y = (dim.y / dim.x)*newTrans[0].x;
		if (newTrans[1].y > passes::transform[1].y) {
			newTrans[1].y = passes::transform[1].y;
			newTrans[0].x = (dim.x / dim.y)*newTrans[1].y;
		}
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++) {
			world::Character ch = world::characters[font][*c];

			glm::vec2 size = ch.size;
			if (modes::text_equispace)
				size = world::characters[font]['m'].size;

			glm::vec4 position;
			glm::vec4 off = glm::vec4(0);
			position = glm::vec4(2 * (x + ch.bearing.x + size.x / 2) / (dim.x - 10) - 1,
				2 * (y - ch.bearing.y + ch.size.y / 2 + (dim.y - 10 - world::sizeMap[font] * 0.3) / 2) / dim.y, 0, 0);

			glm::mat4 mod = processor::form(position, glm::vec3((ch.size.x) / dim.x, (ch.size.y) / dim.y, 0));
			glm::mat4 o = processor::form(off);

			processor::uniM4(passes::shader, "model", passes::globalTransform*o*newTrans*mod);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, ch.texture);

			glBindVertexArray(passes::vao);
			glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, 0);

			if (modes::text_equispace)
				ch = world::characters[font]['m'];
			x += (ch.advance.x / 64.f);
			y += (ch.advance.y > 0 ? (ch.advance.y >> 6) + yspacing : 0);
		}
	}

	std::string removeBlankCharacters(std::string text) {
		
		while (text.find('\n') != std::string::npos)
			text[text.find_first_of('\n')] = ' ';

		return text;
	}

	void drawText(const std::string &text, const char* font, unsigned int texture, glm::vec4 color, float layer, unsigned int vertices, float xspacing, float yspacing) {
		using namespace passes;
		if (vertices == 0)
			vertices = ((vao == VAO_FLAT) ? 6 : (vao == VAO_CUBE) ? 36 : (vao == VAO_SPHERE) ? 192 : 0);

		int CURRTEX = setUniforms(texture, color, layer, vertices);
		processor::uniB(shader, "textAnaglyph", true);

		drawText_FLAT(removeBlankCharacters(text), font, texture, color, layer, vertices, xspacing, yspacing);

		processor::uniB(shader, "textAnaglyph", false);

		closeTextures(CURRTEX);
		
		if (modes::reset_on_draw)
			neutron::reset();
	}

	void beforeLoop() {
		frame::checkInput();
		frame::beginFrame();
		if (modes::post_processing)
			modes::post::bindPost();
	}

	void afterLoop() {
		frame::swap();
		neutron::reset();
		if(modes::post_processing)
			modes::post::pushToScreen();
	}
}