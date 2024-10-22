#shader compute
#version 460 core

#external

layout(local_size_x = WORK_GROUP_SIZE) in;

// constants

#define TIME_STEP 0.0001f
#define WALL_DAMPING 0.3f
struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	float density;
	float pressure;
};
struct Pipe {
	float lowX, lowY, highX, highY, base, dY, dX, power, dampeningRatio;
	bool upper;
};
layout(std430, binding = 1) buffer Pipes
{
	Pipe pipes[];
};
#define e 2.718281f

layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};

uniform int nPipes;

float f(in Pipe pipe,in float x);
float slopeNormal(in Pipe pipe,in float x);
vec2 getNormal(in vec2 velocity,in float m);

void main()
{
	uint i = gl_GlobalInvocationID.x;

	// integrate
	vec2 acceleration = particles[i].force / particles[i].density;
	vec2 new_velocity = particles[i].velocity + TIME_STEP * acceleration;
	vec2 new_position = particles[i].position + TIME_STEP * new_velocity;

	// boundary conditions
	if (new_position.x < -1)
	{
		new_position.x = -1;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	else if (new_position.x > 1)
	{
		new_position.x = 1;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	else if (new_position.y < -1)
	{
		new_position.y = -1;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	else if (new_position.y > 1)
	{
		new_position.y = 1;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	else {
		for (int j = 0; j < nPipes; j++)
		{
			float y = f(pipes[j], new_position.x);
			if (pipes[j].upper && new_position.y > y)
			{
				float m = slopeNormal(pipes[j], new_position.x);
				vec2 norm = getNormal(new_velocity, m);
				new_position.y = y;
				new_velocity = reflect(new_velocity, norm) * pipes[j].dampeningRatio;
			}
			else if (!pipes[j].upper && new_position.y < y)
			{
				float m = slopeNormal(pipes[j], new_position.x);
				vec2 norm = getNormal(new_velocity, m);
				new_position.y = y;
				new_velocity = reflect(new_velocity, norm) * pipes[j].dampeningRatio;
			}
		}
	}

	particles[i].velocity = new_velocity;
	particles[i].position = new_position;
}


float f(in Pipe pipe, in float x) {
	return (pipe.base * pow(e, (x + pipe.dX)*pipe.power)) + pipe.dY;
}
float slopeNormal(in Pipe pipe,in float x) {
	float m = pipe.base * pipe.power * pow(e, pipe.power*(x + pipe.dX));
	m = -1 / m;
	return m;
}
vec2 getNormal(in vec2 velocity,in float m)
{
	float angle = atan(m);
	vec2 norm = vec2(cos(angle), sin(angle));
	velocity = normalize(velocity);
	if (dot(norm, velocity) >= 0)
		return norm;
	else
		return norm * -1;
}