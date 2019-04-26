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
uniform float deltaTime;
float kernel(float distance);

void main()
{
	vec2 pDisplace;
	int index = 0;
	float a;
	vec2 displacement;
	vec2 v;
	while (index < MAX_NEIGHBORS && p.neighbors[index] != -1)
	{
		v = particles[p.neighbors[index]].position - p.position;
		a = kernel(v.length());
		displacement = normalize(v) * deltaTime * deltaTime * (p.pressure*a + p.nearPressure*a*a);
		particles[p.neighbors[index]].position += displacement * 0.5;
		pDisplace -= displacement * 0.5;
		index++;
	}
	p.position += pDisplace;
};

float kernel(float distance)
{
	return 1 - (distance / k);
}