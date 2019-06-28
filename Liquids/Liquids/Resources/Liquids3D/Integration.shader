#shader compute
#version 430 core

#external

layout(local_size_x = WORK_GROUP_SIZE) in;

// constants

#define TIME_STEP 0.0001f
#define WALL_DAMPING 0.3f
struct Particle {
	vec4 position;
	vec4 velocity;
	vec4 force;
	vec4 dp;
};

layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};

shared Particle ps[WORK_GROUP_SIZE];

void main()
{
	uint i = gl_LocalInvocationID.x;

	ps[i] = particles[gl_GlobalInvocationID.x];

	// integrate
	vec3 acceleration = ps[i].force.xyz / ps[i].dp.x;
	vec3 new_velocity = ps[i].velocity.xyz + TIME_STEP * acceleration;
	vec3 new_position = ps[i].position.xyz + TIME_STEP * new_velocity;

	// boundary conditions
	if (new_position.x < -0.2f)
	{
		new_position.x = -0.2f;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	 if (new_position.x > 0.2f)
	{
		new_position.x = 0.2f;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	 if (new_position.y < -0.2f)
	{
		new_position.y = -0.2f;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	 if (new_position.y > 0.2f)
	{
		new_position.y = 0.2f;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	 if (new_position.z < -0.2f)
	{
		new_position.z = -0.2f;
		new_velocity.z *= -1 * WALL_DAMPING;
	}
	 if (new_position.z > 0.2f)
	{
		new_position.z = 0.2f;
		new_velocity.z *= -1 * WALL_DAMPING;
	}
	
	ps[i].velocity = vec4(new_velocity,0);
	ps[i].position = vec4(new_position,0);

	particles[gl_GlobalInvocationID.x] = ps[i];
}
