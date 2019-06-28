#shader vertex
#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec2 force;
layout(location = 3) in vec2 dp;

uniform uint nParticles;

uniform vec3 u_Color;

out vec3 v_Color;

void main()
{
	//gradient color 
	v_Color = u_Color;
	//position u_MVP are the model view projection matricies
	gl_Position = vec4(position, 1.0, 1.0);
};

#shader geometry
#version 430 core

in vec3 v_Color[];
out vec3 g_Color;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float radius;
uniform mat4 u_MVP;


void main() {
	g_Color = v_Color[0];
	gl_Position = u_MVP * (gl_in[0].gl_Position + vec4(0.f, radius, 0.f, 0.f));
	EmitVertex();
	gl_Position = u_MVP * (gl_in[0].gl_Position + vec4(radius, 0.f, 0.f, 0.f));
	EmitVertex();
	gl_Position = u_MVP * (gl_in[0].gl_Position + vec4(-radius, 0.f, 0.f, 0.f));
	EmitVertex();
	gl_Position = u_MVP * (gl_in[0].gl_Position + vec4(0.f, -radius, 0.f, 0.f));
	EmitVertex();
	EndPrimitive();
}

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec3 g_Color;

void main()
{
	color = vec4(g_Color, 1.0f);
};



