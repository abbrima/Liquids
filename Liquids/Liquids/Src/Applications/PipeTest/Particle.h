#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#define MAX_NEIGHBORS 32
struct Particle {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 force;
	float density;
	float pressure;

	Particle(glm::vec2 pos);
	Particle(glm::vec2 pos, glm::vec2 vel);
};