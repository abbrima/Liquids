#shader compute
#version 430 core

#external

layout(local_size_x = WORK_GROUP_SIZE) in;

layout(binding = 3, offset=0) uniform atomic_uint CC[100000];

struct UnsortedList {
	uint cIndex; uint pIndex;
};
layout(std430, binding = 4) buffer IndexList
{
	UnsortedList ulist[];
};

void main()
{
	//atomicCounterIncrement(CC[0]);
}