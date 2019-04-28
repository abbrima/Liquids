#shader compute
#version 430 core

#define MAX_NEIGHBORS 128
#define p particles[gl_GlobalInvocationID.x]
#define np particles[p.neighbors[index]]
#define LOCALX 64

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



void main()
{
	if (p.position.x <= -200)
	{
		p.velocity.x *= -0.001; p.position.x = -200;
	}
	if (p.position.x >= 200)
	{
		p.velocity.x *= -0.001; p.position.x = 200;
	}
	if (p.position.y <= -300)
	{
		p.velocity.y *= -0.001; p.position.y = -300;
	}
};

