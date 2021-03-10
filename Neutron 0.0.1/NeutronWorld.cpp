#include "Neutron.h"
namespace neutron {
	namespace world {
		float clip_near = 0.05f;
		float clip_far = 100.0f;

		std::map<std::string, std::map<GLchar, Character>> characters = std::map<std::string, std::map<GLchar, Character>>();
		std::map<std::string, int> sizeMap = std::map<std::string, int>();
		FT_Library ft = FT_Library();

		void loadFont(const char* filepath, std::string id, int size, byte glyphsNum) {
			FT_Face face; // Load a font face and set the size          ->|
			if (FT_New_Face(ft, filepath, 0, &face))                    //|
				std::cout << "Failed to load font " << id << std::endl; //|
			FT_Set_Pixel_Sizes(face, 0, size);//                        <-|

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Set alignment to 1 for a binary image
			unsigned int exceptions = 0;
			std::map<GLchar, Character> characterList = std::map<GLchar, Character>();

			sizeMap.insert(make_pair(id, size));

			for (GLubyte c = 0; c < glyphsNum; c++) { // Read char through assigned number of glyphs
				if (FT_Load_Char(face, c, FT_LOAD_RENDER)) { // Read the char         ->|
					exceptions++; // If not read, add to exceptions to print at end   //|
					continue;                                                         //|
				}//                                                                   <-|

				// Generate texture
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D( // Assign the texture 
					GL_TEXTURE_2D,				// ..as a 2d texture
					0,                          // ..with 0 mipmapping
					GL_RED,                     // ..in red format
					face->glyph->bitmap.width,	// ..with width of glyph
					face->glyph->bitmap.rows,	// ..with height of glyph
					0,							// ..with no border
					GL_RED,						// ..to the red channel
					GL_UNSIGNED_BYTE,			// ..using unsigned bytes
					face->glyph->bitmap.buffer	// ..the glyph binary bitmap
				);

				// Set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap to the other side (horizontal)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// Don't overlap to the other side (vertical)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// Use nearest neighbor
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// for quickest loading
				// Now store character for later use
				Character character = {	// Create a character
				processor::margin(texture, 1, 1),											//..with a distance field from texture
				glm::ivec2(face->glyph->bitmap.width + 2, face->glyph->bitmap.rows + 2),	//..with the glyph's size
				glm::ivec2(face->glyph->bitmap_left - 1, face->glyph->bitmap_top - 1),		//..with the glyph's margins
				glm::vec2(face->glyph->advance.x, face->glyph->advance.y)					//..with the glyph's forward and downward advance
				};

				characterList.insert(std::make_pair(c, character)); // Add to list
				//delete &character; // Delete temporary character
			}
			if (exceptions > 0) { // Print exceptions												
				std::cout << "ERROR::FREETYPE: Failed to load " << exceptions << " GLyph(s)" << std::endl;
			}

			characters.insert(make_pair(id, characterList)); // Insert to final font-(character-glyph) pairing

			FT_Done_Face(face); // End font
			characterList.empty(); // Empty temporary list

			std::cout << "Loaded font " << filepath << "(" << id << ") with " << glyphsNum << " glyphs" << std::endl;
		};

		glm::vec2 measureString(const std::string &text, const char* font, float xspacing, float yspacing) {
			std::string::const_iterator c;
			float x = 0, y = 0;
			for (c = text.begin(); c != text.end(); c++) {
				Character ch = characters[font][*c];
				if (modes::text_equispace)
					ch = characters[font]['m'];
				x += (ch.advance.x / 64.f)+xspacing;
				y += (ch.advance.y > 0 ? ((ch.advance.y >> 6) + yspacing) : 0);
			}
			return glm::vec2(x, y+sizeMap[font]);
		}

		Camera camera = Camera();

		void set() {
			if (FT_Init_FreeType(&ft))
				std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		}

		void reset() {

		}

		glm::mat4 Camera::GetViewMatrix() {
			if (moveMode == MODE_ORBIT)
				return glm::lookAt(position - (Front*OrbitDistance), position, Up);
			return glm::lookAt(position, position + Front, Up);
		}


		void Camera::centerOn(glm::vec3 pos, float smoothness) {
			float ratio = max(smoothness+frame::delta_time, 0);
			position.x = (position.x*ratio + (pos.x*(1.0 - ratio)));
			position.y = (position.y*ratio + (pos.y*(1.0 - ratio)));
			position.z = (position.z*ratio + (pos.z*(1.0 - ratio)));
		}

		void Camera::ProcessKeyboard(Camera_Movement direction) {
			float velocity = MovementSpeed * frame::delta_time * (Zoom / 90.0f);
			glm::vec3 f = Front;
			glm::vec3 r = Right;
			glm::vec3 u = Up;
			if (LOCK_UP_VALUES) {
				f = glm::normalize(f - (f*WorldUp));
				r = glm::normalize(r - (r*WorldUp));
				u = WorldUp;
			}
			if (direction == FORWARD)
				position += f * velocity;
			if (direction == BACKWARD)
				position -= f * velocity;
			if (direction == LEFT)
				position -= r * velocity;
			if (direction == RIGHT)
				position += r * velocity;
			if (direction == UP)
				position += u * velocity;
			if (direction == DOWN)
				position -= u * velocity;
		}
	
		void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch) {
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}

			// Update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		void Camera::ProcessMouseScroll(float yoffset) {
			if (moveMode == MODE_ORBIT) {
				OrbitDistance -= frame::delta_time * yoffset;
				OrbitDistance = min(max(OrbitDistance, OrbitDistanceMin), OrbitDistanceMax);
			}
			else {
				if (Zoom >= 1.0f && Zoom <= 180.0f)
					Zoom -= yoffset;
				if (Zoom <= 1.0f)
					Zoom = 1.0f;
				if (Zoom >= 180.0f)
					Zoom = 180.0f;
				//Zoom = 90.0f;
			}
		}
	}
}