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
uniform float deltaTime;
uniform float linVis;
uniform float quadVis;

float kernel(float distance);

void main()
{
	int index = 0;
	vec2 rij, impulse;
	float u, a;
	while (p.neighbors[index] != -1 && index < MAX_NEIGHBORS)
	{
		rij = np.position - p.position;
		u = dot(p.velocity - np.velocity, normalize(rij));
		if (u > 0.0)
		{
			a = kernel(rij.length());
			if (a > 0)
			{
				impulse = deltaTime * a * (linVis * u + quadVis * u * u) * normalize(rij);
				p.velocity -= impulse / 2; np.velocity += impulse / 2;
			}
		}
		
		index++;
	}
};

float kernel(float distance) {
	return 1 - (distance / k);
};
