#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 anormal;
layout(location = 2) in vec2 texCoord;

out vec2 v_TexCoord;
out vec3 FragPos;
out vec3 normal;


uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{	
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
	FragPos = vec3(u_Model * position);
	normal = mat3(transpose(inverse(u_Model))) * anormal;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 LightSource;

	float Constant;
	float Linear;
	float Quadratic;
};

in vec2 v_TexCoord;
in vec3 normal;
in vec3 FragPos;

uniform vec3 u_ViewPos;
uniform Light lights[10];
uniform int nLights;
uniform float shininess;


vec3 calculateLight(Light light, vec3 norm, vec3 ViewDir);
void main()
{
	//diffuse
	vec3 norm;
	norm = normalize(normal);
	vec3 ViewDir = normalize(u_ViewPos - FragPos);
	color = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < nLights; i++)
		color += vec4(calculateLight(lights[i],norm,ViewDir),1.0);
};
vec3 calculateLight(Light light, vec3 norm, vec3 ViewDir)
{
	//ambient
	vec3 ambient = vec3(texture(texture_diffuse1, v_TexCoord)) * light.ambient;

	//diffuse
	vec3 LightDir;
	if (light.LightSource.w == 1.0)
		LightDir = normalize(vec3(light.LightSource) - FragPos);
	else if (light.LightSource.w == 0.0)
		LightDir = normalize(-vec3(light.LightSource));
	float diff = max(dot(norm, LightDir), 0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(texture_diffuse1, v_TexCoord)));

	//specular
	vec3 reflectDir = reflect(-LightDir, norm);
	float spec = pow(max(dot(ViewDir, reflectDir), 0.0), shininess);
	vec3 specular = vec3(texture(texture_specular1, v_TexCoord)) * spec * light.specular;

	if (!(light.Constant == 0.0 && light.Linear == 0.0 && light.Quadratic == 0.0)) {
		float distance = length(vec3(light.LightSource) - FragPos);
		float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);
		ambient *= attenuation; diffuse *= attenuation; specular *= attenuation;
	}
	return ambient + diffuse + specular;
}
