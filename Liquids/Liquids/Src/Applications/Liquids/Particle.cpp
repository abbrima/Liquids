#include "Applications/Liquids/Particle.h"
#include <memory>

Particle::Particle(float x, float y)
{
	position = glm::vec2(x, y);
	memset(&previousPosition, 0, (2 * sizeof(glm::vec2)) + (4 * sizeof(float)));
	memset(neighbors, -1, sizeof(int) * MAX_NEIGHBORS);
}
Particle::Particle(float x, float y, float vx, float vy)
{
	position = glm::vec2(x, y);
	previousPosition = glm::vec2(0, 0);
	velocity = glm::vec2(vx, vy);
	memset(&density, 0, 4 * sizeof(float));
	memset(neighbors, -1, sizeof(int) * MAX_NEIGHBORS);
}