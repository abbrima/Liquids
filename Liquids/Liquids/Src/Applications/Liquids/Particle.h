#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#define MAX_NEIGHBORS 8
#define MAXPINC 1000
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
struct Cell
{
	int n;
	int x;
	int y;
	int indicies[MAXPINC];

	Cell(int x, int y);
};
struct Bound
{
	glm::vec2 xLimits;
	glm::vec2 yLimits;

	Bound(float x1, float x2, float y1, float y2);
};