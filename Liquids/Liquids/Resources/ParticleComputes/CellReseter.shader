#shader compute
#version 430 core

#define MAX_PINC 10000

#define WORK_GROUP_SIZE 64

layout(local_size_x = WORK_GROUP_SIZE) in;
layout(binding = 3, offset = 0) uniform atomic_uint cellCounters[];

struct Cell {
	int indicies[MAX_PINC];
};

layout(std430, binding = 2) buffer Cells {
	Cell cells[];
};

void main()
{

}