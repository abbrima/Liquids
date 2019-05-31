#include "Applications/PipeTest/Particle.h"
#include <memory>
#include <random>

Particle::Particle()
{
	memset(this, 0, sizeof(Particle));
}
Particle::Particle(const glm::vec2& pos)
{
	memset(this, 0, sizeof(Particle));
	position = pos;
}
Particle::Particle(const glm::vec2& pos, const float& offset) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	memset(this, 0, sizeof(Particle));
	position = glm::vec2(pos.x + xo, pos.y + yo);
}
Particle::Particle(const glm::vec2& pos,const glm::vec2& vel)
{
	memset(this, 0, sizeof(Particle));
	position = pos;
	velocity = vel;
}
