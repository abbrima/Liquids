#include "Particle.h"
#include <memory>
#include <random>

NormalParticle::NormalParticle()
{
	memset(this, 0, sizeof(NormalParticle));
}
NormalParticle::NormalParticle(const glm::vec2& pos)
{
	memset(this, 0, sizeof(Particle));
	position = pos;
}
NormalParticle::NormalParticle(const glm::vec2& pos, const float& offset) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	memset(this, 0, sizeof(NormalParticle));
	position = glm::vec2(pos.x + xo, pos.y + yo);
}
NormalParticle::NormalParticle(const glm::vec2& pos,const glm::vec2& vel)
{
	memset(this, 0, sizeof(NormalParticle));
	position = pos;
	velocity = vel;
}
