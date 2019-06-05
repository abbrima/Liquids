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
	vec2 color1;
	vec2 color2;
	float density;
	float pressure;
	float pr;
	float k;
	float mass;
	float viscosity;
};

layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};


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
	
	particles[i].velocity = new_velocity;
	particles[i].position = new_position;
}