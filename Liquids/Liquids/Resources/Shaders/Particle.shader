#shader vertex
#version 430 core

layout(location = 0) in vec4 position;



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

uniform vec4 u_Color;


void main()
{
	color = u_Color;
};



