#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#define MAX_NEIGHBORS 8
struct Particle 
{
	glm::vec2 position;
	glm::vec2 previousPosition;
	glm::vec2 velocity;
	float density;
	float nearDensity;
	float pressure;
	float nearPressure;
	int neighbors[MAX_NEIGHBORS];

	Particle(float x, float y);
};