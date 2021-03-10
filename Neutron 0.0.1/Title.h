#pragma once
#include "Envenlope.h"
#ifndef TITLE_H
#define TITLE_H
class Title : public Envelope {
public:
	Title();
	void tick();
	void render();
	void render_gui();
};
#endif