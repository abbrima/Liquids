#pragma once

#define MAX_PIPES 20

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/constants.hpp"

struct Pipe{
	float lowX, lowY, highX, highY, base, dY, dX, power, dampeningRatio;
	Pipe(const float& base,const float& power,const float& dx,const float& dy,const float& dr);
	void setConstraints(const float& lowX,const float& lowY,const float& highX,const float& highY);
	float f(const float& x);
};

