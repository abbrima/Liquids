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

/*
Gid  : gl_WorkGroupID            //atm: -, 0...256, - in rows (Y)        --> current group index (dispatched by c++)
DTid : gl_GlobalInvocationID   //atm: 0...256 in rows & columns (XY)   --> "global" thread id
GTid : gl_LocalInovcationID    //atm: 0...256, -,- in columns (X)      --> current threadId in group / "local" threadId
GI   : gl_LocalInovcationIndex         //atm: 0...256 in columns (X)           --> "flattened" index of a thread within a group
*/
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

