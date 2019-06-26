#shader vertex
#version 430 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 velocity;
layout(location = 2) in vec4 forces;
layout(location = 3) in vec4 dp;

uniform uint nParticles;

uniform mat4 u_MVP;
uniform vec3 u_Color;

out vec3 v_Color;

void main()
{
	v_Color = u_Color;
	gl_Position = u_MVP * vec4(position.xyz,1.f);
};



#shader geometry
#version 430 core



layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

layout(std140, binding = 3) uniform ubo_Normals
{
   vec4 normals[];
};

uniform float radius;

in vec3 v_Color[];
out vec3 g_Color;
out vec3 g_Pos;
out vec3 g_Normal;

const vec4 _zero = gl_in[0].gl_Position + vec4(0.f, 0.f, radius, 1.f);
const vec4 _one = gl_in[0].gl_Position + vec4(radius, 0.f, 0.f, 1.f);
const vec4 _two = gl_in[0].gl_Position + vec4(0.f, 0.f, -radius, 1.f);
const vec4 _three = gl_in[0].gl_Position + vec4(-radius, 0.f, 0.f, 1.f);
const vec4 _four = gl_in[0].gl_Position + vec4(0.f, radius, 0.f, 1.f);
const vec4 _five = gl_in[0].gl_Position + vec4(0.f, -radius, 0.f, 1.f);
void e(vec4 p1, vec4 p2, vec4 p3 ,int id) {
	g_Normal = normals[id].xyz;

	gl_Position = p1;
	g_Pos = p1.xyz;
	EmitVertex();
	gl_Position = p2;
	g_Pos = p2.xyz;
	EmitVertex();
	gl_Position = p3;
	g_Pos = p3.xyz;
	EmitVertex();

	EndPrimitive();
}
void main() {
	g_Color = v_Color[0];
	e(_zero,_one,_four, 0);
	e(_one, _two,_four, 1);
	e(_two, _three, _four, 2);
	e(_three, _zero, _four, 3);
	e(_five, _one, _zero, 4);
	e(_five, _two, _one, 5);
	e(_five, _three, _two, 6);
	e(_five, _zero, _three, 7);
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec3 g_Color;
in vec3 g_Pos;
in vec3 g_Normal;


struct Material {
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float opacity;
};
struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 LightSource;

	float Constant;
	float Linear;
	float Quadratic;
};

uniform vec3 u_ViewPos;
uniform Material material;
uniform Light lights[10];
uniform int nLights;

vec3 calculateLight(Light light, vec3 norm, vec3 ViewDir);


void main()
{
	vec3 norm;
	norm = normalize(g_Normal);
	vec3 ViewDir = normalize(u_ViewPos - g_Pos);
	color = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < nLights; i++)
		color += vec4(calculateLight(lights[i], norm, ViewDir), 0.0f);
	color.w = material.opacity;

	color = vec4(g_Color,1.f);
};


vec3 calculateLight(Light light, vec3 norm, vec3 ViewDir)
{
	//ambient
	vec3 ambient = material.diffuse * light.ambient;

	//diffuse
	vec3 LightDir;
	if (light.LightSource.w == 1.0)
		LightDir = normalize(vec3(light.LightSource) - g_Pos);
	else if (light.LightSource.w == 0.0)
		LightDir = normalize(-vec3(light.LightSource));
	float diff = max(dot(norm, LightDir), 0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	//specular
	vec3 reflectDir = reflect(-LightDir, norm);
	float spec = pow(max(dot(ViewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * spec * light.specular;

	if (!(light.Constant == 0.0 && light.Linear == 0.0 && light.Quadratic == 0.0)) {
		float distance = length(vec3(light.LightSource) - g_Pos);
		float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);
		ambient *= attenuation; diffuse *= attenuation; specular *= attenuation;
	}
	return ambient + diffuse + specular;
}



