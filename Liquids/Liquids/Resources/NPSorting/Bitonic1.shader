#shader compute
#version 430 core

#external

#define invodex gl_GlobalInvocationID.x 

layout(local_size_x = WORK_GROUP_SIZE) in;

struct Elem {
	uint c;
	uint p;
};
layout(std430, binding = 4) buffer IndexList {
	Elem es[];
};

uniform uint cdist;
uniform uint subsize;

void Exchange(inout Elem left, inout Elem right, bool SL) {
	if (SL) {
		if (left.c > right.c) {
			Elem temp;
			temp = left;
			left = right;
			right = temp;
		}
	}
	else {
		if (left.c < right.c) {
			Elem temp;
			temp = left;
			left = right;
			right = temp;
		}
	}
}

void main() {
	uint ind;
	if (invodex / cdist > 0)
		ind = invodex * 2 - invodex % cdist;
	else
		ind = invodex;

	Exchange(es[ind], es[ind + cdist], (ind / subsize) % 2 == 0);
}