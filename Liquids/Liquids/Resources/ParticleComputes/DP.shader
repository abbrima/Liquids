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

#define PARTICLE_STIFFNESS 2000
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

	// compute density
	float density_sum = 0.f;
	for (int j = 0; j < nParticles; j++)
	{
		vec2 delta = particles[i].position - particles[j].position;
		float r = length(delta);
		if (r < SMOOTHING_LENGTH)
		{
			density_sum += PARTICLE_MASS * /* poly6 kernel */ 315.f * pow(SMOOTHING_LENGTH * SMOOTHING_LENGTH - r * r, 3) / (64.f * PI_FLOAT * pow(SMOOTHING_LENGTH, 9));
		}
	}
	particles[i].density = density_sum;
	// compute pressure
	particles[i].pressure = max(PARTICLE_STIFFNESS * (density_sum - PARTICLE_RESTING_DENSITY), 0.f);
}