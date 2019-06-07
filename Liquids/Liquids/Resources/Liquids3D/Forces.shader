#shader compute
#version 460 core

#external
#define invodex gl_GlobalInvocationID.x
#define p particles[invodex]
#define np particles[ulist[ind].pIndex]


layout(local_size_x = WORK_GROUP_SIZE) in;

// constants
#define PI_FLOAT 3.1415927410125732421875f
uniform float mass;
uniform float viscosity;
uniform vec3 gravity;



uniform uint width, height,depth;


uniform int enableInteraction;
uniform vec3 worldPos;
uniform float interactionForce;

struct Particle {
	vec4 position;
	vec4 velocity;
	vec4 force;
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

void main() {
	ivec3 CellIndex3D = GetIndex3D(p.position.xyz);

	vec3 pressureForce = vec3(0, 0,0), viscosityForce = vec3(0,0, 0);
	
	for (int i=-1;i<=1;i++)
		for (int j = -1; j <= 1; j++)
			for (int k = -1; k <= 1; k++)
			{
				
				int CellIndex = _3to1(CellIndex3D + ivec3(i,j,k));
				if (CellIndex == -1)
					continue;
				uint ind = olist[CellIndex];
				if (ind == 0xFFFFFFFF)
					continue;
				while (ulist[ind].cIndex == CellIndex) {
					//resolve particles
					if (ulist[ind].pIndex == invodex)
					{
						ind++;
						continue;
					}
					vec3 delta = p.position.xyz - np.position.xyz;
					float r = length(delta);
					if (r < h)
					{
						pressureForce -= mass * (p.dp.y + np.dp.y) / (2.f * np.dp.x) *
							// gradient of spiky kernel
							gradspiky * pow(h - r, 2) * normalize(delta);

						viscosityForce += mass * (np.velocity.xyz - p.velocity.xyz) / np.dp.x *
							// Laplacian of viscosity kernel
							laplacianvis * (h - r);
					}
					
					ind++;
				}
				
			}


	viscosityForce *= viscosity;
	vec3 externalForce1 = p.dp.x * gravity;

	vec3 externalForce2 = vec3(0, 0,0);
	vec3 myvec = worldPos - p.position.xyz;
	if (myvec != vec3(0, 0,0) && enableInteraction != 0) {
		externalForce2 = p.dp.x * normalize(myvec) * interactionForce;
	}

	p.force = vec4(pressureForce + viscosityForce + externalForce1 + externalForce2 , 0.f);
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


