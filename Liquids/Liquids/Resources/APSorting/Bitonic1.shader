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
void exchange(inout UnsortedList lefter, inout UnsortedList righter,bool SL);


void main() {
	uint ind;
	if (invodex / compare_distance > 0)
		ind = invodex * 2 - invodex % compare_distance;
	else
		invodex = ind;

	exchange(ulist[ind], ulist[ind + compare_distance], (ind / subsize) % 2 == 0);

}
void swap(inout UnsortedList first, inout UnsortedList second) {
	UnsortedList temp = first;
	first = second;
	second = temp;
}
void exchange(inout UnsortedList lefter, inout UnsortedList righter,bool SL) {
	if (SL) {
		if (lefter.pIndex > righter.pIndex)
			swap(lefter, righter);
	}
	else {
		if (lefter.pIndex < righter.pIndex)
			swap(lefter, righter);
	}
}
