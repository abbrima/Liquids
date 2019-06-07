#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "OpenGL/VertexBufferLayout.h"

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
	
	static VertexBufferLayout GetLayout();
};

enum LiquidType {
	Water,Oil,Blood
};

class AdvancedParticle : public Particle
{
public:
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 force;
	glm::vec4 color;
	float density;
	float pressure;
	float pr;
	float k;
	float mass;
	float viscosity;

	AdvancedParticle();
	AdvancedParticle(const glm::vec2& pos,const LiquidType& type);
	AdvancedParticle(const glm::vec2& pos, const float& offset, const LiquidType& type);
	AdvancedParticle(const glm::vec2& pos, const glm::vec2& vel, const LiquidType& type);
	void ImGuiRender();

	static VertexBufferLayout GetLayout();
	static void SetType(AdvancedParticle& particle, const LiquidType& type);
};
class Particle3D : public Particle {
public:
	glm::vec4 position;
	glm::vec4 velocity;
	glm::vec4 force;
	float density;
	float pressure;
	float padding[2];

	Particle3D();
	Particle3D(const glm::vec3& pos);
	Particle3D(const glm::vec3& pos, const float& offset);
	Particle3D(const glm::vec3& pos, const glm::vec3& vel);

	static VertexBufferLayout GetLayout();
};