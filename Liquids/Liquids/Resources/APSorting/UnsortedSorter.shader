#shader compute
#version 460 core

#external


layout(local_size_x = WORK_GROUP_SIZE) in;


struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	vec2 color1;
	vec2 color2;
	float density;
	float pressure;
	float pr;
	float k;
	float mass;
	float viscosity;
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
}

uint GetIndex(vec2 position) {
	position.x *= width / 2; position.y *= height / 2;
	ivec2 pos = ivec2(position);
	pos.x += int(width / 2); if (pos.x == width) pos.x--;
	pos.y += int(height / 2); if (pos.y == height) pos.y--;

	return uint(pos.y)*width + uint(pos.x);
}
