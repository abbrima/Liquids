#shader compute
#version 430 core

#external

#define PI_FLOAT 3.1415927410125732421875f
#define invodex gl_GlobalInvocationID.x
#define p particles[invodex]
#define np particles[ulist[j].pIndex]

layout(local_size_x = WORK_GROUP_SIZE) in;

uniform uint width, height;
uniform int nParticles;
uniform float pr;
uniform float mass;
uniform float k;

struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	float density;
	float pressure;
};
struct UnsortedList {
	uint cIndex; uint pIndex;
};

layout(std430, binding = 4) buffer IndexList
{
	UnsortedList ulist[];
};
layout(std430, binding = 5) buffer OffsetList
{
	uint olist[];
};
layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};
layout(std140, binding = 2) uniform Constants
{
float h;
float poly6;
float gradspiky;
float laplacianvis;
};

ivec2 GetIndex2D(vec2 position);
int _2to1(ivec2 pos);

void main() {
	ivec2 CellIndex2D = GetIndex2D(p.position);
	float density_sum = 0.f;

	for (int iii = -1; iii <= 1; iii++)
		for (int jjj = -1; jjj <= 1; jjj++)
		{
			int CellIndex = _2to1(CellIndex2D + ivec2(iii, jjj));
			if (CellIndex == -1)
				continue;
			//loop through all particles
			uint j = olist[CellIndex];
			if (j == 0xFFFFFFFF)
				continue;
			while (ulist[j].cIndex == CellIndex && j < nParticles) {
				//resolve particles
				vec2 delta = p.position - np.position;
				float r = length(delta);
				if (r < h)
				{
					density_sum += mass * pow(h * h - r * r, 3) * poly6;
				}
				j++;
			}
		}
	p.density = density_sum;
	p.pressure = max(k * (density_sum - pr), 0.f);
}

ivec2 GetIndex2D(vec2 position) {
	position.x *= width / 2; position.y *= height / 2;
	ivec2 pos = ivec2(position);
	pos.x += int(width / 2); if (pos.x == width) pos.x--;
	pos.y += int(height / 2); if (pos.y == height) pos.y--;
	return pos;
}
int _2to1(ivec2 pos) {
	if (pos.y >= height || pos.y < 0 || pos.x >= width || pos.x < 0)
		return -1;
	else
		return (pos.y)*int(width) + (pos.x);
}


