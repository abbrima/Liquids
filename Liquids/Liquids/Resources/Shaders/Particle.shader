#shader vertex
#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec2 force;
layout(location = 3) in vec2 dp;

uniform uint nParticles;

uniform mat4 u_MVP;
uniform float radius;
uniform vec3 u_Color;

out vec3 VertexColor;

void main()
{
	gl_PointSize = radius;
	VertexColor = u_Color * gl_VertexID/nParticles;
	gl_Position = u_MVP * vec4(position,1.0,1.0);
};

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;


in vec3 VertexColor;

void main()
{
	
	color = vec4(VertexColor, 1.0f);
};



