#include "Applications/Liquids/Particle.h"
#include <memory>

Particle::Particle(float x, float y)
{
	position = glm::vec2(x, y);
	previousPosition = glm::vec2(x, y);
	velocity = glm::vec2(0, 0);
	density = 0;
	nearDensity = 0;
	pressure = 0;
	nearPressure = 0;
	memset(neighbors, -1, sizeof(int) * MAX_NEIGHBORS);
}
Cell::Cell(int x, int y)
{
	this->n = 0;
	this->x = x;
	this->y = y;

	memset(indicies, -1, MAXPINC);
}
Bound::Bound(float x1, float x2, float y1, float y2)
{
	xLimits.x = x1; xLimits.y = x2;
	yLimits.x = y1; yLimits.y = y2;
}