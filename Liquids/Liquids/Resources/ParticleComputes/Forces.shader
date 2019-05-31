#shader compute
#version 430 core

#external

layout(local_size_x = WORK_GROUP_SIZE) in;

// constants
uniform int nParticles;
#define PI_FLOAT 3.1415927410125732421875f
uniform float mass;
uniform float viscosity;
uniform vec2 gravity;

struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	float density;
	float pressure;
};
layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};
layout(std140, binding = 2) uniform Constants
{
float h;
float poly6;
float gradspiky;
float laplacianvis;
};

void main()
{
	uint i = gl_GlobalInvocationID.x;
	// compute all forces
	vec2 pressure_force = vec2(0, 0);
	vec2 viscosity_force = vec2(0, 0);

	for (uint j = 0; j < nParticles; j++)
	{
		if (i == j)
		{
			continue;
		}
		vec2 delta = particles[i].position - particles[j].position;
		float r = length(delta);
		if (r < h)
		{
			pressure_force -= mass * (particles[i].pressure + particles[j].pressure) / (2.f * particles[j].density) *
				// gradient of spiky kernel
				gradspiky * pow(h - r, 2) * normalize(delta);
			
			viscosity_force += mass * (particles[j].velocity - particles[i].velocity) / particles[j].density *
				// Laplacian of viscosity kernel
				laplacianvis * (h - r);
		}
	}
	viscosity_force *= viscosity;
	vec2 external_force = particles[i].density * gravity;

	particles[i].force = pressure_force + viscosity_force + external_force;
};