#shader compute
#version 430 core

#define MAX_NEIGHBORS 8
#define p particles[gl_GlobalInvocationID.x]
#define LOCALX 1024
#define MAXPINC 1000

struct Particle {
	vec2 position;
	vec2 prevPosition;
	vec2 velocity;
	float density;
	float nearDensity;
	float pressure;
	float nearPressure;
	int neighbors[MAX_NEIGHBORS];
};
struct Cell {
	int n;
	int x;
	int y;
	int indicies[MAXPINC];
};

uniform int sizei;
uniform int sizej;
uniform int width;
uniform int height;
uniform uint nCells;

layout(local_size_x = LOCALX, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 2) buffer Data
{
	Particle particles[];
};
layout(std430, binding = 3) buffer CellData
{
	Cell cells[];
};


void main()
{
	int i = int(p.position.y / sizei);
	int j = int(p.position.x / sizej);
	i += height / 2;
	j += width / 2;



	cells[(j*height) + i].indicies[cells[(j*height) + i].n] = int(gl_GlobalInvocationID.x);
	cells[(j*height) + i].n += 1;
	memoryBarrierBuffer();

};
