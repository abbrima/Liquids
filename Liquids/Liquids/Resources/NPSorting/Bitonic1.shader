#shader compute
#version 430 core

#external

#define invodex gl_GlobalInvocationID.x 

layout(local_size_x = WORK_GROUP_SIZE) in;


struct UnsortedList {
	uint cIndex; uint pIndex;
};
layout(std430, binding = 4) buffer List
{
	UnsortedList ulist[];
};
uniform uint subsize;
uniform uint compare_distance;
void exchange(inout UnsortedList lefter, inout UnsortedList righter);


void main() {
	/*
	uint lefter_index;
	if (invodex / compare_distance > 0)
		lefter_index = invodex * 2 - invodex % compare_distance;
	else
		lefter_index = invodex;

	exchange(ulist[lefter_index], ulist[lefter_index + compare_distance],(lefter_index/subsize)%2==0);*/



	uint ixj = invodex ^ compare_distance;
		if (ixj > invodex)
		{
			if ((invodex & subsize) == 0)
			{
				if (ulist[invodex].cIndex > ulist[ixj].cIndex)
					exchange(ulist[invodex], ulist[ixj]);
			}
			else {
				if (ulist[invodex].cIndex < ulist[ixj].cIndex)
					exchange(ulist[invodex], ulist[ixj]);
			}
		}
}

void exchange(inout UnsortedList lefter, inout UnsortedList righter) {
		UnsortedList temp = lefter;
		lefter = righter;
		righter = temp;
}
