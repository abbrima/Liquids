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
struct Pipe {
	float lowX, lowY, highX, highY, base, dY, dX, power, dampeningRatio;
};

layout(local_size_x = LOCALX, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 2) buffer Data
{
	Particle particles[];
};
layout(std430, binding = 3) buffer Pipes
{
	Pipe pipes[];
};

#define e 2.718281f
#define dampeningFactor 0.f

uniform int nPipes;

float f(Pipe pipe,float x);
float slopeNormal(Pipe pipe, float x);
vec2 getNormal(vec2 velocity, float m);

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

	for (int i = 0; i < nPipes; i++)
	{
		float y = f(pipes[i],p.position.x);
		if (abs(y - p.position.y) <= 0.06)
		{
			float m = slopeNormal(pipes[i], p.position.x);
			vec2 norm = getNormal(p.velocity, m);
			p.velocity = reflect(p.velocity, norm) * pipes[i].dampeningRatio;
		}
	}
};

float f(Pipe pipe,float x) {
	return (pipe.base * pow(e, (x + pipe.dX)*pipe.power)) + pipe.dY;
}
float slopeNormal(Pipe pipe, float x) {
	float m = pipe.base * pipe.power * pow(e,pipe.power*(x+pipe.dX));
	m = -1 / m;
	return m;
}
vec2 getNormal(vec2 velocity, float m)
{
	float angle = atan(m);
	vec2 norm = vec2(cos(angle),sin(angle));
	velocity = normalize(velocity);
	if (dot(norm, velocity) >= 0)
		return norm;
	else
		return norm * -1;
}