#shader compute
#version 430 core

#external

#define invodex gl_GlobalInvocationID.x 

layout(local_size_x = WORK_GROUP_SIZE) in;

struct Elem {
	uint c;
	uint p;
};
layout(std430, binding = 4) buffer List {
	Elem es[];
};

uniform uint cdist;

void Exchange(inout Elem left, inout Elem right);

void main() {
	uint ind;
	if (invodex / cdist > 0)
		ind = invodex * 2 - invodex % cdist;
	else
		ind = invodex;
	Exchange(es[ind], es[ind + cdist]);
}

void Exchange(inout Elem left, inout Elem right) {
	if (left.c > right.c) {
		Elem temp = left;
		left = right;
		right = temp;
	}
}
