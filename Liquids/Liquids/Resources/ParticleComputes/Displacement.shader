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


void main()
{
	vec2 pDisplace = vec2(0, 0),displacement;
	vec2 v; float a;
	int index = 0;
	while (p.neighbors[index] != -1 && index < MAX_NEIGHBORS) {
		v = np.position - p.position;
		a = 1 - (v.length() / k);
		displacement = deltaTime * deltaTime * (p.pressure * a + p.nearPressure * a * a) * normalize(v);
		np.position += displacement / 2;
		pDisplace -= displacement / 2;
		index++;
	}
	barrier();
	p.position += pDisplace;
};

