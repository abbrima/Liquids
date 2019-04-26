#shader vertex
#version 430 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 prevPos;
layout(location = 2) in vec2 velocity;
layout(location = 3) in vec4 stats;


uniform mat4 u_MVP;
uniform float radius;

void main()
{
	gl_PointSize = radius;
	gl_Position = u_MVP * position;
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;


void main()
{
	color = vec4(1.0, 1.0, 1.0, 1.0);
};



