#shader compute
#version 430 core

#define MAX_NEIGHBORS 128
#define p particles[gl_GlobalInvocationID.x]
#define np particles[p.neighbors[index]]
#define LOCALX 64

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

layout(local_size_x = LOCALX, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 2) buffer Data
{
	Particle particles[];
};

uniform float k;
uniform float restDensity;
uniform float stiffness;
uniform float nearStiffness;

float kernel(float distance);

void main()
{
	float density = 0, nearDensity = 0;
	int index = 0;
	while (p.neighbors[index] != -1 && index < MAX_NEIGHBORS)
	{
		float a = kernel(length(p.position - np.position));
		density += a * a;
		nearDensity += a * a*a;
		index++;
	}
	p.density = density;
	p.nearDensity = nearDensity;
	p.pressure = (p.density - restDensity)*stiffness;
	p.nearPressure = p.nearDensity * nearStiffness;
}
float kernel(float distance)
{
	return 1 - (distance / k);
}