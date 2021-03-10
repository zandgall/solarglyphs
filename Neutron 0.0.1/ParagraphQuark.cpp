#include "ParagraphQuark.h"


// Create a Quark that can render (a) paragraph(s) of text at the designated position, with a constraining getF(WIDTH), a font-size, and optionally an indent and font.
ParagraphQuark::ParagraphQuark(glm::vec3 position, std::string text, float width, float size, float indent, std::string font) : Quark(position, 0.f, glm::vec3(0), glm::vec3(0)) {
	setS(TEXT, text);
	setF(WIDTH, width);
	setF(FONT_SIZE, size);
	setF(INDENT, indent);
	setS(FONT, font);
	updateBounds();
	setB(GUI, true);
	setF(GLYPH_CLIP, 0.4);
	setF(GLYPH_SOFT_EDGE, 0.1);
	setF4(COLOR, glm::vec4(0, 0, 0, 1));
	setB(ALPHA_TEST, false);
	setUINT(SHADER, passes::SHADER_FLAT);
	setUINT(ALIGNMENT, ALIGN_LEFT);
}

ParagraphQuark::~ParagraphQuark() {

}

// Updates the boundaries of individual lines of text, to use word wrapping.
void ParagraphQuark::updateBounds() {
	float in = getF(INDENT), x = in, y = 0;	// Controlling an offset of each line
	std::string o = "";						// The line that characters will be added to, and then measured
	int spIn = 0;							// Saves the last 'space' character to use for a breaking point
	lines = std::vector<std::string>();		// Resetting what will be the list of each line of text

	for (int i = 0; i < getS(TEXT).size(); i++) {
		glm::vec2 s = world::measureString(o, getS(FONT).c_str()); // Get the size of the string to use for later

		if (s.x * (getF(FONT_SIZE)/s.y) + in >= getF(WIDTH)) {	// If the getF(WIDTH) of the text in proportion to the native font size, to the selected fonts default size, is bigger or equal to the set getF(WIDTH):
			lines.push_back(o.substr(0, spIn));						// Break the line from the saves breakpoint, store it,
			o = o.substr(spIn, o.length()-spIn);					// and continue from the breakpoint
			in = 0;													// Reset the indent so that it doesn't cut off each next line early
		}

		o += getS(TEXT)[i];			// Add the current character
		if (getS(TEXT)[i] == '\n') {// and if it's a manual breakpoint:
			lines.push_back(o);		// Save the current text
			o = "";					// Clear the current text
			in = getF(INDENT);		// And use an indent in measuring next line
		}
		if (spIn == 0 || getS(TEXT)[i] == ' ')	// If the current character's a space
			spIn = o.length();					// Save it's position to use as a breakpoint in the future, unless another space is found before then
	}
	lines.push_back(o);													// Push back the last line that didn't have a breakpoint
	size = glm::vec3(getF(WIDTH), lines.size()*(getF(FONT_SIZE)*1.5 + getF(LINE_SPACING)) + getF(FONT_SIZE) / 2, 0);  // And calculate boundaries
}

// Cache Current text as a texture to render with, instead of rendering individual lines every render loop
void ParagraphQuark::freezeToTexture() {
	updateBounds();
	processor::drawToTexture2D(texture, getF(WIDTH), size.y, GL_LINEAR, GL_RGBA);
			// Start drawing to the texture with the given width, and a calculated height. Set the texture to linear interpolation, and use all 4 RGB and A channels
	set();	// And set all uniforms
	//modes::flip_vertically = true;	// Fix vertical flip
	float in = getF(INDENT);				// Start with an indent
	modes::draw = modes::DRAW_GLYPH;		// Use glyh mode
	modes::gui = true;						/// as a gui :: CAN BE REPLACED LATER WHEN USING PARAGRAPHS IN THE 3D SPACE
	modes::alpha_test = false;				// And don't alpha-test, so that opacity in each pixel is preserved
	passes::shader = passes::SHADER_FLAT;	// Use default shader
	for (int i = 0; i < lines.size(); i++) {								// For every line,
		glm::vec2 s = world::measureString(lines[i], getS(FONT).c_str());		// measure it,
		if (getUINT(ALIGNMENT) == ALIGN_LEFT)									// And if this paragraph aligns left,
			in += s.x * (getF(FONT_SIZE)/s.y)/2 - getF(WIDTH)/2;				// Then use the measurement to calculate it's offset
		if (getUINT(ALIGNMENT) == ALIGN_RIGHT)									// But if it's aligning right,
			in += getF(WIDTH) / 2 - s.x * (getF(FONT_SIZE) / s.y) / 2 - in;	// Do the same to offset to the right side

		// Set the draw transform to a rectangle from the current indent, (or 0 if not using it), and a centering offset + it's line offset, with a getF(WIDTH) and the line size
		passes::transform = neutron::processor::rect(in, -getF(FONT_SIZE) / 2 + (i * (getF(FONT_SIZE)*1.5+getF(LINE_SPACING))), getF(WIDTH), getF(FONT_SIZE)*1.5, this->rotation);

		drawText(lines[i], getS(FONT).c_str(), 0, getF4(COLOR), 0.0f, 6);	// Draw the lines,
		in = 0.0f;															// Reset the indent to not indent later
		if (lines[i].find('\n') != std::string::npos)						// Buf if there's a manual breakpoint
			in = getF(INDENT);												// then indent the next line
	}
	frozen = true;							// Set the paragraph to frozen so it knows to render with it's texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// And bind the default framebuffer to draw to the screen
	//modes::flip_vertically = false;			// Reset vertical flip
}

// Render the paragraph of text
void ParagraphQuark::render() {
	if (!frozen)			// If it's not frozen then
		freezeToTexture();	// Freeze the rendering to a texture

	set();																										// set the uniforms like normal,
	modes::draw = modes::DRAW_FLAT;																				// draw in "flat" mode,
	float h = size.y;																							// grab the height of the line,
	passes::transform = neutron::processor::rect(position.x, position.y-h, getF(WIDTH), h, this->rotation);		// and use it to center the transform.
	modes::gui = true;																							/// Render in gui mode :: CAN BE REPLACED LATER WHEN USING PARAGRAPHS IN THE 3D SPACE
	
	draw(texture, glm::vec4(0), layer, 6);																		// and draw
}