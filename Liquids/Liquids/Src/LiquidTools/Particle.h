#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#define MAX_PARTICLES 100000
#define PARTICLE_DISPATCH_SIZE 128
#define SPH_PARTICLE_RADIUS 0.005f
class Particle {
public:
	
};
class NormalParticle : public Particle
{
public:
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 force;
	float density;
	float pressure;

	NormalParticle();
	NormalParticle(const glm::vec2& pos);
	NormalParticle(const glm::vec2& pos, const float& offset);
	NormalParticle(const glm::vec2& pos, const glm::vec2& vel);
};
class AdvancedParticle : public Particle
{
public:

};