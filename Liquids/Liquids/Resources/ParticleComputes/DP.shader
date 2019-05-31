#shader compute
#version 430 core

#external

layout(local_size_x = WORK_GROUP_SIZE) in;


uniform int nParticles;
#define PI_FLOAT 3.1415927410125732421875f
uniform float pr;
uniform float mass;
uniform float k;
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

	float density_sum = 0.f;
	for (int j = 0; j < nParticles; j++)
	{
		vec2 delta = particles[i].position - particles[j].position;
		float r = length(delta);
		if (r < h)
		{
			density_sum += mass *  pow(h * h - r * r, 3) * poly6;
		}
	}
	particles[i].density = density_sum;
	particles[i].pressure = max(k * (density_sum - pr), 0.f);
};