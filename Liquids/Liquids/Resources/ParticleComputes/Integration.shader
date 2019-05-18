#shader compute
#version 430 core

#define p particles[gl_GlobalInvocationID.x]
#define LOCALX 128

struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 pForce;
	vec2 vForce;
	float density;
	float pressure;
	float mass;
	float life;
};

layout(local_size_x = LOCALX, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 2) buffer Data
{
	Particle particles[];
};

#define dt 0.016f
#define dr 0.3f
void main()
{
	
	
	p.velocity += dt * (10*p.pForce / p.density ) * 0.1f;
	p.position += dt * p.velocity;

	if (p.position.y < -100)
	{
		p.velocity = reflect(p.velocity, vec2(0, 1)) * dr;
	}
	if (p.position.x < -100)
	{
		p.velocity = reflect(p.velocity, vec2(1, 0)) * dr;
	}
	if (p.position.x > 100)
	{
		p.velocity = reflect(p.velocity, vec2(-1, 0)) * dr;
	}
}
