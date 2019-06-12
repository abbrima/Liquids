#pragma once

#define MAX_PIPES 20

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/constants.hpp"

struct Pipe{
	float lowX, lowY, highX, highY, base, dY, dX, power, dampeningRatio;
	int upper;
	Pipe(const float& base,const float& power,const float& dx,const float& dy,const float& dr,const bool& upper);
	void setConstraints(const float& lowX,const float& lowY,const float& highX,const float& highY);
	float f(const float& x);
};
struct VerticalPipe {
	float x; float lowY; float highY; float dampeningRatio;

	VerticalPipe(const float& x, const float& lowy, const float& highy, const float& dampeningRatio);
};

struct SinusoidalPipe {
	float lowX, lowY, highX, highY, base, dY, dX;
	int xPower;
	float xCo, dampeningRatio;
	int upper;
	SinusoidalPipe(const float& base, const int& power, const float& xCo, const float& dx, const float& dy, const float& dr, const bool& upper);
	void setConstraints(const float& lowX, const float& lowY, const float& highX, const float& highY);
	float f(const float& x);
};
