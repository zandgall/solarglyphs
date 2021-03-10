#include "Neutron.h"
namespace neutron {
	namespace passes {
		// A preset VAO with a shape as described in the name
		unsigned int VAO_FLAT = 0, VAO_CUBE = 1, VAO_SPHERE = 2;
		// The VAO draw() should use to render : def is the default on neutron::reset()
		unsigned int vao = 0, vao_def = 0;

		// A preset shader that uses every parameter as expected
		unsigned int SHADER_FLAT = 0, SHADER_INSTANCE = 1, SHADER_BUTTON = 2, SHADER_POST = 3, SHADER_COLOR_CORRECTION = 4, SHADER_TEXT = 5, SHADER_SKYBOX = 6;
		// What shader Neutron should use : def is the default on neutron::reset();
		unsigned int shader = 0, shader_def = 0;

		/// COLOR PROCESSING
		// What mask texture to use, or use none with 0 : def is the default on neutron::reset();
		unsigned int mask_texture = 0, mask_texture_def = 0;
		// What multiply texture to use, or use none with 0 : def is the default on neutron::reset();
		unsigned int mult_texture = 0, mult_texture_def = 0;
		// What additive texture to use, or use none with 0 : def is the default on neutron::reset();
		unsigned int add_texture = 0, add_texture_def = 0;

		// A color to multiply to the result of draw()
		glm::vec4 mult_color = glm::vec4(1), mult_color_def = glm::vec4(1);
		// A color to add to the result of draw()
		glm::vec4 add_color = glm::vec4(0), add_color_def = glm::vec4(0);

		/// COORDINATES
		// A transformation matrix used to offset, rotate, or scale UV coordinates
		glm::mat4 uvTransform = glm::mat4(0), uvTransform_def = glm::mat4(0);

		/// CUSTOM
		// Custom unsigned ints
		std::map<std::string, unsigned int> cust_texures = std::map<std::string, unsigned int>(), def_cust_textures = std::map<std::string, unsigned int>();
		// Custom floats
		std::map<std::string, float> cust_floats = std::map<std::string, float>(), def_cust_floats = std::map<std::string, float>();
		// Custom float4s
		std::map<std::string, glm::vec4> cust_float4s = std::map<std::string, glm::vec4>(), def_cust_float4s = std::map<std::string, glm::vec4>();
		// Custom float3s
		std::map<std::string, glm::vec3> cust_float3s = std::map<std::string, glm::vec3>(), def_cust_float3s = std::map<std::string, glm::vec3>();
		// Custom mat4s
		std::map<std::string, glm::mat4> cust_mat4s = std::map<std::string, glm::mat4>(), def_cust_mat4s = std::map<std::string, glm::mat4>();

		/// GEOMETRY PROCESSING
		// Where the object is, and how it's scaled and rotated : def is the default on neutron::reset();
		glm::mat4 transform = glm::mat4(0), transform_def = glm::mat4(0);
		// To apply automatically to everything when drawing : def is the default on neutron::reset();
		glm::mat4 globalTransform = glm::mat4(0), globalTransform_def = glm::mat4(0);
		// Used to correct any drawing methods that require scalars from the transform matrix, e.i. text correction
		glm::vec3 rotation = glm::vec3(0);
		// Used for 2d elements in 3d space that always face the camera
		bool face_camera = false;

		/// GLYPH PROCESSING
		// What texture to use as a distance texture, or 0 for no texture at all. used in glyph rendering : def is the default on neutron::reset();
		unsigned int distance_texture = 0, distance_texture_def = 0;
		// Glyph nearest mask texture to create hard edges : def is the default on neutron::reset();
		unsigned int glyph_nearest_mask = 0, glyph_nearest_mask_def = 0;
		// What value the glyph distancefield clips at : def is the default on neutron::reset();
		float glyph_clip = 0, glyph_clip_def = 0;
		// How soft the edges will seem when drawing a glyph : def is the default on neutron::reset();
		float glyph_soft_edge = 0, glyph_soft_edge_def = 0;
		// The spread/blurriness of a glyph when anti-aliased
		float glyph_anti_alias_spread = 0, glyph_anti_alias_spread_def = 1;

		// Sets the default value of every passed variable : like a hard reset
		void set() {
			VAO_FLAT = processor::VAO();
			unsigned int vbo = processor::VBO();
			unsigned int ebo = processor::EBO();
			float pos[] = {
				-1.0,  1.0, 0.0, 1.0,  0.0, 0.0, -1.0,  0.0, 1.0,
				 1.0,  1.0, 0.0, 1.0,  0.0, 0.0, -1.0,  1.0, 1.0,
				-1.0, -1.0, 0.0, 1.0,  0.0, 0.0, -1.0,  0.0, 0.0,
				 1.0, -1.0, 0.0, 1.0,  0.0, 0.0, -1.0,  1.0, 0.0,
			};
			unsigned int ind[] = {
			0, 1, 2, 2, 1, 3,
			};
			glBindVertexArray(VAO_FLAT);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);

			processor::attribute(0, 4, 9, 0);
			processor::attribute(1, 2, 9, 7);
			processor::attribute(2, 3, 9, 4);

			///
			VAO_CUBE = processor::VAO();
			vbo = processor::VBO();
			ebo = processor::EBO();
			float posc[] = {
				-1, 1, -1, 1, 0, 1, 0, 0.333333, 1,
				 1, 1, 1, 1, 0, 0, 1, 0.333333, 1,
				 1, 1, -1, 1, -1, 0, 0, 0.333333, 1,
				 1, 1, 1, 1, 0, -1, 0, 0, 0.666667, -1,
				 -1, 1, 1, 1, 0, 0, 0, 0.666667, 1, -1, 1, 1, 0, 0, -1, 0, 0.666667, -1, 1, 1, 1, 0, 0, -1, 0.333333, 0.666667, -1, -1, -1, 1, 0, 0, -1, 0.333333, 0.666667, -1, -1, 1, 1, 0, 0, -1, 0.333333, 0.666667, 1, -1, -1, 1, 0, 0, -1, 0.333333, 0.333333, -1, -1, 1, 1, 0, 0, -1, 0.333333, 0.333333, -1, -1, -1, 1, 0, 0, -1, 0.333333, 0.333333, 1, 1, -1, 1, 0, 0, -1, 0.666667, 0.666667, 1, -1, 1, 1, 0, 0, -1, 0.666667, 0.666667, 1, -1, -1, 1, 0, 0, -1, 0.666667, 0.666667, -1, 1, -1, 1, 0, 0, -1, 0.333333, 0.666667, 1, -1, -1, 1, 0, 0, -1, 0.333333, 0.666667, -1, 1, 1, 1, 0, 0, -1, 0.333333, 1, -1, 1, 1, 1, 0, 0, -1, 0, 0.666667, 1, -1, 1, 1, 0, 0, -1, 0.333333, 0.333333, 1, 1, 1, 1, 0, 0, -1, 0.666667, 0.666667, 1, 1, -1, 1, 0, 0, -1, 0.333333, 0.666667,
			};

			unsigned int indc[] = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 7, 0, 17, 1, 3, 18, 4, 6, 15, 7, 9, 19, 10, 12, 20, 13, 15, 21, 16
			};

			glBindVertexArray(VAO_CUBE);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(posc), posc, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indc), indc, GL_STATIC_DRAW);

			processor::attribute(0, 4, 9, 0);
			processor::attribute(1, 2, 9, 7);
			processor::attribute(2, 3, 9, 4);
			///
			VAO_SPHERE = processor::VAO();
			vbo = processor::VBO();
			ebo = processor::EBO();

			float posData[] = {
				0, -1, 0, 1, 0.1876, -0.7947, 0.5774, 0.818181, 0,
				0.7236, -0.447215, 0.52572, 1, 0.6071, -0.7947, 0, 0.818181, 0,
				-0.276385, -0.447215, 0.85064, 1, -0.4911, -0.7947, 0.3568, 0.818181, 0,
				0.7236, -0.447215, 0.52572, 1, 0.6071, -0.7947, 0, 0.727272, 0.157461,
				0, -1, 0, 1, -0.4911, -0.7947, -0.3568, 0.727272, 0.157461,
				0.7236, -0.447215, -0.52572, 1, 0.1876, -0.7947, -0.5774, 0.727272, 0.157461,
				0, -1, 0, 1, 0.9822, -0.1876, 0, 0.90909, 0.157461,
				-0.276385, -0.447215, 0.85064, 1, 0.3035, -0.1876, 0.9342, 0.90909, 0.157461,
				-0.894425, -0.447215, 0, 1, -0.7946, -0.1876, 0.5774, 0.90909, 0.157461,
				0, -1, 0, 1, -0.7946, -0.1876, -0.5774, 0.636363, 0,
				-0.894425, -0.447215, 0, 1, -0.7946, -0.1876, 0.5774, 0.636363, 0,
				-0.276385, -0.447215, -0.85064, 1, 0.3035, -0.1876, -0.9342, 0.636363, 0,
				0, -1, 0, 1, 0.7946, 0.1876, 0.5774, 0.545454, 0.157461,
				-0.276385, -0.447215, -0.85064, 1, 0.3035, -0.1876, -0.9342, 0.545454, 0.157461,
				0.7236, -0.447215, -0.52572, 1, 0.1876, -0.7947, -0.5774, 0.545454, 0.157461,
				0.7236, -0.447215, 0.52572, 1, 0.6071, -0.7947, 0, 0.090909, 0,
				0.7236, -0.447215, -0.52572, 1, 0.1876, -0.7947, -0.5774, 0.090909, 0,
				0.894425, 0.447215, 0, 1, -0.3035, 0.1876, 0.9342, 0.090909, 0,
				-0.276385, -0.447215, 0.85064, 1, -0.4911, -0.7947, 0.3568, 0, 0.157461,
				0.7236, -0.447215, 0.52572, 1, 0.6071, -0.7947, 0, 0, 0.157461,
				0.276385, 0.447215, 0.85064, 1, -0.9822, 0.1876, 0, 0, 0.157461,
				-0.894425, -0.447215, 0, 1, -0.7946, -0.1876, 0.5774, 0.181818, 0.157461,
				-0.276385, -0.447215, 0.85064, 1, 0.3035, -0.1876, 0.9342, 0.181818, 0.157461,
				-0.7236, 0.447215, 0.52572, 1, -0.3035, 0.1876, -0.9342, 0.181818, 0.157461,
				-0.276385, -0.447215, -0.85064, 1, 0.3035, -0.1876, -0.9342, 0.272727, 0,
				-0.894425, -0.447215, 0, 1, -0.7946, -0.1876, 0.5774, 0.272727, 0,
				-0.7236, 0.447215, -0.52572, 1, 0.7946, 0.1876, -0.5774, 0.272727, 0,
				0.7236, -0.447215, -0.52572, 1, 0.1876, -0.7947, -0.5774, 0.363636, 0.157461,
				-0.276385, -0.447215, -0.85064, 1, 0.3035, -0.1876, -0.9342, 0.363636, 0.157461,
				0.276385, 0.447215, -0.85064, 1, 0.4911, 0.7947, 0.3568, 0.363636, 0.157461,
				0.7236, -0.447215, 0.52572, 1, 0.6071, -0.7947, 0, 0.454545, 0,
				0.894425, 0.447215, 0, 1, -0.3035, 0.1876, 0.9342, 0.454545, 0,
				0.276385, 0.447215, 0.85064, 1, -0.9822, 0.1876, 0, 0.454545, 0,
				-0.276385, -0.447215, 0.85064, 1, -0.4911, -0.7947, 0.3568, 0.636363, 0.314921,
				0.276385, 0.447215, 0.85064, 1, -0.9822, 0.1876, 0, 0.636363, 0.314921,
				-0.7236, 0.447215, 0.52572, 1, -0.1876, 0.7947, 0.5774, 0.636363, 0.314921,
				-0.894425, -0.447215, 0, 1, -0.7946, -0.1876, 0.5774, 0.818181, 0.314921,
				-0.7236, 0.447215, 0.52572, 1, -0.3035, 0.1876, -0.9342, 0.818181, 0.314921,
				-0.7236, 0.447215, -0.52572, 1, 0.7946, 0.1876, -0.5774, 0.818181, 0.314921,
				-0.276385, -0.447215, -0.85064, 1, 0.3035, -0.1876, -0.9342, 0.090909, 0.314921,
				-0.7236, 0.447215, -0.52572, 1, 0.7946, 0.1876, -0.5774, 0.090909, 0.314921,
				0.276385, 0.447215, -0.85064, 1, 0.4911, 0.7947, 0.3568, 0.090909, 0.314921,
				0.7236, -0.447215, -0.52572, 1, 0.1876, -0.7947, -0.5774, 0.272727, 0.314921,
				0.276385, 0.447215, -0.85064, 1, 0.4911, 0.7947, 0.3568, 0.272727, 0.314921,
				0.894425, 0.447215, 0, 1, -0.3035, 0.1876, 0.9342, 0.272727, 0.314921,
				0.276385, 0.447215, 0.85064, 1, -0.9822, 0.1876, 0, 0.454545, 0.314921,
				0.894425, 0.447215, 0, 1, -0.3035, 0.1876, 0.9342, 0.454545, 0.314921,
				0, 1, 0, 1, -0.6071, 0.7947, 0, 0.454545, 0.314921,
				-0.7236, 0.447215, 0.52572, 1, -0.1876, 0.7947, 0.5774, 1, 0.314921,
				0.276385, 0.447215, 0.85064, 1, -0.9822, 0.1876, 0, 1, 0.314921,
				0, 1, 0, 1, -0.1876, 0.7947, -0.5774, 1, 0.314921,
				-0.7236, 0.447215, -0.52572, 1, 0.7946, 0.1876, -0.5774, 0.727272, 0.472382,
				-0.7236, 0.447215, 0.52572, 1, -0.3035, 0.1876, -0.9342, 0.727272, 0.472382,
				0, 1, 0, 1, 0.4911, 0.7947, -0.3568, 0.727272, 0.472382,
				0.276385, 0.447215, -0.85064, 1, 0.4911, 0.7947, 0.3568, 0.90909, 0.472382,
				-0.7236, 0.447215, -0.52572, 1, 0.7946, 0.1876, -0.5774, 0.90909, 0.472382,
				0, 1, 0, 1, 0, 0, -1, 0.90909, 0.472382,
				0.894425, 0.447215, 0, 1, -0.3035, 0.1876, 0.9342, 0.181818, 0.472382,
				0.276385, 0.447215, -0.85064, 1, 0.4911, 0.7947, 0.3568, 0.181818, 0.472382,
				0, 1, 0, 1, 0, 0, -1, 0.181818, 0.472382,
			};
			
			unsigned int indData[] = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 
			};
			glBindVertexArray(VAO_SPHERE);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indData), indData, GL_STATIC_DRAW);

			processor::attribute(0, 4, 9, 0);
			processor::attribute(1, 2, 9, 7);
			processor::attribute(2, 3, 9, 4);

			///
			SHADER_TEXT = processor::LoadShader("res/shaders/text.shader");
			SHADER_POST = processor::LoadShader("res/shaders/post.shader");
			SHADER_FLAT = processor::LoadShader("res/shaders/shader.shader");
			SHADER_BUTTON = processor::LoadShader("res/shaders/button.shader");
			SHADER_SKYBOX = processor::LoadShader("res/shaders/skybox.shader");
			SHADER_INSTANCE = processor::LoadShader("res/shaders/instancing.shader");
			SHADER_COLOR_CORRECTION = processor::LoadShader("res/shaders/colorcorrection.shader");

			vao_def = VAO_FLAT;
			shader_def = SHADER_FLAT;

			mult_color_def = glm::vec4(1);
			mult_texture_def = processor::textureFromColor(glm::vec4(1));
			glyph_clip_def = 0.5f;
			glyph_clip = 0.5f;
			glyph_soft_edge_def = 0.1f;
			glyph_soft_edge = 0.1f;
			glyph_anti_alias_spread_def = 1.f;
			glyph_anti_alias_spread = 1.f;

			uvTransform_def = processor::form(glm::vec3(0));
			transform_def = processor::form(glm::vec3(0));
			globalTransform_def = processor::form(glm::vec3(0));
		}

		// Sets every pass variable to the set default value : like a soft reset
		void reset() {
			vao = vao_def;
			shader = shader_def;

			mask_texture = mask_texture_def;
			mult_texture = mult_texture_def;
			add_texture = add_texture_def;
			mult_color = mult_color_def;
			add_color = add_color_def;

			transform = transform_def;
			globalTransform = globalTransform_def;

			distance_texture = distance_texture_def;
			glyph_clip = glyph_clip_def;
			glyph_soft_edge = glyph_soft_edge_def;
			glyph_anti_alias_spread_def = glyph_anti_alias_spread;

			cust_mat4s = def_cust_mat4s;
			cust_texures = def_cust_textures;
			cust_floats = def_cust_floats;
			cust_float4s = def_cust_float4s;
			cust_float3s = def_cust_float3s;

			uvTransform = uvTransform_def;
		}
	}
}