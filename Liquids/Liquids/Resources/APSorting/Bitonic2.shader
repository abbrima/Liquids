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
	uint ind;
	if (invodex / compare_distance > 0)
		ind = invodex * 2 - invodex % compare_distance;
	else
		invodex = ind;
	exchange(ulist[ind], ulist[ind + compare_distance]);
}

void swap(inout UnsortedList first, inout UnsortedList second) {
	UnsortedList temp = first;
	first = second;
	second = temp;
}
void exchange(inout UnsortedList lefter, inout UnsortedList righter) {
	if (lefter.pIndex > righter.pIndex)
		swap(lefter, righter);
}
