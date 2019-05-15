#shader compute
#version 430 core

#define MAX_NEIGHBORS 32
#define p particles[gl_GlobalInvocationID.x]
#define np particles[p.neighbors[index]]
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

#define dampeningFactor 0.3f

void main()
{
	if (p.position.x < -1)
	{
		p.position.x = -1;
		p.velocity = reflect(p.velocity, vec2(1, 0)) * dampeningFactor;
	}
	if (p.position.x > 1)
	{
		p.position.x = 1;
		p.velocity = reflect(p.velocity, vec2(-1, 0)) * dampeningFactor;
	}
	if (p.position.y > 1)
	{
		p.position.y = 1;
		p.velocity = reflect(p.velocity, vec2(0, -1)) * dampeningFactor;
	}
	if (p.position.y < -1)
	{
		p.position.y = -1;
		p.velocity = reflect(p.velocity, vec2(0, 1)) * dampeningFactor;
	}
};

