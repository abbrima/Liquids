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
