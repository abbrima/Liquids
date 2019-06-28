#shader compute
#version 430 core

#external

layout(local_size_x = WORK_GROUP_SIZE) in;

// constants
#define TIME_STEP 0.0001f
#define WALL_DAMPING 0.6f
struct Particle {
	vec2 position;
	vec2 velocity;
	vec2 force;
	float density;
	float pressure;
};
struct Pipe {
	float lowX, lowY, highX, highY, base, dY, dX;
	int xPower;
	float xCo, dampeningRatio;
	int upper;
};
struct FloatPair {
	float f;
	float slope;
};
layout(std430, binding = 2) buffer DEBUG {
	FloatPair arr[];
};
layout(std430, binding = 1) buffer Pipes
{
	Pipe pipes[];
};
#define e 2.718281f

layout(std430, binding = 0) buffer Data
{
	Particle particles[];
};

uniform int nPipes;

float f(Pipe pipe,float x);
float slopeNormal(in Pipe pipe,in float x);
vec2 getNormal(in vec2 velocity,in float m);

void main()
{
	uint i = gl_GlobalInvocationID.x;

	// integrate
	vec2 acceleration = particles[i].force / particles[i].density;
	vec2 new_velocity = particles[i].velocity + TIME_STEP * acceleration;
	vec2 new_position = particles[i].position + TIME_STEP * new_velocity;

	// boundary conditions
	if (new_position.x < -1)
	{
		new_position.x = -1;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	else if (new_position.x > 1)
	{
		new_position.x = 1;
		new_velocity.x *= -1 * WALL_DAMPING;
	}
	else if (new_position.y < -1)
	{
		new_position.y = -1;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	else if (new_position.y > 1)
	{
		new_position.y = 1;
		new_velocity.y *= -1 * WALL_DAMPING;
	}
	{
		for (int j = 0; j < nPipes; j++)
		{
			if (new_position.x > pipes[j].lowX && new_position.x < pipes[j].highX)
			{
				float y = f(pipes[j], new_position.x);
				arr[i].f = y;
				arr[i].slope = slopeNormal(pipes[j],new_position.x);
				if (new_position.y > pipes[j].lowY && new_position.y < pipes[j].highY)
				{
					if (pipes[j].upper != 0 && new_position.y > y)
					{
						float m = slopeNormal(pipes[j], new_position.x);
						vec2 norm = getNormal(new_velocity, m);
						new_position.y = y;
						new_velocity = reflect(new_velocity, norm) * pipes[j].dampeningRatio;
					}
					else if (pipes[j].upper == 0 && new_position.y < y)
					{
						float m = slopeNormal(pipes[j], new_position.x);
						vec2 norm = getNormal(new_velocity, m);
						new_position.y = y;
						new_velocity = reflect(new_velocity, norm) * pipes[j].dampeningRatio;
					}
				}
				else
					continue;
			}
			else
				continue;
		}
	}

	particles[i].velocity = new_velocity;
	particles[i].position = new_position;
}


float f(Pipe pipe, float x) {
	float var = pipe.base * sin(pipe.xCo * pow(abs(x), pipe.xPower)) + pipe.dY;
	if (x > 0.f)
		return var;
	else
		return pipe.xPower % 2 == 0 ? var : -var + 2*pipe.dY;
}
float slopeNormal(in Pipe pipe,in float x) {
	float m = pipe.base * pipe.xCo * pipe.xPower * pow(abs(x),pipe.xPower-1)
		* cos(pipe.xCo * pow(abs(x),pipe.xPower));
	if (x<0.f)
	   m = (pipe.xPower - 1) % 2 == 0 ? m : -m;
	
	m = -1 / m;
	
	return m;
}
vec2 getNormal(in vec2 velocity,in float m)
{
	float angle = atan(m);
	vec2 norm = vec2(cos(angle), sin(angle));
	velocity = normalize(velocity);
	//return vec2(0, 1);
	if (dot(norm, velocity) >= 0)
		return norm;
	else
		return norm * -1;
}