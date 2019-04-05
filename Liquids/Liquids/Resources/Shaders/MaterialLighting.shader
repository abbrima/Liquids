#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 anormal;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec3 normal;
out vec3 FragPos;

void main()
{
	gl_Position = u_MVP * position;
	normal = mat3(transpose(inverse(u_Model))) * anormal;
	FragPos = vec3(u_Model *position);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

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

in vec3 normal;
in vec3 FragPos;

vec3 calculateLight(Light light, vec3 norm, vec3 ViewDir);
void main()
{
	vec3 norm;
	norm = normalize(normal);
	vec3 ViewDir = normalize(u_ViewPos - FragPos);
	color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < nLights; i++)
		color += vec4(calculateLight(lights[i], norm, ViewDir), 0.0f);
	color.w = material.opacity;
};

vec3 calculateLight(Light light, vec3 norm, vec3 ViewDir)
{
	//ambient
	vec3 ambient = material.diffuse * light.ambient;

	//diffuse
	vec3 LightDir;
	if (light.LightSource.w == 1.0)
		LightDir = normalize(vec3(light.LightSource) - FragPos);
	else if (light.LightSource.w == 0.0)
		LightDir = normalize(-vec3(light.LightSource));
	float diff = max(dot(norm, LightDir), 0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	//specular
	vec3 reflectDir = reflect(-LightDir, norm);
	float spec = pow(max(dot(ViewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * spec * light.specular;

	if (!(light.Constant == 0.0 && light.Linear == 0.0 && light.Quadratic == 0.0)) {
		float distance = length(vec3(light.LightSource) - FragPos);
		float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);
		ambient *= attenuation; diffuse *= attenuation; specular *= attenuation;
	}
	return ambient + diffuse + specular;
}