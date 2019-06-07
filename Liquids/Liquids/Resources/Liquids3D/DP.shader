#shader compute
#version 460 core

#external
#define PI_FLOAT 3.1415927410125732421875f
#define invodex gl_GlobalInvocationID.x
#define p particles[invodex]
#define np particles[ulist[ind].pIndex]

layout(local_size_x = WORK_GROUP_SIZE) in;

uniform uint width, height,depth;
uniform float pr;
uniform float mass;
uniform float k;

struct Particle {
	vec4 position;
	vec4 velocity;
	vec4 forces;
	vec4 dp;
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

ivec3 GetIndex3D(vec3 position);
int _3to1(ivec3 pos);

uniform uint nParticles;

void main() {
	ivec3 CellIndex3D = GetIndex3D(p.position.xyz);
	float density_sum = 0.f;
	
	for (int i=-1;i<=1;i++)
		for (int j=-1;j<=1;j++)
			for (int kk = -1; kk <= 1; kk++)
			{
				int CellIndex = _3to1(CellIndex3D + ivec3(i, j, kk));
				if (CellIndex == -1)
					continue;
				uint ind = olist[CellIndex];
				if (ind == 0xFFFFFFFF)
					continue;
				while (ulist[ind].cIndex == CellIndex) {
					vec3 delta = p.position.xyz - np.position.xyz;
					float r = length(delta);
					if (r < h)
					{
						density_sum += mass * pow(h*h - r * r, 3) * poly6;
					}
					ind++;
				}
			}
	p.dp.x = density_sum;
	p.dp.y = max(k*(density_sum - pr), 0.f);
	
}

ivec3 GetIndex3D(vec3 position) {
	position.x *= width / 2; position.y *= height / 2; position.z *= depth / 2;
	ivec3 pos = ivec3(position);
	pos.x += int(width / 2); if (pos.x == width) pos.x--; 
	pos.y += int(height / 2); if (pos.y == height) pos.y--;
	pos.z += int(depth / 2); if (pos.z == depth) pos.z--;
	return pos;
}

int _3to1(ivec3 pos) {
	if (pos.y >= height || pos.y < 0 || pos.x >= width || pos.x < 0 || pos.z < 0 || pos.z >= depth)
		return -1;
	else
		return pos.z*int(width)*int(height) + (pos.y)*int(width) + (pos.x);
}


