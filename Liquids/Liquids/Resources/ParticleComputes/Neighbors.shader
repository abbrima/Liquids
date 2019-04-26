#shader compute
#version 430 core

#define MAX_NEIGHBORS 8
#define p particles[gl_GlobalInvocationID.x]
#define LOCALX 1024

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


float kernel(vec2 p1, vec2 p2);

void main()
{
	int index = 0;
	for (int i = 0;i< nParticles; i++)
	{
		float a = kernel(p.position, particles[i].position);
		if (a > 0 && a < 1 && i != gl_GlobalInvocationID.x && index<8)
			p.neighbors[index++] = i;
	}
	while (index < 8)
		p.neighbors[index++] = -1;

};
float kernel(vec2 p1, vec2 p2)
{
	float distance = length(p1 - p2);
	return 1 - (distance / k);
}