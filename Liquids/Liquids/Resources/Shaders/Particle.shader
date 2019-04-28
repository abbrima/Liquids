#shader vertex
#version 430 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 prevPos;
layout(location = 2) in vec2 velocity;
layout(location = 3) in vec4 stats;


uniform mat4 u_MVP;
uniform float radius;

out vec2 VELOCITY;

void main()
{
	gl_PointSize = radius;
	gl_Position = u_MVP * position;
	VELOCITY = velocity;
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

in vec2 VELOCITY;

void main()
{
	color = u_Color;
};



