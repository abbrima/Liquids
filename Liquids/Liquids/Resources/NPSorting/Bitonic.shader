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

#define GI gl_LocalInvocationIndex
#define GTid gl_LocalInvocationID
#define DTid gl_GlobalInvocationID


uniform uint u_Level;
shared Elem shared_data[WORK_GROUP_SIZE];


void main() {

	shared_data[GI] = es[DTid.x];
	groupMemoryBarrier();
	barrier();
	
	for (uint j = u_Level >> 1; j > 0; j >>= 1) {

			Elem result = ((shared_data[GI & ~j].c <= shared_data[GI | j].c) 
				== bool(u_Level & DTid.x)) ? es[GI ^ j] : es[GI];


			groupMemoryBarrier();
			barrier();

     		shared_data[GI] = result;

			groupMemoryBarrier();
			barrier();
	}

	es[DTid.x] = shared_data[GI];	
}

*/