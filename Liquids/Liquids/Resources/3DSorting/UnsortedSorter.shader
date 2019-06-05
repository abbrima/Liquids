#shader compute
#version 460 core

#external


layout(local_size_x = WORK_GROUP_SIZE) in;

struct Particle {
	vec3 position;
	vec3 velocity;
	vec3 force;
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
uniform uint depth;
uniform uint nParticles;

uint GetIndex(in vec3 cellIndex);

void main()
{
	uint i = gl_GlobalInvocationID.x;
	ulist[i].cIndex = GetIndex(particles[i].position);
	ulist[i].pIndex = i;
	ulist[i + nParticles].cIndex = 0xFFFFFFFF;	
	ulist[i + nParticles].pIndex = 0xFFFFFFFF;
}

uint GetIndex(in vec3 cellIndex)
{
	const uint p1 = 73856093; // some large primes
	const uint p2 = 19349663;
	const uint p3 = 83492791;
	uint n = p1 * cellIndex.x ^ p2*cellIndex.y ^ p3*cellIndex.z;
	n %= depth*width*height;
	return n;
}