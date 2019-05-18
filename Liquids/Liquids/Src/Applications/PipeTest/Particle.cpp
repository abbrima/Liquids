#include "Applications/PipeTest/Particle.h"
#include <memory>
#include <random>


Particle::Particle(glm::vec2 pos)
{
	memset(this, 0, sizeof(Particle));
	position = pos;
}
Particle::Particle(glm::vec2 pos, glm::vec2 vel)
{
	memset(this, 0, sizeof(Particle));
	position = pos;
	velocity = vel;
}