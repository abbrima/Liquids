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
uniform float stiffness;
uniform float nearStiffness;
uniform float restDensity;

float kernel(vec2 p1, vec2 p2);

void main()
{
	float density = 0, nearDensity = 0;
	uint index = 0;
	while (index < MAX_NEIGHBORS && p.neighbors[index] != -1)
	{
		float a = kernel(p.position, particles[p.neighbors[index]].position);
		density += a * a;
		nearDensity += a * a * a;
		index++;
	}
	p.density = density; p.nearDensity = nearDensity;
	p.pressure = (p.density - restDensity)*stiffness;
	p.nearPressure = p.nearDensity * nearStiffness;
};
float kernel(vec2 p1, vec2 p2)
{
	float distance = length(p1 - p2);
	return 1 - (distance / k);
}