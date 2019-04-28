#shader compute
#version 430 core

#define MAX_NEIGHBORS 128
#define p particles[gl_GlobalInvocationID.x]
#define o outputs[gl_GlobalInvocationID.x]
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
struct Output {
	float deltaTime;
	float gravity;
};

layout(local_size_x = LOCALX, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 2) buffer Data
{
	Particle particles[];
};


uniform float gravity;
uniform float deltaTime;

void main()
{
	p.velocity.y -= deltaTime * gravity;
}