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
uniform float deltaTime;
uniform float linVis;
uniform float quadVis;

float kernel(float distance);

void main()
{
	for (uint index = 0; index < MAX_NEIGHBORS; index++)
	{
		if (p.neighbors[index] == -1)
			break;
		vec2 v = np.position - p.position;
		if (length(v) > 0)
		{
			float u = dot(p.velocity - np.velocity, normalize(v));
			if (u > 0.f && u<1.f)
			{
				float a = kernel(length(v));
				vec2 impulse = deltaTime * a * (linVis*u + quadVis * pow(u, 2)) * normalize(v);
				p.velocity -= impulse * 0.5f;
				np.velocity += impulse * 0.5f;
			}
		}
	}
};

float kernel(float distance) {
	return 1 - (distance / k);
};
