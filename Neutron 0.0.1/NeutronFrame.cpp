#include "Neutron.h"
namespace neutron {
	// To run and start a window and get input
	namespace frame {

		GLFWwindow* window = NULL;
		int width = 800, height = 800;

		bool paused = false;

		void pause() { paused = true; }
		void unpause() { paused = false; }

		bool running() {
			return !glfwWindowShouldClose(window) && !paused;
		};

		// Time from this frame to last frame, use for stable movement (in seconds)
		float delta_time = 0.0f;
		// Time at last frame
		float last_time = 0.0f;
		// Time at this frame
		float this_time = 0.0f;
		// Duration of last frame
		float duration = 0.0f;

		// Current FPS
		float fps = 0.0f;
		int lastFrameCount = 0;
		// Estimated FPS
		float estimate_fps = 0.0f;

		void* selected_component = nullptr;

		int frames = 0;

		void beginFrame() {
			last_time = this_time;
			this_time = glfwGetTime();
			delta_time = this_time - last_time;
			selected_component = nullptr;
			estimate_fps = 1.0 / delta_time;
		}

		void resize_call(GLFWwindow* window, int width, int height);

		// Error
		void error_call(int code, const char* description) {
			std::cout << "GLFW error (" << code << "): " << description << std::endl;
		}

		int set(std::string name, int width, int height) {
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

			// glfw window creation
			// --------------------
			window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
			if (window == NULL) {
				std::cout << "Failed to create GLFW window" << std::endl;
				Sleep(5000);
				glfwTerminate();
				return -1;
			}
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, resize_call);
			if (mouse::initWithWindow(window) == -1) {
				std::cout << "Mouse couldn't initialize" << std::endl;
				Sleep(5000);
				return -1;
			}
			if (keys::initWithWindow(window) == -1) {
				std::cout << "Keys couldn't initialize" << std::endl;
				Sleep(5000);
				return -1;
			}
			glfwSetErrorCallback(error_call);

			// During init, enable debug output

			// glad: load all OpenGL function pointers
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				std::cout << "Failed to initialize GLAD" << std::endl;
				Sleep(5000);
				return -1;
			}

			return 0;
		}

		void checkInput() {
			keys::beginFrame();
		}

		void swap() {
			++frames;
			if (floor(glfwGetTime()) - lastFrameCount != 0) {
				lastFrameCount = floor(glfwGetTime());
				fps = frames;
				std::cout << "FPS: " << fps << std::endl;
				frames = 0;
			}

			glfwSwapBuffers(window);
			mouse::endFrame();
			keys::endFrame();
			glfwPollEvents();
		}

		void resize_call(GLFWwindow* window, int width, int height) {
			frame::width = width;
			frame::height = height;
			if (modes::post_processing)
				modes::post::resetScreenSize(width, height);
		}

		void stop() {
			glfwSetWindowShouldClose(window, true);
		}

		namespace mouse {
			bool left = false, right = false, middle = false, l = false, r = false, m = false, lClicked = false, rClicked = false, mClicked = false, lReleased = false, rReleased = false, mReleased = false;
			float x = 0, y = 0, pX = 0, pY = 0, prevX = 0, prevY = 0, nextX = 0, nextY = 0, nX = 0, nY = 0;
			double totalScroll = 0, scroll = 0, x_scroll = 0, y_scroll = 0, x_s = 0, y_s = 0, scroll_glide = 0, x_scroll_glide = 0, y_scroll_glide = 0, scroll_g = 0, x_s_g = 0, y_s_g = 0, GLIDE_DECAY = 0.75;
			bool print = true;
			void lock() {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			void unlock() {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			void endFrame() {
				lClicked = false;
				rClicked = false;
				mClicked = false;
				lReleased = false;
				rReleased = false;
				mReleased = false;
				scroll = 0;
				pX = x;
				pY = y;
				prevX = x;
				prevY = y;

				y_s_g *= GLIDE_DECAY;
				x_s_g *= GLIDE_DECAY;
				y_scroll_glide = y_s_g;
				x_scroll_glide = x_s_g;
				scroll_glide = y_s_g;
				scroll_g = y_s_g;
			}
			bool isLocked() {
				return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
			}
			void mouse_movement_call(GLFWwindow* window, double xpos, double ypos) {
				pX = x;
				pY = y;
				prevX = x;
				prevY = y;
				x = xpos;
				y = ypos;
				nX = x + (x - pX);
				nY = y + (y - pY);
				nextX = nX;
				nextY = nY;
			}
			void mouse_button_call(GLFWwindow* window, int button, int action, int mods) {
				if (button == GLFW_MOUSE_BUTTON_LEFT) {
					if (action == GLFW_PRESS) {
						if (print)
							std::cout << "Left mouse pressed (" << x << ", " << y << ")" << std::endl;
						lClicked = !l;
						l = true;
						left = true;
					}
					else {
						if (print)
							std::cout << "Left mouse released (" << x << ", " << y << ")" << std::endl;
						lClicked = false;
						l = false;
						left = false;
						lReleased = true;						
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
					if (action == GLFW_PRESS) {
						if (print)
							std::cout << "Right mouse pressed (" << x << ", " << y << ")" << std::endl;
						rClicked = !right;
						r = true;
						right = true;
					}
					else {
						if (print)
							std::cout << "Right mouse released (" << x << ", " << y << ")" << std::endl;
						rClicked = false;
						r = false;
						right = false;
						rReleased = true;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
					if (action == GLFW_PRESS) {
						if (print)
							std::cout << "Middle mouse pressed" << std::endl;
						mClicked = !middle;
						m = true;
						middle = true;
					}
					else {
						if (print)
							std::cout << "Middle mouse released" << std::endl;
						mClicked = false;
						m = false;
						middle = false;
						mReleased = true;
					}
				}
			}
			void mouse_scroll_call(GLFWwindow* window, double x, double y) {
				x_scroll = x;
				x_s = x;
				y_scroll = y;
				y_s = y;
				totalScroll += y;
				scroll = y;
				y_s_g += y;
				x_s_g += x;
				y_scroll_glide = y_s_g;
				x_scroll_glide = x_s_g;
				scroll_glide = y_s_g;
				scroll_g = y_s_g;
			}
			int initWithWindow(GLFWwindow* window) {
				glfwSetCursorPosCallback(window, mouse_movement_call);
				glfwSetMouseButtonCallback(window, mouse_button_call);
				glfwSetScrollCallback(window, mouse_scroll_call);
				return 0;
			}
		};

		namespace keys {

			std::vector<bool> keys = std::vector<bool>(GLFW_KEY_LAST);
			bool typed = false, released = false, recordTyping = false;
			std::string typedInputString = "";
			std::map<unsigned int, void(*)()> pressedEvents = std::map<unsigned int, void(*)()>(), releasedEvents = std::map<unsigned int, void(*)()>(), typedEvents = std::map<unsigned int, void(*)()>();
			bool print = false;
			void beginFrame() {
				for (std::pair<unsigned int, void(*)()> i : pressedEvents) {
					if (keys[i.first])
						i.second();
				}
			}

			void endFrame() {
				typed = false;
				released = false;
			}

			void press_call(GLFWwindow* window, int key, int scancode, int action, int mods) {
				if (key == -1)
					return;
				while (key >= keys.size())
					keys.push_back(false);
				if(print && action!=GLFW_REPEAT)
					std::cout << "Key " << key << (action == GLFW_PRESS ? " pressed " : action == GLFW_RELEASE ? " released " : " (has unkown action) " ) << (glfwGetKeyName(key, scancode)==NULL ? "" : glfwGetKeyName(key, scancode)) << std::endl;
				typed = action == GLFW_PRESS || action == GLFW_REPEAT;
				if (typed && recordTyping) {
					if (key == GLFW_KEY_BACKSPACE)
						typedInputString = typedInputString.substr(0, strlen(typedInputString.c_str()) - 1);
					else
						typedInputString += (glfwGetKeyName(key, scancode) == NULL ? "" : glfwGetKeyName(key, scancode));
				}
				keys[key] = typed;
				released = !typed;
				if (typed && typedEvents.count(key) != 0) {
					typedEvents[(unsigned int)key]();
				}
				else if(released && releasedEvents.count(key) != 0) {
					releasedEvents[(unsigned int)key]();
				}
			}

			int initWithWindow(GLFWwindow* window) {
				glfwSetKeyCallback(window, press_call);
				return 0;
			}

			void whenKeyPressedDo(unsigned int keyId, void(*function)()) {
				pressedEvents.insert(std::make_pair(keyId, function));
			}
			void whenKeyTypedDo(unsigned int keyId, void(*function)()) {
				typedEvents.insert(std::make_pair(keyId, function));
			}
			void whenKeyReleasedDo(unsigned int keyId, void(*function)()) {
				releasedEvents.insert(std::make_pair(keyId, function));
			}

		};
	};
};