#shader compute
#version 430 core

#external


layout(local_size_x = WORK_GROUP_SIZE) in;

struct Elem {
	uint c;
	uint p;
};
layout(std430, binding = 4) buffer IndexList {
	Elem es[];
};

#define GI gl_LocalInvocationIndex
#define GTid gl_LocalInvocationID
#define DTid gl_GlobalInvocationID

uniform uint u_Level;
uniform uint u_LevelMask;

shared Elem shared_data[WORK_GROUP_SIZE];


void main() {

	// Load shared data
	shared_data[gl_LocalInvocationIndex] = es[gl_GlobalInvocationID.x];

	barrier();
	
	// Sort the shared data
	for (uint j = u_Level >> 1; j > 0; j >>= 1) {

			Elem result = ((shared_data[gl_LocalInvocationIndex & ~j].c <= shared_data[gl_LocalInvocationIndex | j].c) 
				== bool(u_LevelMask & gl_GlobalInvocationID.x)) ? 
				shared_data[gl_LocalInvocationIndex ^ j] : shared_data[gl_LocalInvocationIndex];
		

			barrier();
     		shared_data[gl_LocalInvocationIndex] = result;

			barrier();
	}

	// Store shared data
	es[gl_GlobalInvocationID.x] = shared_data[gl_LocalInvocationIndex];	
}

