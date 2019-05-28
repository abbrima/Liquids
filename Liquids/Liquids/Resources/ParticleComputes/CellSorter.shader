#shader compute
#version 430 core

#define MAX_PINC 1000
#define p particles[gl_GlobalInvocationID.x]
#define WORK_GROUP_SIZE 128

layout(local_size_x = WORK_GROUP_SIZE) in;
layout(binding = 3, offset = 0) uniform atomic_uint cellCounters[100000];
struct Cell {
	int indicies[MAX_PINC];
};
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

layout(std430, binding = 2) buffer Cells {
	Cell cells[];
};

uniform uint width;
uniform uint height;
uniform uint nParticles;

void main()
{
	if (gl_GlobalInvocationID.x < nParticles) {
		ivec2 pos = ivec2(int(p.position.x), int(p.position.y));
		pos.x *= int(width) / 2;  pos.x += int(width) / 2;  if (pos.x == int(width)) pos.x--;
		pos.y *= int(height) / 2; pos.y += int(height) / 2; if (pos.y == int(height)) pos.y--;
		const uint index = uint(pos.y*width + pos.x);

		cells[index].indicies[atomicCounterIncrement(cellCounters[index])] = int(gl_GlobalInvocationID.x);
	}
}