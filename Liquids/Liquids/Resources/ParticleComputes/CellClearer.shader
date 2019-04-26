#shader compute
#version 430 core

#define LOCALX 16
#define MAXPINC 1000

struct Cell {
	int n;
	int x;
	int y;
	int indicies[MAXPINC];
};

layout(local_size_x = LOCALX, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 3) buffer CellData
{
	Cell cells[];
};


void main()
{
	cells[gl_GlobalInvocationID.x].n = 0;
};
