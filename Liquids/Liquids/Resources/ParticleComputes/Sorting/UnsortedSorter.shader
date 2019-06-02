#shader compute
#version 460 core

#external


layout(local_size_x = WORK_GROUP_SIZE) in;

struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	float density;
	float pressure;
};
struct UnsortedList {
	uint cIndex; uint pIndex;
};
layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};
layout(std430, binding = 4) buffer List
{
	UnsortedList ulist[];
};


uniform uint width;
uniform uint height;
uniform uint nParticles;

uint GetIndex(vec2 position);

void main()
{
	uint i = gl_GlobalInvocationID.x;
	ulist[i].cIndex = GetIndex(particles[i].position);
	ulist[i].pIndex = i;
	ulist[i + nParticles].cIndex = 0xFFFFFFFF;	
	ulist[i + nParticles].pIndex = 0xFFFFFFFF;
}

uint GetIndex(vec2 position) {
	position.x *= width / 2; position.y *= height / 2;
	ivec2 pos = ivec2(position);
	pos.x += int(width / 2); if (pos.x == width) pos.x--;
	pos.y += int(height / 2); if (pos.y == height) pos.y--;

	return uint(pos.y)*width + uint(pos.x);
}
