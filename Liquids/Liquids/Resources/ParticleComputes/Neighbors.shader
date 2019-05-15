#shader compute
#version 430 core

#define MAX_NEIGHBORS 32
#define p particles[gl_GlobalInvocationID.x]
#define LOCALX 128

struct Particle {
	vec2 position;
	vec2 prevPosition;
	vec2 velocity;
	float density;
	float nearDensity;
	float pressure;
	float nearPressure;
	int neighbors[MAX_NEIGHBORS];
};

layout(local_size_x = LOCALX, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 2) buffer Data
{
	Particle particles[];
};

uniform float k;
uniform int nParticles;


void main()
{
	int index = 0;
	for (uint i = 0; i < nParticles; i++)
	{
		if (index >= MAX_NEIGHBORS)
			break;
		if (i == gl_GlobalInvocationID.x)
			continue;
		float d = length(p.position - particles[i].position);
		if (d <= k)
		{
			p.neighbors[index] = int(i);
			index++;
		}
	}
	while (index < MAX_NEIGHBORS)
	{
		p.neighbors[index] = -1;
		index++;
	}
};