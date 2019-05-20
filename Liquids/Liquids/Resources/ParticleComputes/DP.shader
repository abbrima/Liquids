#shader compute
#version 430 core

#define WORK_GROUP_SIZE 128

layout(local_size_x = WORK_GROUP_SIZE) in;


// constants
uniform int nParticles;

#define PI_FLOAT 3.1415927410125732421875f
uniform float pRadius; //0.005f
uniform float pr;
// Mass = Density * Volume
uniform float mass;

#define SMOOTHING_LENGTH (4 * pRadius)

uniform float k;
struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	float density;
	float pressure;
};

layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};

void main()
{
	uint i = gl_GlobalInvocationID.x;

	float density_sum = 0.f;
	for (int j = 0; j < nParticles; j++)
	{
		vec2 delta = particles[i].position - particles[j].position;
		float r = length(delta);
		if (r < SMOOTHING_LENGTH)
		{
			density_sum += mass * 315.f * pow(SMOOTHING_LENGTH * SMOOTHING_LENGTH - r * r, 3) / (64.f * PI_FLOAT * pow(SMOOTHING_LENGTH, 9));
		}
	}
	particles[i].density = density_sum;
	particles[i].pressure = max(k * (density_sum - pr), 0.f);
}