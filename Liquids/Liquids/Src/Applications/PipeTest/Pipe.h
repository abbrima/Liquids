#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#define e 2.718281f

struct Pipe{
	float lowX, lowY, highX, highY, base, dY, dX, power, dampeningRatio;
	Pipe(float base, float power, float dx, float dy, float dr);
	void setConstraints(float lowX, float lowY, float highX, float highY);
	float f(float x);
};

