#shader compute
#version 430 core

#external

#define invo gl_GlobalInvocationID.x

layout(local_size_x = WORK_GROUP_SIZE) in;

struct UnsortedList {
	uint cIndex; uint pIndex;
};
layout(std430, binding = 4) buffer IndexList
{
	UnsortedList ulist[];
};
layout(std430, binding = 5) buffer OffsetList
{
	uint olist[];
};

void main()
{
	atomicMin(olist[ulist[invo].cIndex],invo);
}