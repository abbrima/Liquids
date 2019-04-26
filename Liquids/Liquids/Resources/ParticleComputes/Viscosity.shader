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
uniform float linVis;
uniform float quadVis;
float kernel(float distance);

void main()
{
	int index = 0;
	vec2 v;
	while (index < 8 && p.neighbors[index] != -1)
	{
		v = particles[p.neighbors[index]].position - p.position;
		float u = dot(p.velocity - particles[p.neighbors[index]].velocity, normalize(v));
		if (u > 0.0)
		{
			float a = kernel(v.length());
			vec2 impulse = deltaTime * a * normalize(v) * (linVis * u + quadVis * u * u);
			p.velocity -= impulse * 0.5;
			particles[p.neighbors[index]].velocity += impulse * 0.5;
		}
		index++;
	}
};

float kernel(float distance)
{
	return 1 - (distance / k);
}