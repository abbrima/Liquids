#shader compute
#version 460 core

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


void main()
{
	uint i = gl_GlobalInvocationID.x;

	// integrate
	vec3 acceleration = particles[i].force.xyz / particles[i].dp.x;
	vec3 new_velocity = particles[i].velocity.xyz + TIME_STEP * acceleration;
	vec3 new_position = particles[i].position.xyz + TIME_STEP * new_velocity;

	// boundary conditions
	if (new_position.x < -0.3f)
	{
		new_position.x = -0.3f;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	 if (new_position.x > 0.3f)
	{
		new_position.x = 0.3f;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	 if (new_position.y < -0.3f)
	{
		new_position.y = -0.3f;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	 if (new_position.y > 0.3f)
	{
		new_position.y = 0.3f;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	 if (new_position.z < -0.3f)
	{
		new_position.z = -0.3f;
		new_velocity.z *= -1 * WALL_DAMPING;
	}
	 if (new_position.z > 0.3f)
	{
		new_position.z = 0.3f;
		new_velocity.z *= -1 * WALL_DAMPING;
	}
	
	particles[i].velocity = vec4(new_velocity,0);
	particles[i].position = vec4(new_position,0);
}
