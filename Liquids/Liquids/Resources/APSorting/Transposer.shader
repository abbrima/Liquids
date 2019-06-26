#shader compute
#version 430 core

#external


layout(local_size_x = XSIZE ,local_size_y = YSIZE, local_size_z = 1) in;

struct Elem {
	uint c;
	uint p;
};
layout(std430, binding = 4) buffer InputBUF {
	Elem inputElem[];
};
layout(std430, binding = 5) buffer OutputBUF {
	Elem outputElem[];
};

#define GI gl_LocalInvocationIndex
#define GTid gl_LocalInvocationID
#define DTid gl_GlobalInvocationID

uniform uint u_Width;
uniform uint u_Height;

shared Elem shared_data[XSIZE * YSIZE];

void main() {
	shared_data[gl_LocalInvocationIndex] = inputElem[gl_GlobalInvocationID.y * u_Width + gl_GlobalInvocationID.x];
	
	groupMemoryBarrier();	barrier();

	uvec2 XY = DTid.yx - GTid.yx + GTid.xy;
	outputElem[XY.y * u_Height + XY.x] = shared_data[gl_LocalInvocationID.x * XSIZE + gl_LocalInvocationID.y];
}
