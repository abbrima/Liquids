#shader compute
#version 460 core

#external

#define invodex gl_GlobalInvocationID.x

layout(local_size_x = WORK_GROUP_SIZE) in;
struct Particle {
	vec4 position;
	vec4 velocity;
	vec4 force;
	vec4 dp;
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
uniform uint depth;
uint GetIndex(vec3 cellIndex);

void main()
{
	ulist[invodex].cIndex = GetIndex(particles[invodex].position.xyz);
	ulist[invodex].pIndex = invodex;
}

uint GetIndex(vec3 cellIndex)
{
	cellIndex.x *= width/2; cellIndex.y *= height/2; cellIndex.z *= depth/2;
	ivec3 pos = ivec3(cellIndex);
	pos.x += int(width) / 2; if (pos.x == width) pos.x--;
	pos.y += int(height) / 2; if (pos.y == height) pos.y--;
	pos.z += int(depth) / 2; if (pos.z == depth) pos.z--;

	return uint(pos.z)*width*height + uint(pos.y)*width + uint(pos.x);
}