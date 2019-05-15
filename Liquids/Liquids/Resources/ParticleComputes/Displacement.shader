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

float kernel(vec2 v);

void main()
{
	vec2 pDisplace = vec2(0, 0);
	for (uint index = 0; index < MAX_NEIGHBORS; index++)
	{
		if (p.neighbors[index] == -1)
			break;
		vec2 v = np.position - p.position;
		float a = kernel(v);
		if (a < 1 && a >= 0)
		{
			vec2 dis = pow(deltaTime, 2) * (p.pressure * a + p.nearPressure * pow(a, 2)) * normalize(v);
			np.position += dis * 0.5;
			pDisplace -= dis * 0.5;
		}
	}
	p.position += pDisplace;
};

float kernel(vec2 v) {
	return 1 - (length(v) / k);
}