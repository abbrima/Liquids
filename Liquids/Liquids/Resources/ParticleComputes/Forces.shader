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
uniform int nParticles;
uniform float viscosity;

const float spiky = (-45.f / (PI * pow(h,6)));
float visKernel(float r);

void main()
{
	vec2 pForce = vec2(0, 0);
	vec2 vForce = vec2(0, 0);
	for (uint index = 0; index < nParticles; index++)
	{
		if (index == gl_GlobalInvocationID.x)
			continue;
		vec2 diff = p.position - np.position;
		float r = length(diff);
		if (r < h && r>0)
		{
			pForce += (np.mass / p.mass)*((p.pressure + np.pressure) / (2 * p.density * np.density)) 
				* spiky * pow(h - r, 2) * normalize(diff);
			vForce += (np.mass / p.mass)*(1.0f / np.density)*(np.velocity - p.velocity) * visKernel(r) * normalize(diff);
		}
	}
	pForce *= -1;
	vForce *= viscosity;
	p.pForce = pForce;
	//p.vForce = vForce;
}

float visKernel(float r)
{
	return (pow(r, 3) / -2 * pow(h, 3)) + pow(r / h, 2) + (h / (2.f*r)) - 1;
}