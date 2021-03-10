#pragma once
#ifndef ENVELOPE_H
#define ENVELOPE_H
class Envelope {
public:
	virtual void tick() = 0;
	virtual void render() = 0;
	virtual void render_gui() = 0;
};
#endif