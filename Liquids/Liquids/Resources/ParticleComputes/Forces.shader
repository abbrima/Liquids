#shader compute
#version 460 core

#external
#define invodex gl_GlobalInvocationID.x
#define p particles[invodex]
#define np particles[ulist[j].pIndex]


layout(local_size_x = WORK_GROUP_SIZE) in;

// constants
uniform int nParticles;
#define PI_FLOAT 3.1415927410125732421875f
uniform float mass;
uniform float viscosity;
uniform vec2 gravity;
uniform uint width, height;
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

	vec2 pressureForce = vec2(0, 0), viscosityForce = vec2(0, 0);

	for (int iii=-1;iii<=1;iii++)
		for (int jjj = -1; jjj <= 1; jjj++)
		{
			int CellIndex = _2to1(CellIndex2D + ivec2(iii, jjj));
			if (CellIndex == -1)
				continue;
			uint j = olist[CellIndex];
			if (j == 0xFFFFFFFF)
				continue;
			while (ulist[j].cIndex == CellIndex && j < nParticles) {
				//resolve particles
				if (ulist[j].pIndex == invodex)
				{
					j++;
					continue;
				}
				vec2 delta = p.position - np.position;
				float r = length(delta);
				if (r < h)
				{
					pressureForce -= mass * (p.pressure + np.pressure) / (2.f * np.density) *
						// gradient of spiky kernel
						gradspiky * pow(h - r, 2) * normalize(delta);

					viscosityForce += mass * (np.velocity - p.velocity) / np.density *
						// Laplacian of viscosity kernel
						laplacianvis * (h - r);
				}

				j++;
			}
		}


	viscosityForce *= viscosity;
	vec2 externalForce = p.density * gravity;

	p.force = pressureForce + viscosityForce + externalForce;
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

