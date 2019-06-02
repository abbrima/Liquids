#shader compute
#version 460 core

#external

#define invo gl_GlobalInvocationID.x

layout(local_size_x = WORK_GROUP_SIZE) in;
//layout(binding = 3, offset=0) uniform atomic_uint CC;

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