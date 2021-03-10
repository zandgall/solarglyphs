#pragma once
#ifndef NEUTRON_H
#define NEUTRON_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "stb_image.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include<iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <windows.h>
#include <map>
#include <ft2build.h>
#include <freetype/freetype.h>

extern GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

namespace neutron {
	// A set of commands that can be used to process variables and use complex initiators
	namespace processor {
		struct ShaderProgramSource {
			std::string VertexSource;
			std::string FragmentSource;
			void clear() {
				VertexSource = "";
				FragmentSource = "";
			}
		};

		extern std::map<const std::string, unsigned int> preLoad;

		extern ShaderProgramSource ParseShader(const std::string& filepath);

		extern unsigned int CompileShader(unsigned int type, const std::string& source);

		extern unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

		extern unsigned int LoadShader(const std::string& filepath, bool cache = true);

		extern void uniM4(unsigned int shader, const GLchar* address, glm::mat4 matrix);

		extern void uniF4(unsigned int shader, const GLchar* address, glm::vec4 vec);

		extern void uniF3(unsigned int shader, const GLchar* address, glm::vec3 vec);

		extern void uniF2(unsigned int shader, const GLchar* address, glm::vec2 vec);

		extern void uniF1(unsigned int shader, const GLchar* address, float a);

		extern void uniF4(unsigned int shader, const GLchar* address, float a, float b, float c, float d);

		extern void uniF3(unsigned int shader, const GLchar* address, float a, float b, float c);

		extern void uniF2(unsigned int shader, const GLchar* address, float a, float b);

		extern void uniI4(unsigned int shader, const GLchar* address, int a, int b, int c, int d);

		extern void uniI3(unsigned int shader, const GLchar* address, int a, int b, int c);

		extern void uniI2(unsigned int shader, const GLchar* address, int a, int b);

		extern void uniI1(unsigned int shader, const GLchar* address, int a);

		extern void uniB(unsigned int shader, const GLchar* address, bool a);

		//Your shader ID, the name of the sampler in your shader, (i.e. "diffuseTexture"), the ID of your texture, the number you want to give to your texture, and a target to go allong with it (i.e. "0" and "GL_TEXTURE0")
		extern void uniSampler(unsigned int shader, const char* name, unsigned int texture, int shaderNumber);
		// Returns a translation matrix
		extern glm::mat4 translation(glm::vec3 pos);
		// Returns a rotation matrix off of a given matrix
		extern glm::mat4 rotation(glm::mat4 mat, glm::vec3 rot);
		// Returns a rotation matrix off of [1]
		extern glm::mat4 rotation(glm::vec3 rot);
		// Returns a scalar matri
		extern glm::mat4 scaled(glm::vec3 scaling);
		// Returns a positioned, rotated, and possibly scaled matrix off of given matrix, or [1]
		extern glm::mat4 form(glm::vec3 pos, glm::vec3 scale = glm::vec3(1), glm::vec3 rot = glm::vec3(0), glm::mat4 mat = glm::mat4(1));

		extern glm::mat4 rect(float x, float y, float w, float h, glm::vec3 rot = glm::vec3(0), glm::mat4 mat = glm::mat4(1));

		// values
		extern unsigned int colstoimg, dVAO, /*Distance VAO*/ dVBO, /*Distance VBO*/ distanceMap /*A bilinear texture of a dot in the center*/;

		// Gives an assigned VBO
		extern unsigned int VBO();
		// Gives an assigned VBO but with vertices in it
		extern unsigned int VBO(float vertices[]);
		// Gives an assigned EBO
		extern unsigned int EBO();
		// Gives an assigned VAO
		extern unsigned int VAO();
		// Attributes 'in' values for a shader
		extern void attribute(int order_in_shader, int length, int elements_per_vertex, int index_in_array);
		// Attributes but with a direct index
		extern void attributedirect(int order_in_shader, int length, int elements_per_vertex, const void* index_in_array);
		// Loads a texture with a given interpolation, or bilinear default
		extern unsigned int getTextureFrom(const char* filepath, unsigned int interpol = GL_LINEAR, unsigned int wrap = GL_REPEAT, bool cache = true);
		/// UNDOCUMENTED: returns a cubemapid from a resolution
		extern std::vector<unsigned int> createCubemapID_UNDOCUMENTED(int GEN_RESOLUTION = 124);
		/// UNTESTED: Creates an empty cubemap with width and height
		extern unsigned int createEmptyCubemap(int width, int height);
		/// UNTESTED: Binds a texture to the side of a cubemap
		extern unsigned int bindSideOfCubemap(std::vector<unsigned int> textures, unsigned int cubeTex);
		// Loads a cubemap from set of faces as filepaths
		extern unsigned int loadCubemap(std::vector<std::string> faces, bool cache = false);
		// Returns a texture from the given colors, size, and possibly interpolation, or nearest-neighbor default 
		extern unsigned int textureFromColors(std::vector<glm::vec4> colors, int width, int height, unsigned int interpol = GL_NEAREST);
		// Returns a texture from the given color
		extern unsigned int textureFromColor(glm::vec4 color);
		// Returns a "distance field" for glyph rendering based on given colors, size, and margin
		extern unsigned int getDistanceField(std::vector<glm::vec4> colors, int width, int height, int marginX = 6, int marginY = 8);
		/// UNFINISHED Returns a set of colors from a filepath
		extern std::vector<glm::vec4> colorsFromTextureUNFINISHED(const char* filepath);
		// Returns a set of colors from a textureID
		extern std::vector<glm::vec4> colorsFromTexture(unsigned int texture);
		// Gets the size of a texture
		extern glm::ivec2 sizeofTexture(unsigned int texture, unsigned int textureType = GL_TEXTURE_2D);
		// Gets a distance field from a textureID
		extern unsigned int getDistanceField(unsigned int texture);
		// Returns a distance field for a text specific glyph
		extern unsigned int getTextDistanceField(unsigned int TEXTure);

		// Screenspace coordinate to pixel coord
		extern glm::vec4 toPixelCoord(glm::vec4 screenspaceCoordinate);
		// Pixel coordinate to screenspace coord
		extern glm::vec4 toScreenCoord(glm::vec4 pixelCoordinate);
		// Worldspace transformation to screenspace transformation
		extern glm::mat4 toScreenTransform(glm::mat4 transform);
		/// Get coord again trying lets see if this works haha yeah Change desc when possible
		extern glm::vec4 useMatrices(glm::vec4 position);

		extern std::vector<std::string> split(const std::string& s, char delimiter);

		extern float mapTo(float in, float inMax, float inMin, float outMax, float outMin);

		extern unsigned int margin(unsigned int texture, unsigned int marginX, unsigned int marginY);

		extern void drawToTexture2D(unsigned int &texture, int width, int height, int interpolation = GL_NEAREST, int colors = GL_RGBA);

		// Create a color correction matrix
		extern glm::mat4x3 getColorCorrection(glm::vec3 selectColor, glm::vec3 mult = glm::vec3(1), glm::vec3 add = glm::vec3(0), glm::vec3 tolerance = glm::vec3(0.1));
		// Modify a color correction matrix's contrast and brightness
		extern void changeColorCorrectionBrightness(glm::mat4x3 correction, float brightness, float contrast = 1.0, bool hsv = true);
		// Set a color correction matrix's contrast and brightness
		extern void setColorCorrectionBrightness(glm::mat4x3 correction, float brightness, float contrast = 1.0, bool hsv = true);
	};

	// Different options for how Neutron should handle things
	namespace modes {
		// DRAW_FLAT just draws the texture and/or color as is. DRAW_GLYPH draws using a distance texture to map out a glyph. DRAW_INSTANCED uses instance methods with certain functions like drawText
		const unsigned int DRAW_FLAT = 0, DRAW_GLYPH = 1, DRAW_INSTANCED = 2;
		// draw = changes how draw() should handle it's parameters : draw_def = what draw will be on reset
		extern unsigned int draw, draw_def;

		// Draw in a GUI mode or not, removing perspective from what you're going to draw
		extern bool gui, gui_def;

		// Test alpha whether to discard under values of 0.8 or to rely on sorted blending. Disabling works mostly with gui : def is default on neutron::reset();
		extern bool alpha_test, alpha_test_def;

		// Whether to anti-alias or not
		extern bool anti_alias, anti_alias_def;

		// Different flipping options
		extern bool flip_horizontally, flip_vertically;

		// Text options
		extern bool text_equispace, text_equispace_def;

		// Set everything to the default values after drawing?
		extern bool reset_on_draw;

		// Whether or not Neutron should post process
		extern bool post_processing;
		namespace post {
			// The Framebuffer that will be modified and pushed to the screen when done rendering
			extern unsigned int screen;
			// The Texture assigned with the screen Framebuffer
			extern unsigned int screenTex;
			// A type of post processing
			extern float vignette, bloom, bloomIterations, bloomThreshold, gamma;
			// Post processing variable
			extern glm::vec4 vignetteColor, hsvAdd, hsvMult, rgbAdd, rgbMult;
			// Color selection and correction, where the first layer is the color you'd want to change, the second layer is the tolerance of the color selection, the third layer is the value you want to multiply by, and the fourth layer is the layer to add by
			extern std::vector<glm::mat4x3> hsvCorrection, rgbCorrection;

			extern void set();
			extern void resetScreenSize(int width, int height);
			extern void bindPost();
			extern void pushToScreen();
		};

		extern void reset();
		extern void set();
	};
	// The stuff you pass to the draw function to draw exactly what you're looking for
	namespace passes {
		// A preset VAO with a shape as described in the name
		extern unsigned int VAO_FLAT, VAO_CUBE, VAO_SPHERE;
		// The VAO draw() should use to render : def is the default on neutron::reset()
		extern unsigned int vao, vao_def;

		// A preset shader that uses every parameter as expected
		extern unsigned int SHADER_FLAT, SHADER_INSTANCE, SHADER_BUTTON, SHADER_POST, SHADER_COLOR_CORRECTION, SHADER_TEXT, SHADER_SKYBOX;
		// What shader Neutron should use : def is the default on neutron::reset();
		extern unsigned int shader, shader_def;

		/// COLOR PROCESSING
		// What mask texture to use, or use none with 0 : def is the default on neutron::reset();
		extern unsigned int mask_texture, mask_texture_def;
		// What multiply texture to use, or use none with 0 : def is the default on neutron::reset();
		extern unsigned int mult_texture, mult_texture_def;
		// What additive texture to use, or use none with 0 : def is the default on neutron::reset();
		extern unsigned int add_texture, add_texture_def;

		// A color to multiply to the result of draw()
		extern glm::vec4 mult_color, mult_color_def;
		// A color to add to the result of draw()
		extern glm::vec4 add_color, add_color_def;

		/// COORDINATES
		// A transformation matrix used to offset, rotate, or scale UV coordinates
		extern glm::mat4 uvTransform, uvTransform_def;

		/// CUSTOM
		// Custom unsigned ints
		extern std::map<std::string, unsigned int> cust_texures, def_cust_textures;
		// Custom floats
		extern std::map<std::string, float> cust_floats, def_cust_floats;
		// Custom float4s
		extern std::map<std::string, glm::vec4> cust_float4s, def_cust_float4s;
		// Custom float3s
		extern std::map<std::string, glm::vec3> cust_float3s, def_cust_float3s;
		// Custom mat4s
		extern std::map<std::string, glm::mat4> cust_mat4s, def_cust_mat4s;

		/// GEOMETRY PROCESSING
		// Where the object is, and how it's scaled and rotated : def is the default on neutron::reset();
		extern glm::mat4 transform, transform_def;
		// To apply automatically to everything when drawing : def is the default on neutron::reset();
		extern glm::mat4 globalTransform, globalTransform_def;
		// Used to correct any drawing methods that require scalars from the transform matrix, e.i. text correction
		extern glm::vec3 rotation;
		// Used for 2d elements in 3d space that always face the camera
		extern bool face_camera;

		/// GLYPH PROCESSING
		// What texture to use as a distance texture, or 0 for no texture at all. used in glyph rendering : def is the default on neutron::reset();
		extern unsigned int distance_texture, distance_texture_def;
		// Glyph nearest mask texture to create hard edges : def is the default on neutron::reset();
		extern unsigned int glyph_nearest_mask, glyph_nearest_mask_def;
		// What value the glyph distancefield clips at : def is the default on neutron::reset();
		extern float glyph_clip, glyph_clip_def;
		// How soft the edges will seem when drawing a glyph : def is the default on neutron::reset();
		extern float glyph_soft_edge, glyph_soft_edge_def;
		// The spread/blurriness of a glyph when anti-aliased
		extern float glyph_anti_alias_spread, glyph_anti_alias_spread_def;

		// Sets the default value of every passed variable : like a hard reset
		extern void set();

		// Sets every pass variable to the set default value : like a soft reset
		extern void reset();
	};

	namespace world {

		extern float clip_near, clip_far;

		// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
		enum Camera_Movement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
		class Camera {
		public:
			static const unsigned int MODE_ORBIT = 0, MODE_FREE = 1;
			unsigned int moveMode = MODE_FREE;
			bool LOCK_UP_VALUES = true;
			// Camera Attributes
			glm::vec3 position;
			glm::vec3 Front;
			glm::vec3 Up;
			glm::vec3 Right;
			glm::vec3 Left;
			glm::vec3 Down;
			glm::vec3 Back;
			glm::vec3 WorldUp;
			// Euler Angles
			float Yaw;
			float Pitch;
			// Camera options
			float MovementSpeed;
			float MouseSensitivity;
			float Zoom;
			float OrbitDistance = 1, OrbitDistanceMin = 0.1, OrbitDistanceMax = INFINITY;

			// Constructor with vectors
			Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.f, float pitch = 0.f) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(116.0f) {
				this->position = position;
				this->WorldUp = up;
				this->Yaw = yaw;
				this->Pitch = pitch;
				updateCameraVectors();
			}
			// Constructor with scalar values
			Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(116.0f) {
				position = glm::vec3(posX, posY, posZ);
				WorldUp = glm::vec3(upX, upY, upZ);
				Yaw = yaw;
				Pitch = pitch;
				updateCameraVectors();
			}

			// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
			glm::mat4 GetViewMatrix();

			// Returns a new view matrix with a position, front, and up value
			glm::mat4 genViewMatrix(glm::vec3 Position, glm::vec3 Front, glm::vec3 Up) {
				return glm::lookAt(Position, Position + Front, Up);
			}

			// Smoothly track to a position
			void centerOn(glm::vec3 pos, float smoothness = 0.75);

			// Constrain to area
			void constrain(float left, float up, float right, float down) {
				position.x = max((float)(min(right, position.x)), left);
				position.y = max((float)(min(down, position.y)), up);
			}

			// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
			void ProcessKeyboard(Camera_Movement direction);

			// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
			void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

			// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
			void ProcessMouseScroll(float yoffset);

		private:
			// Calculates the front vector from the Camera's (updated) Euler Angles
			void updateCameraVectors() {
				// Calculate the new Front vector
				glm::vec3 front;
				front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				front.y = sin(glm::radians(Pitch));
				front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				Front = glm::normalize(front);
				// Also re-calculate the Right and Up vector
				Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
				Up = glm::normalize(glm::cross(Right, Front));
				Left = glm::vec3(-Right.x, -Right.y, -Right.z);
				Down = glm::vec3(-Up.x, -Up.y, -Up.z);
				Back = glm::vec3(-Front.x, -Front.y, -Front.z);
			}
		};

		struct Character {
			unsigned int texture;
			glm::ivec2 size;
			glm::ivec2 bearing;
			glm::ivec2 advance;
		};
		extern std::map<std::string, std::map<GLchar, Character>> characters;
		extern std::map<std::string, int> sizeMap;

		extern FT_Library ft;

		extern void loadFont(const char* filepath, std::string id = "arial", int size = 48, byte glyphsNum = 255);

		extern glm::vec2 measureString(const std::string &text, const char* font, float xspacing = 0, float yspacing = 0);

		extern world::Camera camera;

		extern void set();

		extern void reset();
	};

	// To run and start a window and get input
	namespace frame {

		// Current window as set in GLFW
		extern GLFWwindow* window;
		// Width and height of window
		extern int width, height;

		// Time from this frame to last frame, use for stable movement
		extern float delta_time;
		// Time at last frame
		extern float last_time;
		// Time at this frame
		extern float this_time;
		// Duration of last frame
		extern float duration;
		
		// Current FPS
		extern float fps;
		// Estimated FPS
		extern float estimate_fps;

		// Current selected object of the screen
		extern void* selected_component;

		extern void pause();
		extern void unpause();

		extern bool running();

		extern int set(std::string name = "Neutron", int width = 800, int height = 800);

		extern void checkInput();

		extern void swap();

		extern void beginFrame();

		extern void stop();

		namespace mouse {

			// Mouse Button inputs
			extern bool left, right, middle, l, r, m, lClicked, rClicked, mClicked, lReleased, rReleased, mReleased;
			// Mouse Position inputs
			extern float x, y, pX, pY, prevX, prevY, nextX, nextY, nX, nY;
			// Mouse Scroll inputs
			extern double totalScroll, scroll, x_scroll, y_scroll, x_s, y_s, scroll_glide, x_scroll_glide, y_scroll_glide, scroll_g, x_s_g, y_s_g, GLIDE_DECAY;

			extern int initWithWindow(GLFWwindow* window);

			extern void lock();
			extern void unlock();
			extern void endFrame();
			extern bool isLocked();
		};

		namespace keys {

			extern std::vector<bool> keys;
			extern bool typed, released, recordTyping;
			extern std::string typedInputString;
			extern std::map<unsigned int, void(*)()> pressedEvents, releasedEvents, typedEvents;
			
			extern int initWithWindow(GLFWwindow* window);
			extern void beginFrame();
			extern void endFrame();
			extern void whenKeyPressedDo(unsigned int keyId, void(*function)());
			extern void whenKeyTypedDo(unsigned int keyId, void(*function)());
			extern void whenKeyReleasedDo(unsigned int keyId, void(*function)());

		};
	};

	// Use this before your main loop
	extern void beforeLoop();
	// Use this after your main loop
	extern void afterLoop();

	// Sets every value to it's given default, useful for customizing your default gamemode/drawmode : like a soft reset
	extern void reset();

	// Sets every default value, required on startup : like a hard reset
	extern void set(std::string name = "Neutron", int width = 800, int height = 800);

	extern bool isSet();

	extern int setUniforms(unsigned int texture = 0, glm::vec4 color = glm::vec4(1), float layer = 0.f, unsigned int vertices = 0);

	extern void closeTextures(unsigned int number = 32);

	extern void draw(unsigned int texture = 0, glm::vec4 color = glm::vec4(1), float layer = 0.f, unsigned int vertices = 0);

	extern void drawInstances(std::vector<glm::mat4> instances, unsigned int texture = 0, glm::vec4 color = glm::vec4(1), float layer = 0.f, unsigned int vertices = 0);

	extern void drawText(const std::string &text, const char* font, unsigned int texture, glm::vec4 color, float layer, unsigned int vertices, float xspacing = 0, float yspacing = 0);
};
#endif
