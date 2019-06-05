#shader vertex
#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec2 force;
layout(location = 3) in vec4 color;
layout(location = 4) in vec2 dp;
layout(location = 5) in vec2 prk;
layout(location = 6) in vec2 mvf;


uniform uint nParticles;

uniform mat4 u_MVP;
uniform float radius;

out vec4 VertexColor;

void main()
{
	gl_PointSize = radius;
	VertexColor = vec4(color.xyz * gl_VertexID/nParticles , color.w);
	gl_Position = u_MVP * vec4(position,1.0,1.0);
};

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;


in vec4 VertexColor;

void main()
{
	
	color = VertexColor;
};



