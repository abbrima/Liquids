#shader compute
#version 460 core

#external


layout(local_size_x = XSIZE ,local_size_y = YSIZE, local_size_z = 1) in;

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

uniform uint u_Width;
uniform uint u_Height;

shared Elem shared_data[XSIZE * YSIZE];

void main() {
	shared_data[GI] = es[DTid.y * u_Width + DTid.x];
	groupMemoryBarrier();
	barrier();
	uvec2 XY = DTid.yx - GTid.yx + GTid.xy;
	es[XY.y * u_Height + XY.x] = shared_data[GTid.x * XSIZE + GTid.y];
}
