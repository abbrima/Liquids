#shader compute
#version 430 core

#define p particles[gl_GlobalInvocationID.x]
#define np particles[index]
#define LOCALX 128
#define h 10*radius
#define PI 3.1415f

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


uniform float radius;
uniform float k;
uniform float pr;
uniform int nParticles;


const float poly6 = (315.0f / (64.0f * PI * pow(h,9)));

void main()
{
	float den = 0.f;
	for (uint index = 0; index < nParticles; index++)
	{
		vec2 diff = p.position - np.position;
		float r = length(diff);
		if (r < h)
			den += np.mass * poly6 * pow(h*h - r * r, 3);
	}
	//p.density = max(den, pr);
	p.density = den;
	p.pressure = k * (p.density - pr);
};