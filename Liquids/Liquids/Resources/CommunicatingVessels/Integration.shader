#shader compute
#version 430 core

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
	float x;
	float lowY;
	float highY;
	float dr;
};
layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};
layout(std430, binding = 1) buffer Pipes
{
	Pipe pipes[];
};

uniform uint nPipes;

void main()
{
	uint i = gl_GlobalInvocationID.x;

	// integrate
	vec2 acceleration = particles[i].force / particles[i].density;
	vec2 new_velocity = particles[i].velocity + TIME_STEP * acceleration;
	vec2 new_position = particles[i].position + TIME_STEP * new_velocity;

	// boundary conditions
	if (new_position.x < -0.3)
	{
		new_position.x = -0.3;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
    if (new_position.x > 0.3)
	{
		new_position.x = 0.3;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	if (new_position.y < -1)
	{
		new_position.y = -1;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	if (new_position.y > 1)
	{
		new_position.y = 1;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	{
		for (int i = 0; i < nPipes; i++)
		{
			if (new_position.y <= pipes[i].highY && new_position.y >= pipes[i].lowY)
			{

				float diff = new_position.x - pipes[i].x;
				if (diff > 0.f && abs(diff)<0.006f) {
					if (new_velocity.x < 0.f)
					{
						new_position.x = pipes[i].x + 0.006f;
						new_velocity = reflect(new_velocity, vec2(1.0f, 0.0f)) * pipes[i].dr;
					}
				}
				else if (diff < 0.f && abs(diff)<0.006f) {
					if (new_velocity.x > 0.f)
					{
						new_position.x = pipes[i].x - 0.006f;
						new_velocity = reflect(new_velocity, vec2(-1.0f, 0.0f)) * pipes[i].dr;
					}
				}
			}
		}
	}
	
	particles[i].velocity = new_velocity;
	particles[i].position = new_position;
}
