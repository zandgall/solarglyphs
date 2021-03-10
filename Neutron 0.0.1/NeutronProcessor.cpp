#include "Neutron.h"

namespace neutron {
	namespace processor {

		std::map<const std::string, unsigned int> preLoad = std::map<const std::string, unsigned int>();

		// values
		unsigned int colstoimg = 0, dVAO = 0, /*Distance VAO*/ dVBO = 0, /*Distance VBO*/ distanceMap = 0 /*A bilinear texture of a dot in the center*/;

		std::vector<std::string> split(const std::string& s, char delimiter) {
			std::vector<std::string> tokens;
			std::string token;
			std::istringstream tokenStream(s);
			while (getline(tokenStream, token, delimiter)) {
				tokens.push_back(token);
			}
			return tokens;
		}

		ShaderProgramSource ParseShader(const std::string& filepath) {				

			std::ifstream stream(filepath);

			enum class ShaderType {
				NONE = -1, VERTEX = 0, FRAGMENT = 1
			};

			std::string line;
			std::stringstream ss[2];
			ShaderType type = ShaderType::NONE;
			int lineNum = 1;
			while (getline(stream, line)) {
				if (line.find("#shader") != std::string::npos) {
					if (line.find("vertex") != std::string::npos)
						type = ShaderType::VERTEX;
					else if (line.find("fragment") != std::string::npos)
						type = ShaderType::FRAGMENT;
					lineNum = 1;
				}
				else {
					//std::cout << lineNum << " : " << line << std::endl;
					lineNum++;
					ss[(int)type] << line << '\n';
				}
			}

			stream.clear();
			stream.close();
			
			return { ss[0].str(), ss[1].str() };

		}

		unsigned int CompileShader(unsigned int type, const std::string& source) {

			unsigned int id = glCreateShader(type);
			const char* src = source.c_str();
			glShaderSource(id, 1, &src, nullptr);
			glCompileShader(id);

			int result;
			glGetShaderiv(id, GL_COMPILE_STATUS, &result);

			if (result == GL_FALSE) {
				int length;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

				char* message = (char*)alloca(length * sizeof(char));

				glGetShaderInfoLog(id, length, &length, message);

				std::string line;
				std::istringstream stream(source);
				int lineNum = 0;
				while (std::getline(stream, line)) {
					std::cout << ++lineNum << ": " << line << std::endl;
				}

				std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
				std::cout << message << std::endl;
				glDeleteShader(id);
				return 0;
			}

			return id;
		}
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

			unsigned int program = glCreateProgram();

			unsigned int vs = 0;
			vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
			unsigned int fs = 0;
			fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

			glAttachShader(program, vs);
			glAttachShader(program, fs);
			glLinkProgram(program);
			glValidateProgram(program);

			glDeleteShader(vs);
			glDeleteShader(fs);

			return program;
		}
		unsigned int LoadShader(const std::string& filepath, bool cache) {
			if (preLoad.count("SHADER" + filepath) != 0 && cache)
				return preLoad["SHADER" + filepath];
			
			ShaderProgramSource source = ParseShader(filepath);
			if(cache)
				preLoad.insert(std::make_pair("SHADER" + filepath, CreateShader(source.VertexSource, source.FragmentSource)));
			return CreateShader(source.VertexSource, source.FragmentSource);
		}

		void uniM4(unsigned int shader, const GLchar* address, glm::mat4 matrix) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniformMatrix4fv(vertexColorLocation, 1, GL_FALSE, glm::value_ptr(matrix));
		}
		void uniF4(unsigned int shader, const GLchar* address, glm::vec4 vec) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform4f(vertexColorLocation, vec.x, vec.y, vec.z, vec.w);
		}
		void uniF3(unsigned int shader, const GLchar* address, glm::vec3 vec) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform3f(vertexColorLocation, vec.x, vec.y, vec.z);
		}
		void uniF2(unsigned int shader, const GLchar* address, glm::vec2 vec) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform2f(vertexColorLocation, vec.x, vec.y);
		}
		void uniF1(unsigned int shader, const GLchar* address, float a) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform1f(vertexColorLocation, a);
		}
		void uniF4(unsigned int shader, const GLchar* address, float a, float b, float c, float d) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform4f(vertexColorLocation, a, b, c, d);
		}
		void uniF3(unsigned int shader, const GLchar* address, float a, float b, float c) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform3f(vertexColorLocation, a, b, c);
		}
		void uniF2(unsigned int shader, const GLchar* address, float a, float b) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform2f(vertexColorLocation, a, b);
		}
		void uniI4(unsigned int shader, const GLchar* address, int a, int b, int c, int d) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform4i(vertexColorLocation, a, b, c, d);
		}
		void uniI3(unsigned int shader, const GLchar* address, int a, int b, int c) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform3i(vertexColorLocation, a, b, c);
		}
		void uniI2(unsigned int shader, const GLchar* address, int a, int b) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform2i(vertexColorLocation, a, b);
		}
		void uniI1(unsigned int shader, const GLchar* address, int a) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform1i(vertexColorLocation, a);
		}
		void uniB(unsigned int shader, const GLchar* address, bool a) {
			int vertexColorLocation = glGetUniformLocation(shader, address);
			glUniform1i(vertexColorLocation, a);
		}
		//Your shader ID, the name of the sampler in your shader, (i.e. "diffuseTexture"), the ID of your texture, the number you want to give to your texture, and a target to go allong with it (i.e. "0" and "GL_TEXTURE0")
		void uniSampler(unsigned int shader, const char* name, unsigned int texture, int shaderNumber) {

			uniI1(shader, name, shaderNumber);

			std::string use = "material.use";
			use += name;
			use.replace(12, 9, "");

			uniB(shader, use.c_str(), (texture != 0));

			glActiveTexture(GL_TEXTURE0 + shaderNumber);

			glBindTexture(GL_TEXTURE_2D, texture);

		}
		


		// Returns a translation matrix
		glm::mat4 translation(glm::vec3 pos) {
			return glm::translate(glm::mat4(1.0), pos);
		}
		// Returns a rotation matrix off of a given matrix
		glm::mat4 rotation(glm::mat4 mat, glm::vec3 rot) {
			mat = glm::rotate(mat, rot.x, glm::vec3(1.0, 0.0, 0.0));
			mat = glm::rotate(mat, rot.y, glm::vec3(0.0, 1.0, 0.0));
			mat = glm::rotate(mat, rot.z, glm::vec3(0.0, 0.0, 1.0));
			return mat;
		}
		// Returns a rotation matrix off of [1]
		glm::mat4 rotation(glm::vec3 rot) {
			glm::mat4 out = glm::mat4(1.0);
			out = glm::rotate(out, rot.x, glm::vec3(1.0, 0.0, 0.0));
			out = glm::rotate(out, rot.y, glm::vec3(0.0, 1.0, 0.0));
			out = glm::rotate(out, rot.z, glm::vec3(0.0, 0.0, 1.0));
			return out;
		}
		// Returns a scalar matrix
		glm::mat4 scaled(glm::vec3 scaling) {
			glm::mat4 out = glm::mat4(1.0);
			out = glm::scale(out, scaling);
			return out;
		}
		// Returns a positioned, rotated, and possibly scaled matrix off of given matrix, or [1]
		glm::mat4 form(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, glm::mat4 mat) {
			return glm::scale(rotation(glm::translate(mat, pos), rot), scale);
		}
		// Returns a transform function for a specified rectangle shape
		glm::mat4 rect(float x, float y, float w, float h, glm::vec3 rot, glm::mat4 mat) {
			return form(glm::vec3(x + w / 2.f, y + h / 2.f, 0), glm::vec3(w / 2.f, h / 2.f, 0), rot, mat);
		}


		// Gives an assigned VBO
		unsigned int VBO() {
			unsigned int VBO;									// Have an empty number,
			glGenBuffers(1, &VBO);								// Generate a VBO into that number,
			glBindBuffer(GL_ARRAY_BUFFER, VBO);					// Bind that number,
			//std::cout << "Created VBO " << VBO << std::endl;	// Scream if needed,
			return VBO;											// and then return that number
		}
		// Gives an assigned VBO but with vertices in it
		unsigned int VBO(float vertices[]) {
			unsigned int VBO;															// Take an empty number,
			glGenBuffers(1, &VBO);														// Generate a VBO into that number,
			glBindBuffer(GL_ARRAY_BUFFER, VBO);											// Bind it,
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // Put numbers in it,
			//std::cout << "Created VBO " << VBO << std::endl;							// Scream if needed,
			return VBO;																	// and then return it.
		}
		// Gives an assigned EBO
		unsigned int EBO() {
			unsigned int EBO;									// Have empty number
			glGenBuffers(1, &EBO);								// Generate EBO into it,
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);			// Bind it,
			//std::cout << "Created EBO " << EBO << std::endl;	// Scream if needed
			return EBO;											// and then return it
		}
		// Gives an assigned VAO
		unsigned int VAO() {
			unsigned int VAO;									// Have an empty number,
			glGenVertexArrays(1, &VAO);							// Take that number and generate a VAO into it,
			glBindVertexArray(VAO);								// Bind the number,
			//std::cout << "Created VAO " << VAO << std::endl;	// Scream if needed
			return VAO;											// and then return it.
		}
		// Attributes 'in' values for a shader
		void attribute(int order_in_shader, int length, int elements_per_vertex, int index_in_array) {

			glVertexAttribPointer(order_in_shader, length, GL_FLOAT, GL_FALSE, elements_per_vertex * sizeof(float), (void*)(index_in_array * sizeof(float)));	// Set the attributes
			glEnableVertexAttribArray(order_in_shader);																											// Enable the attributes

			//std::cout << "Attribute " << order_in_shader << " with " << length << " members, added at " << index_in_array << " out of " << elements_per_vertex << std::endl; // Scream if needed
		}
		// Attributes but with a direct index
		void attributedirect(int order_in_shader, int length, int elements_per_vertex, const void* index_in_array) {
			glVertexAttribPointer(order_in_shader, length, GL_FLOAT, GL_FALSE, elements_per_vertex * sizeof(float), (index_in_array));	// Set the attributes, but more directly
			glEnableVertexAttribArray(order_in_shader);																					// Enable the attributes

			//std::cout << "Attribute " << order_in_shader << " with " << length << " members, added at " << index_in_array << " out of " << elements_per_vertex << std::endl; // Scream if needed
		}



		// Loads a texture with a given interpolation, or bilinear default
		unsigned int getTextureFrom(const char* filepath, unsigned int interpol, unsigned int wrap, bool cache) {
			if (preLoad.count("IMG" + std::string(filepath)) != 0 && cache)
				return preLoad["IMG" + std::string(filepath)];
			unsigned int texture;						// Have empty number,
			glGenTextures(1, &texture);					// Gen texture id into that number,
			glBindTexture(GL_TEXTURE_2D, texture);		// Bind it

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);			// Repeat/clip/etc image horizontally
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);			// Repeat/clip/etc image vertically
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpol);	// Set interpolation as minimum			(COULD BE FASTER WITH NEAREST MINIMUM?)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpol);	// Set interpolation as maximum

			int width, height, nrChannels;					// Have empty width, height and channel numbers
			std::string string = std::string(filepath);		// Load the chars into a string
			int loadtype = STBI_rgb;						// Set loadtype default to RGB
			if (string.find(".png") != std::string::npos)	// If it's a png use RGBA									(COULD BE AN ISSUE WITH 24 BIT PNGs)
				loadtype = STBI_rgb_alpha;
			stbi_set_flip_vertically_on_load(true);
			unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, loadtype); // Load the texture and get width, height, and channel number
			if (data) {
				int type = GL_RGB;								// Set default type to RGB
				if (string.find(".png") != std::string::npos)	// Use RGBA if it's a png								(^)
					type = GL_RGBA;
				glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);	// Buffer data into the texture
				glGenerateMipmap(GL_TEXTURE_2D);														// Generate mipmap based on interpolation
			}
			else {
				std::cout << "Failed to load texture " << filepath << std::endl;	// Scream if needed
			}
			stbi_image_free(data);					// Empty data,
			std::cout << "Loaded texture " << filepath << " " << texture << std::endl; // Scream
			if(cache)
				preLoad.insert(std::make_pair("IMG"+std::string(filepath), texture));
			return texture;							// and return the texture
		}
		// Returns a texture from the given colors, size, and possibly interpolation, or nearest-neighbor default 
		unsigned int textureFromColors(std::vector<glm::vec4> colors, int width, int height, unsigned int interpol) {

			if (width == 0 || height == 0) // Check size to make sure it's a possible image
				return 0;

			if (colstoimg == 0) {
				ShaderProgramSource source = ParseShader("res/shaders/colorstexture.shader");	// Read colstotex shader into vertex and fragment sounrces
				colstoimg = CreateShader(source.VertexSource, source.FragmentSource);		// Create a shader and set it to colstoimg
			}


			unsigned int FramebufferName;						// Create frame buffer,
			glGenFramebuffers(1, &FramebufferName);				// gen frame buffer,
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);	// and then bind it


			unsigned int drawTex;					// Create texture,
			glGenTextures(1, &drawTex);				// gen texture,
			glBindTexture(GL_TEXTURE_2D, drawTex);  // and then bind it

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpol);		// Use set interpolation close up
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpol);		// ..and far away
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically


			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, drawTex, 0);	// Set a Color attachment to the frame buffer
			glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);						// Bind the framebuffer once more


			glUseProgram(colstoimg);					// Use the "colors to img" shader,
			glViewport(0, 0, width, height);			// Assign viewport to match the framebuffer size
			glClearColor(0.0, 0.0, 0.0, 0.0);			// Set background color to clear
			glClear(GL_COLOR_BUFFER_BIT);				// Use background color as background

			if (dVAO == 0) {
				dVAO = VAO();							// Create VAO
				dVBO = VBO();							// Create VBO

				GLfloat box[4][4] = {					// Create box from (-1, -1) to (1, 1) 
						{-1,  1, 0, 1},
						{ 1,  1, 1, 1},
						{-1, -1, 0, 0},
						{ 1, -1, 1, 0},
				};

				glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);	// Apply the box to the VBO and VAO
				attribute(0, 4, 4, 0);												// Set attributes so that it works with our shader
			}

			glBindVertexArray(dVAO);				// Bind VAO

			uniF4(colstoimg, "scale", glm::vec4(0.5, 0.5, 0, 0)); // pass size to the shader
			uniM4(colstoimg, "ortho", glm::ortho(0.f, (float)width, 0.f, (float)height, -1.f, 1.f));
			uniB(colstoimg, "useTex", false);

			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					float _x = ((x + 1) - width / 2.0) * (2.0 / width);			// Get x location to draw the pixel in GL
					float _y = (height - (y) * 2) * (1.0 / height);				// Get y location to draw the pixel in GL
					uniF4(colstoimg, "offset", glm::vec4(x - width / 2.f + 1, y - height / 2.f + 1, 0, 0));		// And Assign them as the location of the pixel

					//std::cout << "(" << x << ", " << y << ") : " << colors.at(x + y * width).x << ", " << colors.at(x + y * width).y << ", " << colors.at(x + y * width).z << ", " << colors.at(x + y * width).w << std::endl;

					uniF4(colstoimg, "inCol", colors.at(x + y * width));	// Set the color
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);					// and render
				}
			}

			glBindVertexArray(0);					// Unbind VAO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Unbind frame buffer
			return drawTex;							// *Victory jingle*
		}
		// Returns a texture from the given color
		unsigned int textureFromColor(glm::vec4 color) {
			if (colstoimg == 0) {
				ShaderProgramSource source = ParseShader("res/shaders/colorstexture.shader");	// Read colstotex shader into vertex and fragment sounrces
				colstoimg = CreateShader(source.VertexSource, source.FragmentSource);		// Create a shader and set it to colstoimg
			}


			unsigned int FramebufferName;						// Create frame buffer,
			glGenFramebuffers(1, &FramebufferName);				// gen frame buffer,
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);	// and then bind it


			unsigned int drawTex;					// Create texture,
			glGenTextures(1, &drawTex);				// gen texture,
			glBindTexture(GL_TEXTURE_2D, drawTex);  // and then bind it

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);		// Use set interpolation close up
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);		// ..and far away
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically


			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, drawTex, 0);	// Set a Color attachment to the frame buffer
			glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);						// Bind the framebuffer once more


			glUseProgram(colstoimg);					// Use the "colors to img" shader,
			glViewport(0, 0, 1, 1);			// Assign viewport to match the framebuffer size
			glClearColor(0.0, 0.0, 0.0, 0.0);			// Set background color to clear
			glClear(GL_COLOR_BUFFER_BIT);				// Use background color as background

			if (dVAO == 0) {
				dVAO = VAO();							// Create VAO
				dVBO = VBO();							// Create VBO

				GLfloat box[4][4] = {					// Create box from (-1, -1) to (1, 1) 
						{-1,  1, 0, 1},
						{ 1,  1, 1, 1},
						{-1, -1, 0, 0},
						{ 1, -1, 1, 0},
				};

				glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);	// Apply the box to the VBO and VAO
				attribute(0, 4, 4, 0);												// Set attributes so that it works with our shader
			}

			glBindVertexArray(dVAO);				// Bind VAO

			uniF4(colstoimg, "scale", glm::vec4(2, 2, 0, 0)); // pass size to the shader
			uniM4(colstoimg, "ortho", glm::ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f));
			uniB(colstoimg, "useTex", false);


			uniF4(colstoimg, "offset", glm::vec4(0, 0, 0, 0));		// And Assign them as the location of the pixel

			//std::cout << "(" << x << ", " << y << ") : " << colors.at(x + y * width).x << ", " << colors.at(x + y * width).y << ", " << colors.at(x + y * width).z << ", " << colors.at(x + y * width).w << std::endl;

			uniF4(colstoimg, "inCol", color);	// Set the color
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);					// and render

			glBindVertexArray(0);					// Unbind VAO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Unbind frame buffer
			return drawTex;							// *Victory jingle*
		}
		/// UNFINISHED Returns a set of colors from a filepath
		std::vector<glm::vec4> colorsFromTextureUNFINISHED(const char* filepath) {
			int width, height, nrChannels;														// Empty width, height, and channel values
			int loadtype = STBI_rgb;															// Set loadtype default to rgb
			if (((std::string) filepath).find(".png") != std::string::npos)						// Use rgba if it's a .png			(COULD BE PROBLEMATIC WITH 24BIT PNGs)
				loadtype = STBI_rgb_alpha;
			unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, loadtype);	// Load data as unsigned char list
			std::vector<glm::vec4> out = std::vector<glm::vec4>();
			return out;
		}
		// Returns a set of colors from a textureID
		std::vector<glm::vec4> colorsFromTexture(unsigned int texture) {
			glBindTexture(GL_TEXTURE_2D, texture);									// Bind the texture
			int width, height;														// Have empty width and height values
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);   // Get texture width parameter into (pointer) width
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height); // Get texture height parameter into (pointer) height

			float* pix = new float[width*height * 4]();									// Create empty pixels array with size of images pixels
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pix);					// Assign image pixels to pixels array

			std::vector<glm::vec4> out = std::vector<glm::vec4>();							// Create empty list of 4-float vectors
			for (int i = 0; i < width*height * 4; i += 4)										// i through pixel length,
				out.push_back(glm::vec4(pix[i], pix[i + 1], pix[i + 2], pix[i + 3]));		// push color from pixels[i] into out colors

			delete pix;																	// Delete pixels array
			glBindTexture(GL_TEXTURE_2D, 0);										// Unbind texture
			return out;																// Return colors
		}
		// Gets the size of a texture
		glm::ivec2 sizeofTexture(unsigned int texture, unsigned int textureType) {
			glBindTexture(textureType, texture);									// Bind the texture
			int width, height;														// Have an empty width and height,
			glGetTexLevelParameteriv(textureType, 0, GL_TEXTURE_WIDTH, &width);		// Get the width parameter into (pointer) width
			glGetTexLevelParameteriv(textureType, 0, GL_TEXTURE_HEIGHT, &height);	// Get the height parameter into (pointer) height
			glBindTexture(textureType, 0);											// Unbind texture for safety
			return glm::ivec2(width, height);										// Return cool new width and height values
		}
		// Returns a "distance field" for glyph rendering based on given colors, size, and margin
		unsigned int getDistanceField(std::vector<glm::vec4> colors, int width, int height, int marginX , int marginY) {

			if (width == 0 || height == 0) // Check size to make sure it's a possible image
				return 0;

			if (colstoimg == 0) {
				ShaderProgramSource source = ParseShader("res/shaders/colorstexture.shader");	// Read colstotex shader into vertex and fragment sounrces
				colstoimg = CreateShader(source.VertexSource, source.FragmentSource);			// Create a shader and set it to colstoimg
			}

			if (distanceMap == 0) {
				std::vector<glm::vec4> dTextCols = std::vector<glm::vec4>();	// Create list of colors
				for (int i = 0; i < 4; i++)										// Until coord (1,1),
					dTextCols.push_back(glm::vec4(0));							// Add color (0, 0, 0, 0)
				dTextCols.push_back(glm::vec4(0.5));							// Then at (1,1) add color (0.5, 0.5, 0.5, 0.5)
				for (int i = 0; i < 4; i++)										// For rest of image
					dTextCols.push_back(glm::vec4(0));							// Add color (0, 0, 0, 0)
				distanceMap = textureFromColors(dTextCols, 3, 3, GL_LINEAR);	// Convert to new image
			}

			glEnable(GL_BLEND);																			// Enable blending
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);											// Use correct alpha blending
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);	// Draw from back to front

			unsigned int FramebufferName;						// Create frame buffer,
			glGenFramebuffers(1, &FramebufferName);				// gen frame buffer,
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);	// and then bind it

			unsigned int drawTex;					// Create texture,
			glGenTextures(1, &drawTex);				// gen texture,
			glActiveTexture(GL_TEXTURE0);			// Make sure is using texture 0
			glBindTexture(GL_TEXTURE_2D, drawTex);  // and then bind it

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width + marginX * 2, height + marginY * 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use Linear filtering close up
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// ..and far away
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, drawTex, 0);	// Set a Color attachment to the frame buffer
			glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);						// Bind the framebuffer once more
			glBindTexture(GL_TEXTURE_2D, 0);

			glUseProgram(colstoimg);					// Use the "colors to img" shader,
			glViewport(0, 0, width + marginX * 2, height + marginY * 2);			// Assign viewport to match the framebuffer size
			glClearColor(0.0, 0.0, 0.0, 0.0);			// Set background color to clear
			glClear(GL_COLOR_BUFFER_BIT);				// Use background color as background

			if (dVAO == 0) {
				dVAO = VAO();							// Create VAO
				dVBO = VBO();							// Create VBO

				GLfloat box[4][4] = {					// Create box from (-1, -1) to (1, 1) 
						{-1,  1, 0, 1},
						{ 1,  1, 1, 1},
						{-1, -1, 0, 0},
						{ 1, -1, 1, 0},
				};

				glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);	// Apply the box to the VBO and VAO
				attribute(0, 4, 4, 0);												// Set attributes so that it works with our shader
			}

			glBindVertexArray(dVAO);	// Bind VAO	

			uniM4(colstoimg, "ortho", glm::ortho(0.f, (float)width + marginX * 2, 0.f, (float)height + marginY * 2, -1.f, 1.f));	// Be able to use pixel-coords
			uniB(colstoimg, "useTex", true);																						// Use a texture (distanceMap)
			glActiveTexture(GL_TEXTURE0);																							// Use texture #0
			glBindTexture(GL_TEXTURE_2D, distanceMap);																				// And set it to distanceMap

			for (int x = -1; x < width + 1; x++) {
				for (int y = -1; y < height + 1; y++) {
					uniF4(colstoimg, "offset", glm::vec4(x - width / 2.f + 1, y - height / 2.f + 1, 0, 0));		// Assign the location of the pixel

					glm::vec4 color = glm::vec4(0);												// Start with a clear color

					if (x < width&&y < height&&x >= 0 && y >= 0) {
						color = colors.at(x + y * width);										// And set to current pixel if inside texture
						float r = ((float)color.y)*20.f;
						uniF4(colstoimg, "scale", glm::vec4(r, r, 0.f, 0.f));	// Pass Spread of DistanceMap using the green channel as source
					}

					uniF4(colstoimg, "inCol", color);					// Set the color
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);				// and render
				}
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindVertexArray(0);					// Unbind VAO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Unbind frame buffer
			return drawTex;							// *Victory jingle*
		}
		// Gets a distance field from a textureID
		unsigned int getDistanceField(unsigned int texture) {
			return getDistanceField(colorsFromTexture(texture), sizeofTexture(texture).x, sizeofTexture(texture).y); // Return a distance field with the colors and size from given texture id
		}
		// Returns a distance field for a text specific glyph
		unsigned int getTextDistanceField(unsigned int TEXTure) {
			std::vector<glm::vec4> colors = colorsFromTexture(TEXTure);
			for (int i = 0; i < colors.size(); ++i) {
				colors.at(i).z = 1;			// Set distance field usage to 1
				colors.at(i).y = 0.2;		// Set distance field spread to 0.2
				colors.at(i).w = 1;			// And set overall alpha to 1
			}
			return getDistanceField(colors, sizeofTexture(TEXTure).x, sizeofTexture(TEXTure).y);
		}
		// Loads a cubemap from set of faces as filepaths
		unsigned int loadCubemap(std::vector<std::string> faces, bool cache) {
			if (preLoad.count("CUBE" + faces[0] + faces[1] + faces[2] + faces[3] + faces[4] + faces[5]) != 0 && cache)
				return preLoad["CUBE" + faces[0] + faces[1] + faces[2] + faces[3] + faces[4] + faces[5]];
			unsigned int textureID;							// have an empty cube-map texture id
			glGenTextures(1, &textureID);					// Generate a texture into that id
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);	// Bind it as a cube map

			int width, height, nrChannels;					// Have empty width, height and channel numbers
			for (unsigned int i = 0; i < faces.size(); i++) {
				std::string string = std::string(faces[i]);		// Load the chars into a string
				int loadtype = STBI_rgb;						// Set loadtype default to RGB
				if (string.find(".png") != std::string::npos)	// If it's a png use RGBA									(COULD BE AN ISSUE WITH 24 BIT PNGs)
					loadtype = STBI_rgb_alpha;
				unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, loadtype); // Load data and grab width, height, and channel number

				if (data) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	// Buffer image into selected face,
					stbi_image_free(data);																							// and free the data
				}
				else
				{
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;								// Scream if needed
					stbi_image_free(data);																							// Free the data anyways
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Use Nearest-neighbor interpolation minimum
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use linear interpolation maximum
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Clip texture x-wise
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// Clip texture y-wise
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);	// Clip texture z-wise

			if (cache)
				preLoad.insert(std::make_pair("CUBE" + faces[0] + faces[1] + faces[2] + faces[3] + faces[4] + faces[5], textureID));

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);			// Unbind the texture
			return textureID;								// and return it
		}
		/// UNDOCUMENTED: returns a cubemapid from a resolution
		std::vector<unsigned int> createCubemapID_UNDOCUMENTED(int GEN_RESOLUTION) {

			// Create framebuffer
			unsigned int framebuffer;
			glGenFramebuffers(1, &framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			// Generate texture
			unsigned int texColorBuffer;
			glGenTextures(1, &texColorBuffer);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GEN_RESOLUTION, GEN_RESOLUTION, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Attach it to currently bound framebuffer object
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

			// Create, bind, apply, and then unbind render buffer object
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GEN_RESOLUTION, GEN_RESOLUTION);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			// Apply depth and stencil
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			// Check for errors
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return std::vector<unsigned int> {
				framebuffer, texColorBuffer, rbo
			};
		}
		/// UNTESTED: Creates an empty cubemap with width and height
		unsigned int createEmptyCubemap(int width, int height) {
			unsigned int textureID;							// Have empty texture id
			glGenTextures(1, &textureID);					// Generate a texture and use that id
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);	// Bind the texture

			for (unsigned int i = 0; i < 6; i++) {																				// Through all the faces
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	// Generate an empty face
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Use nearest-neighbor interpolation minimum
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use bilinear interpolation maximum
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Clip texture x-wise
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// Clip texture y-wise
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);	// Clip texture z-wise

			return textureID;								// Return finished texture
		}
		/// UNTESTED: Binds a texture to the side of a cubemap
		unsigned int bindSideOfCubemap(std::vector<unsigned int> textures, unsigned int cubeTex) {

			unsigned int cubeFBO;						// Have empty cube frame-buffer id
			glGenFramebuffers(1, &cubeFBO);				// Generate into that id
			glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO);	// Bind it

			GLenum attachments[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3,
				GL_COLOR_ATTACHMENT4,
				GL_COLOR_ATTACHMENT5
			}; // Use a set of color attachments

			for (int i = 0; i < 6; i++) {																				// Through all the faces,
				glBindTexture(attachments[i], textures[i]);																// Bind the new texture,
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTex, 0);	// Bind it to the frame buffer
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);		// Unbind the frame buffer,
			return cubeFBO;								// and return it
		}


		// Screenspace coordinate to pixel coord
		glm::vec4 toPixelCoord(glm::vec4 screenspaceCoordinate) {
			glm::mat4 view = world::camera.GetViewMatrix();
			glm::mat4 ortho = glm::perspective(glm::radians(world::camera.Zoom), (float)frame::width / (float)frame::height, 0.05f, 100.0f);
			if (modes::gui)
				ortho = glm::ortho<float>(0, frame::width, frame::height, 0, -1, 1);
			glm::mat4 ntransform = passes::globalTransform;
			glm::mat4 fac = glm::mat4(
				glm::vec4(world::camera.Left, 0.f),
				glm::vec4(world::camera.Up, 0.f),
				glm::vec4(world::camera.Back, 0.f),
				glm::vec4(0, 0, 0, 1)
			);
			if (passes::face_camera)
				ntransform *= fac;

			glm::vec4 o = glm::vec4((view * (ntransform*screenspaceCoordinate)).xyz(), 1);
			//return glm::vec4(frame::width/4 * (o.x / (o.z) + 2), frame::height/4 * (4-(o.y/(o.z) + 2)), o.z, o.w);
			return glm::vec4(frame::width / 2 * (o.x * (frame::width/glm::radians(world::camera.Zoom))), frame::height / 2 + ((o.y * (frame::height / glm::radians(world::camera.Zoom)))), o.z, o.w);
		}
		glm::vec4 useMatrices(glm::vec4 position) {
			glm::mat4 view = world::camera.GetViewMatrix();
			glm::mat4 ortho = glm::perspective(glm::radians(world::camera.Zoom), (float)frame::width / (float)frame::height, world::clip_near, world::clip_far);
			if (modes::gui)
				ortho = glm::ortho<float>(0, frame::width, frame::height, 0, -1, 1);
			glm::mat4 fac = glm::mat4(
				glm::vec4(world::camera.Right, 0.f),
				glm::vec4(world::camera.Up, 0.f),
				glm::vec4(world::camera.Front, 0.f),
				glm::vec4(0.f, 0.f, 0.f, 1.f)
			);

			glm::mat4 transform = passes::globalTransform*passes::transform;
			if (passes::face_camera) {
				glm::mat4 mult = transform;
				mult[3] = glm::vec4(0, 0, 0, 1);
				glm::mat4 tran = glm::mat4(1);
				tran[3] = (transform)[3];
				transform = tran * fac*mult;
			}

			return ortho * glm::vec4((view* (transform * position)).xyz(), 1.0);
		}
		// Pixel coordinate to screenspace coord
		glm::vec4 toScreenCoord(glm::vec4 pixelCoordinate) {
			glm::mat4 view = world::camera.GetViewMatrix();
			glm::mat4 ortho = glm::perspective(glm::radians(world::camera.Zoom), (float)frame::width / (float)frame::height, 0.05f, 100.0f);
			glm::vec4 o = ortho / (view/passes::globalTransform/pixelCoordinate);
			return o;
		}
		// World transform to screenspace transform
		glm::mat4 toScreenTransform(glm::mat4 transform) {
			glm::mat4 view = world::camera.GetViewMatrix();
			glm::mat4 ortho;
			if (modes::gui)
				ortho = glm::ortho<float>(0, frame::width, frame::height, 0, -1, 1);
			else
				ortho = glm::perspective(glm::radians(world::camera.Zoom), (float)frame::width / (float)frame::height, 0.05f, 100.0f);
			// Local Position		
			glm::mat4 fac = glm::mat4(
				glm::vec4(world::camera.Left, 0.f),
				glm::vec4(world::camera.Up, 0.f),
				glm::vec4(world::camera.Back, 0.f),
				glm::vec4(0.f, 0.f, 0.f, 1.f)
			);

			glm::mat4 ntransform = passes::globalTransform*transform;
			if (passes::face_camera) {
				glm::mat4 mult = passes::globalTransform * transform;
				mult[3] = glm::vec4(0, 0, 0, 1);
				glm::mat4 tran = glm::mat4(1);
				tran[3] = (passes::globalTransform * transform)[3];
				ntransform = tran*fac*mult;
			}
			return ortho * view * ntransform;
		}

		// Maps a value from one range to another
		float mapTo(float in, float inMax, float inMin, float outMax, float outMin) {
			return (in - inMin) * ((outMax - outMin) / (inMax - inMin)) + outMin;
		}

		// Add a margin to a given texture
		unsigned int margin(unsigned int texture, unsigned int marginX, unsigned int marginY) {
			if (colstoimg == 0) {
				ShaderProgramSource source = ParseShader("res/shaders/colorstexture.shader");	// Read colstotex shader into vertex and fragment sounrces
				colstoimg = CreateShader(source.VertexSource, source.FragmentSource);			// Create a shader and set it to colstoimg
			}

			glm::vec2 size = sizeofTexture(texture);

			if (distanceMap == 0) {
				std::vector<glm::vec4> dTextCols = std::vector<glm::vec4>();	// Create list of colors
				for (int i = 0; i < 4; i++)										// Until coord (1,1),
					dTextCols.push_back(glm::vec4(0));							// Add color (0, 0, 0, 0)
				dTextCols.push_back(glm::vec4(0.5));							// Then at (1,1) add color (0.5, 0.5, 0.5, 0.5)
				for (int i = 0; i < 4; i++)										// For rest of image
					dTextCols.push_back(glm::vec4(0));							// Add color (0, 0, 0, 0)
				distanceMap = textureFromColors(dTextCols, 3, 3, GL_LINEAR);	// Convert to new image
			}

			glEnable(GL_BLEND);																			// Enable blending
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);											// Use correct alpha blending
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);	// Draw from back to front

			unsigned int FramebufferName;						// Create frame buffer,
			glGenFramebuffers(1, &FramebufferName);				// gen frame buffer,
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);	// and then bind it

			unsigned int drawTex;					// Create texture,
			glGenTextures(1, &drawTex);				// gen texture,
			glActiveTexture(GL_TEXTURE0);			// Make sure is using texture 0
			glBindTexture(GL_TEXTURE_2D, drawTex);  // and then bind it

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x + marginX * 2, size.y + marginY * 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use Linear filtering close up
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// ..and far away
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, drawTex, 0);	// Set a Color attachment to the frame buffer
			glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);						// Bind the framebuffer once more
			glBindTexture(GL_TEXTURE_2D, 0);

			glUseProgram(colstoimg);					// Use the "colors to img" shader,
			glViewport(0, 0, size.x + marginX * 2, size.y + marginY * 2);			// Assign viewport to match the framebuffer size
			glClearColor(0.0, 0.0, 0.0, 0.0);			// Set background color to clear
			glClear(GL_COLOR_BUFFER_BIT);				// Use background color as background

			if (dVAO == 0) {
				dVAO = VAO();							// Create VAO
				dVBO = VBO();							// Create VBO

				GLfloat box[4][4] = {					// Create box from (-1, -1) to (1, 1) 
						{-1,  1, 0, 1},
						{ 1,  1, 1, 1},
						{-1, -1, 0, 0},
						{ 1, -1, 1, 0},
				};

				glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);	// Apply the box to the VBO and VAO
				attribute(0, 4, 4, 0);												// Set attributes so that it works with our shader
			}

			glBindVertexArray(dVAO);	// Bind VAO	

			uniM4(colstoimg, "ortho", glm::ortho(0.f, (float)size.x + marginX * 2, 0.f, (float)size.y + marginY * 2, -1.f, 1.f));	// Be able to use pixel-coords
			uniB(colstoimg, "useTex", false);																						// Dont use a texture

			std::vector<glm::vec4> colors = colorsFromTexture(texture);

			for (int x = 0; x < size.x; x++) {
				for (int y = 0; y < size.y; y++) {
					uniF4(colstoimg, "offset", glm::vec4(x - size.x / 2.f + marginX, y - size.y / 2.f + marginY, 0, 0));// Assign the location of the pixel
					uniF4(colstoimg, "inCol", colors.at(x + y * size.x));												// Set the color
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);																// and render
				}
			}

			glBindVertexArray(0);					// Unbind VAO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Unbind frame buffer
			return drawTex;							// *Victory jingle*
		}

		// Creates a texture2D id for the given texture, with the given parameters, and RGBA. Then sets the drawing place to said texture2D
		void drawToTexture2D(unsigned int &texture, int width, int height, int interpolation, int colors) {
			if (width == 0 || height == 0) // Check size to make sure it's a possible image
				return;

			unsigned int FramebufferName;						// Create frame buffer,
			glGenFramebuffers(1, &FramebufferName);				// gen frame buffer,
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);	// and then bind it

			glGenTextures(1, &texture);				// gen texture,
			glBindTexture(GL_TEXTURE_2D, texture);  // and then bind it

			glTexImage2D(GL_TEXTURE_2D, 0, colors, width, height, 0, colors, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);	// Use set interpolation close up
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);	// ..and far away
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically


			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);	// Set a Color attachment to the frame buffer
			glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);						// Bind the framebuffer once more

			glViewport(0, 0, width, height);			// Assign viewport to match the framebuffer size
			glClearColor(0.0, 0.0, 0.0, 0.0);			// Set background color to clear
			glClear(GL_COLOR_BUFFER_BIT);				// Use background color as background

		}

		// Create a color correction matrix
		glm::mat4x3 getColorCorrection(glm::vec3 selectColor, glm::vec3 mult, glm::vec3 add, glm::vec3 tolerance) {
			return glm::mat4x3(selectColor, mult, add, tolerance);
		}
		// Modify a color correction matrix's contrast and brightness
		void changeColorCorrectionBrightness(glm::mat4x3 correction, float brightness, float contrast, bool hsv) {
			if (hsv) {
				correction[2].z *= contrast;
				correction[3].z += brightness;
			}
			else {
				correction[2] *= contrast;
				correction[3] += brightness;
			}
		}
		// Set a color correction matrix's contrast and brightness
		void setColorCorrectionBrightness(glm::mat4x3 correction, float brightness, float contrast, bool hsv) {
			if (hsv) {
				correction[2].z = contrast;
				correction[3].z = brightness;
			}
			else {
				correction[2] = glm::vec3(contrast);
				correction[3] = glm::vec3(brightness);
			}
		}
	};
}