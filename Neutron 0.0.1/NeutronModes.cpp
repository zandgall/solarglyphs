#include "Neutron.h"
namespace neutron {
	namespace modes {
		// draw = changes how draw() should handle it's parameters : draw_def = what draw will be on reset
		unsigned int draw = 0, draw_def = 0;

		// Draw in a GUI mode or not, removing perspective from what you're going to draw
		bool gui = false, gui_def = false;

		// Test alpha whether to discard under values of 0.8 or to rely on sorted blending. Disabling works mostly with gui : def is default on neutron::reset();
		bool alpha_test = true, alpha_test_def = true;

		// Whether to anti-alias or not
		bool anti_alias = true, anti_alias_def = true;

		// Flipping options
		bool flip_vertically = false, flip_horizontally = false;

		// Text options
		bool text_equispace = false, text_equispace_def = false;

		// Set everything to the default values after drawing?
		bool reset_on_draw = false;

		// Whether or not Neutron should post process
		bool post_processing = false;
		namespace post {
			// The Framebuffer that will be modified and pushed to the screen when done rendering
			unsigned int screen = 0;
			// Multisampling framebuffer for antialiasing
			unsigned int msScreen = 0;
			// Buffer used for pre-post processing effects
			unsigned int bloomFBO = 0;
			// Multisampling render-buffer object
			unsigned int msRBO = 0;
			// The Textures assigned with the screen Framebuffers
			unsigned int screenTex = 0, multisampleTex = 0, bloomTex = 0;
			// A type of post processing
			float vignette = 0.5, bloom = 1.2, bloomIterations = 10, bloomThreshold = 1, gamma = 1;
			// Post processing variable
			glm::vec4 vignetteColor(0.2, 0.2, 0.2, 1), hsvAdd(0), hsvMult(1), rgbAdd(0), rgbMult(1);
			// Color selection and correction, where the first layer is the color you'd want to change, the second layer is the tolerance of the color selection, the third layer is the value you want to multiply by, and the fourth layer is the layer to add by
			std::vector<glm::mat4x3> hsvCorrection = std::vector<glm::mat4x3>(), rgbCorrection = std::vector<glm::mat4x3>();

			// Local

			unsigned int blurShader = 0, clipShader = 0;

			void set() {
				
				glGenFramebuffers(1, &msScreen);
				glBindFramebuffer(GL_FRAMEBUFFER, msScreen);

				glGenTextures(1, &multisampleTex);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTex);
				
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, frame::width, frame::height, GL_TRUE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleTex, 0);

				glCheckError();

				glGenRenderbuffers(1, &msRBO);
				glBindRenderbuffer(GL_RENDERBUFFER, msRBO);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, frame::width, frame::height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRBO);

				glCheckError();

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "Multi-Sampler Framebuffer is not complete!" << std::endl;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);


				glGenFramebuffers(1, &screen);
				glBindFramebuffer(GL_FRAMEBUFFER, screen);	

				glGenTextures(1, &screenTex);
				glBindTexture(GL_TEXTURE_2D, screenTex);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame::width, frame::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex, 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glCheckError();

				glBindTexture(GL_TEXTURE_2D, 0);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "Regular Framebuffer is not complete!" << std::endl;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glGenFramebuffers(1, &bloomFBO);
				glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);

				glGenTextures(1, &bloomTex);
				glBindTexture(GL_TEXTURE_2D, bloomTex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame::width, frame::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTex, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "Bloom Framebuffer is not complete!" << std::endl;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glCheckError();

				// Set shaders
				blurShader = processor::LoadShader("res/shaders/post/blur.shader");
				clipShader = processor::LoadShader("res/shaders/post/BLOOMclip.shader");
			}

			void resetScreenSize(int width, int height) {

				glBindTexture(GL_TEXTURE_2D, screenTex);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

				glBindTexture(GL_TEXTURE_2D, bloomTex);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

				glBindTexture(GL_TEXTURE_2D, 0);

				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTex);

				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);

				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

				glBindRenderbuffer(GL_RENDERBUFFER, msRBO);

				glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);

				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}

			void bindPost() {
				glBindFramebuffer(GL_FRAMEBUFFER, msScreen);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleTex, 0);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRBO);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
			}

			void pushToScreen() {
				using namespace processor;
				unsigned int vao = passes::VAO_FLAT;
	
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glm::ivec2 size = sizeofTexture(screenTex, GL_TEXTURE_2D);

				glBindFramebuffer(GL_READ_FRAMEBUFFER, msScreen);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screen);
				glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
				
				glBindVertexArray(vao);

				if (hsvCorrection.size() > 0) {
					glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screenTex, 0);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);

					if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
						std::cout << "Bloom FBO is not complete in HSV correction phase!" << std::endl;

					glUseProgram(passes::SHADER_COLOR_CORRECTION);
					uniB(passes::SHADER_COLOR_CORRECTION, "hsv", true);
					uniSampler(passes::SHADER_COLOR_CORRECTION, "image", screenTex, 0);
					uniI1(passes::SHADER_COLOR_CORRECTION, "length", hsvCorrection.size());
					for (int i = 0; i < hsvCorrection.size() && i < 32; i++) {
						int l = glGetUniformLocation(passes::SHADER_COLOR_CORRECTION, ("corrections["+std::to_string(i)+"]").c_str());
						glUniformMatrix4x3fv(l, 1, GL_FALSE, glm::value_ptr(hsvCorrection[i]));
					}
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
					glCheckError();
				}

				if (rgbCorrection.size() > 0) {
					glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screenTex, 0);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);

					if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
						std::cout << "Bloom FBO is not complete in HSV correction phase!" << std::endl;

					glUseProgram(passes::SHADER_COLOR_CORRECTION);
					uniB(passes::SHADER_COLOR_CORRECTION, "hsv", false);
					uniSampler(passes::SHADER_COLOR_CORRECTION, "image", screenTex, 0);
					uniI1(passes::SHADER_COLOR_CORRECTION, "length", rgbCorrection.size());
					for (int i = 0; i < rgbCorrection.size() && i < 32; i++) {
						int l = glGetUniformLocation(passes::SHADER_COLOR_CORRECTION, ("corrections[" + std::to_string(i) + "]").c_str());
						glUniformMatrix4x3fv(l, 1, GL_FALSE, glm::value_ptr(rgbCorrection[i]));
					}
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
					glCheckError();
				}

				if (bloom > 0) {
					glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
					glBindTexture(GL_TEXTURE_2D, bloomTex);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
					glBindTexture(GL_TEXTURE_2D, 0);

					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, bloomTex, 0);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);

					if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
						std::cout << "Bloom FBO is not complete in bloom phase!" << std::endl;

					glUseProgram(clipShader);
					uniF1(clipShader, "clipAt", bloomThreshold);
					uniSampler(clipShader, "image", screenTex, 0);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
					glCheckError();

					glUseProgram(blurShader);
					uniSampler(blurShader, "image", bloomTex, 0);
					uniF1(blurShader, "bloomAmount", bloom/10);

					uniB(blurShader, "horizontal", true);
					for(int i = 0; i < bloomIterations; i++)
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
					uniB(blurShader, "horizontal", false);
					for (int i = 0; i < bloomIterations; i++)
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				unsigned int s = passes::SHADER_POST;
				glUseProgram(s);
				uniSampler(s, "screen", screenTex, 0);
				uniSampler(s, "bloom", bloomTex, 1);
				uniF1(s, "vignette", vignette);
				uniF1(s, "gamma", gamma);
				uniF4(s, "vignetteColor", vignetteColor);
				uniF4(s, "hsvAdd", hsvAdd);
				uniF4(s, "hsvMult", hsvMult);
				uniF4(s, "rgbAdd", rgbAdd);
				uniF4(s, "rgbMult", rgbMult);

				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				for (int i = 0; i < 1; i++) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}
		};

		void reset() {
			draw = draw_def;
			gui = gui_def;
			alpha_test = alpha_test_def;
			anti_alias = anti_alias_def;
			text_equispace = text_equispace_def;
		}
		void set() {
			post::set();
		}
	}
}