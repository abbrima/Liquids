#shader compute
#version 430 core


layout(local_size_x = 2, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 2) buffer Data
{
	float buf[];
};

uniform float val;

void main()
{
	buf[gl_GlobalInvocationID.x] += val;
};