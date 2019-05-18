#include "Applications/PipeTest/Particle.h"
#include <memory>
#include <random>

Particle::Particle(glm::vec2 pos, float m)
{
	memset(this, 0, sizeof(Particle));
	position = pos;
	mass = (m-0.1*m) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ((m + 0.1*m) - (m - 0.1*m))));
}
Particle::Particle(glm::vec2 pos, glm::vec2 vel, float m)
{
	Particle(pos, m);
	velocity = vel;
}