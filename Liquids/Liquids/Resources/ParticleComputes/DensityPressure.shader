#shader compute
#version 430 core

#define MAX_NEIGHBORS 32
#define p particles[gl_GlobalInvocationID.x]
#define np particles[p.neighbors[index]]
#define LOCALX 128

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

float kernel(vec2 p1,vec2 p2);

void main()
{
	float density = 0.f,nearDensity=0.f;
	for (uint index = 0; index < MAX_NEIGHBORS; index++)
	{
		if (p.neighbors[index] == -1)
			break;
		float a = kernel(p.position, np.position);
		density += a * a;
		nearDensity += a * a*a;
	}
	p.density = density; p.nearDensity = nearDensity;

	//try taking positive pressures only
	p.pressure = stiffness * (density - restDensity);
	p.nearPressure = nearStiffness * nearDensity;
}
float kernel(vec2 p1,vec2 p2)
{
	float distance = length(p1 - p2);
	return 1 - (distance / k);
}