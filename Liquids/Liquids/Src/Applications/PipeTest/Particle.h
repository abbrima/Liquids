#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#define MAX_PARTICLES 100000
#define PARTICLE_DISPATCH_SIZE 128
#define MAX_NEIGHBORS 32
#define SPH_PARTICLE_RADIUS 0.005f
struct Particle {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 force;
	float density;
	float pressure;

	Particle();
	Particle(const glm::vec2& pos);
	Particle(const glm::vec2& pos, const float& offset);
	Particle(const glm::vec2& pos, const glm::vec2& vel);
};