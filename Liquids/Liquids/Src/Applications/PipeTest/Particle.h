#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

struct Particle {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 pForce;
	glm::vec2 vForce;
	float density;
	float pressure;
	float mass;
	float life;

	Particle(glm::vec2 pos, float m);
	Particle(glm::vec2 pos, glm::vec2 vel, float m);
};