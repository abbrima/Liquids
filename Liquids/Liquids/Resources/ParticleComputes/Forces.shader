#shader compute
#version 430 core

#define WORK_GROUP_SIZE 128

layout(local_size_x = WORK_GROUP_SIZE) in;

// constants
uniform int nParticles;

#define PI_FLOAT 3.1415927410125732421875f
#define PARTICLE_RADIUS 0.005f
#define PARTICLE_RESTING_DENSITY 1000
// Mass = Density * Volume
#define PARTICLE_MASS 0.02
#define SMOOTHING_LENGTH (4 * PARTICLE_RADIUS)

#define PARTICLE_VISCOSITY 3000.f

// OpenGL y-axis is pointing up, while Vulkan y-axis is pointing down.
// So in OpenGL this is negative, but in Vulkan this is positive.
#define GRAVITY_FORCE vec2(0, -9806.65)
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
		if (r < SMOOTHING_LENGTH)
		{
			pressure_force -= PARTICLE_MASS * (particles[i].pressure + particles[j].pressure) / (2.f * particles[j].density) *
				// gradient of spiky kernel
				-45.f / (PI_FLOAT * pow(SMOOTHING_LENGTH, 6)) * pow(SMOOTHING_LENGTH - r, 2) * normalize(delta);
			viscosity_force += PARTICLE_MASS * (particles[j].velocity - particles[i].velocity) / particles[j].density *
				// Laplacian of viscosity kernel
				45.f / (PI_FLOAT * pow(SMOOTHING_LENGTH, 6)) * (SMOOTHING_LENGTH - r);
		}
	}
	viscosity_force *= PARTICLE_VISCOSITY;
	vec2 external_force = particles[i].density * GRAVITY_FORCE;

	particles[i].force = pressure_force + viscosity_force + external_force;
}