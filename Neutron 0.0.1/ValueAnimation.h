#pragma once
#include "Neutron.h"
#include <vector>
#include <math.h>
#ifndef VALUEANIMATION_H
#define VALUEANIMATION_H
class KeyFrame {
public:
	static const int CALCCONST = 0, CALCLINEAR = 1, CALCSIN = 2, CALCCUBIC = 3, CALCCIRC = 4, CALCELASTIC = 5, CALCBACK = 9, CALCBOUNCE = 10;
	static const int TYPEIN = 0, TYPEOUT = 1, TYPEINOUT = 2;
	KeyFrame(double value, double time, int calculation, int type) {
		this->value = value;
		this->time = time;
		this->calculation = calculation;
		this->type = type;
	}

	double getValue() {
		return value;
	}

	double getTime() {
		return time;
	}

	double get(double time, KeyFrame* next) {
		double t = time - this->time;
		double c = value;
		double n = next->getValue();
		double d = next->getTime() - this->time;

		double out = value;

		if (n == c) 
			return n;
		if (t >= d)
			return n;
		if (t <= 0)
			return c;

		if (calculation == CALCCONST) {
			if (type == TYPEIN) {
				return n;
			} else if(type == TYPEOUT) {
				return c;
			} else {
				if (t > d / 2)
					return n;
				else return c;
			}
		}
		else if (calculation == CALCLINEAR) {
			return (n - c)*r(t / d) + c;
		}
		else if (calculation == CALCSIN) {
			if (type == TYPEINOUT) {
				double f = std::cos(r(t / d)*(3.14159265));
				return (-(n - c) / 2 * (f - 1)) + c;
			}
			else {
				double f = std::cos(r(t / d)*(3.14159265/2));
				if(type==TYPEIN)
					return (-(n - c) * f) + n;
				f = std::sin(r(t / d)*(3.14159265 / 2));
				if(type==TYPEOUT)
					return ((n - c) * f) + c;
			}
		}
		else if (calculation == CALCCUBIC) {
			if(type==TYPEIN)
				return (n - c)*(t /= d)*t*t + c;
			else if(type==TYPEOUT) 
				return (n - c)*((t = t / d - 1)*t*t + 1) + c;
			else {
				if ((t /= d / 2) < 1) return (n - c) / 2 * t*t*t + c;
				return (n - c) / 2 * ((t -= 2)*t*t + 2) + c;
			}
		}
		else if (calculation == CALCCIRC) {
			if (type == TYPEIN) {
				return -(n-c) * (sqrt(1 - (t /= d)*t) - 1) + c;
			}
			else if (type == TYPEOUT) {
				return (n-c) * sqrt(1 - (t = t / d - 1)*t) + c;
			} 
			else {
				if ((t /= d / 2) < 1) return -(n - c) / 2 * (sqrt(1 - t * t) - 1) + c;
				return (n-c) / 2 * (sqrt(1 - t * (t -= 2)) + 1) + c;
			}
		}
		else if (calculation == CALCELASTIC) {
			if (type == TYPEIN) {
				if (t == 0) return c;  
				if ((t /= d) == 1) return n;
				float p = d * .3f;
				float a = n-c;
				float s = p / 4;
				float postFix = a * pow(2, 10 * (t -= 1)); // this is a fix, again, with post-increment operators
				return -(postFix * sin((t*d - s)*(2 * 3.14159265) / p)) + c;
			} 
			else if (type == TYPEOUT) {
				t /= d;
				if (t == 0) return c;
				if (t == 1) return n;
				float p = d * .3f;
				float a = n-c;
				float s = p / 4;
				return (a*pow(2, -10 * t) * sin((t*d - s)*(2 * 3.14159265) / p) + n);
			}
			else {
				if (t == 0) return c;  if ((t /= d / 2) == 2) return c + n;
				float p = d * (.3f*1.5f);
				float a = n-c;
				float s = p / 4;

				if (t < 1) {
					float postFix = a * pow(2, 10 * (t -= 1)); // postIncrement is evil
					return -.5f*(postFix* sin((t*d - s)*(2 * 3.14159265) / p)) + c;
				}
				float postFix = a * pow(2, -10 * (t -= 1)); // postIncrement is evil
				return postFix * sin((t*d - s)*(2 * 3.14159265) / p)*.5f + n;
			}

		}

		else if (calculation == CALCBACK) {
			if (type == TYPEIN) {
				float s = 1.70158f;
				float postFix = t /= d;
				return (n-c) * (postFix)*t*((s + 1)*t - s) + c;
			}
			else if(type==TYPEOUT) {
				float s = 1.70158f;
				return (n-c) * ((t = t / d - 1)*t*((s + 1)*t + s) + 1) + c;
			}
			else {
				float s = 1.70158f;
				if ((t /= d / 2) < 1) return (n-c) / 2 * (t*t*(((s *= (1.525f)) + 1)*t - s)) + c;
				float postFix = t -= 2;
				return (n-c) / 2 * ((postFix)*t*(((s *= (1.525f)) + 1)*t + s) + 2) + c;
			}
		}
		else if (calculation == CALCBOUNCE) {
			double out = n-c;
			bool iotd = false;
			if (type == TYPEIN)
				t = d - t;
			else if (type == TYPEINOUT) {
				t *= 2;
				if (t >= d) {
					t -= d;
					iotd = true;
				}
				else {
					t = d - t;
				}
			}
			if ((t /= d) < (1 / 2.75f)) {
				out *= (7.5625f*t*t);
			}
			else if (t < (2 / 2.75f)) {
				float postFix = t -= (1.5f / 2.75f);
				out *= (7.5625f*(postFix)*t + .75f);
			}
			else if (t < (2.5 / 2.75)) {
				float postFix = t -= (2.25f / 2.75f);
				out *= (7.5625f*(postFix)*t + .9375f);
			}
			else {
				float postFix = t -= (2.625f / 2.75f);
				out *= (7.5625f*(postFix)*t + .984375f);
			}

			if (type == TYPEIN) {
				return n - out;
			}
			else if (type == TYPEOUT) {
				return out + c;
			}
			else {
				if (!iotd) return (n - out/2) - (n+c)/2 + c;
				else return out/2 + (n+c)/2;
			}
		}
	}
private:
	double value, time;
	int calculation, type;

	double r(double r) {
		return std::fmax(std::fmin(r, 1), 0);
	}
};
class ValueAnimation {
public:
	ValueAnimation(double speed=1, double length=1, std::vector<KeyFrame> frames = std::vector<KeyFrame>(), bool repeat = false) {
		value = 0;
		this->frames = frames;
		this->repeat = repeat;
		this->speed = speed;
		this->length = length;
	}
	static double getFrom(std::vector<KeyFrame> frames, double time) {
		if (frames.size() == 0)
			return 0;
		KeyFrame f = frames.at(0);
		for (KeyFrame l : frames) {
			if (l.getTime() < time)
				f = l;
			else
				return f.get(time, &l);
		}
		return f.getValue();
	}
	void tick() {
		value += speed*neutron::frame::delta_time;
		if (value > length)
			value = (repeat) ? 0:length;
	}
	double get() {
		return getFrom(frames, value);
	}
	double getTime() {
		return value;
	}
	void setTime(double value) {
		this->value = value;
	}
private:
	std::vector<KeyFrame> frames;
	bool repeat; 
	double value, speed, length;
};

class ValueAnimationSet {
public:
	ValueAnimationSet(double speed = 1, double length = 1, std::vector<std::vector<KeyFrame>> frames = std::vector<std::vector<KeyFrame>>(), bool repeat = false) {
		value = 0;
		this->speed = speed;
		this->length = length;
		this->frames = frames;
		this->repeat = repeat;
	}
	static std::vector<double> getFromSet(std::vector<std::vector<KeyFrame>> frames, double time) {
		std::vector<double> out = std::vector<double>();

		for (int i = 0; i < frames.size(); i++) {
			bool added = false;
			if (frames.at(i).size() == 0) {
				out.push_back(0);
				added = true;
			}
			KeyFrame f = frames.at(i).at(0);
			for (KeyFrame l : frames.at(i)) {
				if (l.getTime() < time) {
					f = l;
				} else if(!added){ 
					added = true;
					out.push_back(f.get(time, &l));
				}
			}
			if(!added)
				out.push_back(f.getValue());
		}
		return out;
	}
	void tick() {
		value += speed;
		if (value > length)
			value = (repeat) ? 0 : length;
	}
	std::vector<double> get() {
		return getFromSet(frames, value);
	}
	double getTime() {
		return value;
	}
	void setTime(double value) {
		this->value = value;
	}
private:
	std::vector <std::vector<KeyFrame>> frames;
	bool repeat;
	double value, speed, length;
};
static std::vector<KeyFrame> keyFrameSetFromArrays(double data[][4], int size) {
	std::vector<KeyFrame> frames;
	for (int i = 0; i < size; i++) {
		std::cout << "Timing: " << data[i][0] << " Value: " << data[i][1] << " Calc: " << data[i][2] << "Type: " << data[i][3] << std::endl;
		frames.push_back(KeyFrame(data[i][0], data[i][1], (int)data[i][2], (int)data[i][3]));
	}
	return frames;
}
#endif