#shader compute
#version 460 core

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

uniform uint compare_distance;

void exchange(inout UnsortedList lefter, inout UnsortedList righter);

void main() {
	uint lefter_index;
	if (invodex / compare_distance > 0)
		lefter_index = invodex * 2 - invodex % compare_distance;
	else
		lefter_index = invodex;

	//exchange(ulist[lefter_index], ulist[lefter_index + compare_distance]);
}


void exchange(inout UnsortedList lefter, inout UnsortedList righter) {
		if (lefter.cIndex > righter.cIndex)
		{
			UnsortedList temp = lefter;
			lefter = righter;
			righter = temp;
		}
}

