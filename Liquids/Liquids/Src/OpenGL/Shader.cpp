#include "Shader.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Renderer.h"
#include "math_3d.h"

Shader::Shader(const std::string& filepath):m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}
Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	//compiles this shader into a program (returned as unsigned int)
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char * message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") <<
			" Shader" << std::endl << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
//gets shader code from files and puts it in a string
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	ShaderType type = ShaderType::NONE;
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss[2];
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}	return { ss[0].str() , ss[1].str() };

}
void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}
void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name),v0,v1,v2,v3));
}
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}
void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}
void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}
void Shader::SetUniformMat4f(const std::string& name, glm::mat4 matrix,bool trans)
{
	if (!trans)
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
	else
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE, &matrix[0][0]));
}
void Shader::SetUniformBones(const std::string& name, std::vector<Matrix4f>& matrix)
{
	for (unsigned int i = 0; i < matrix.size(); i++)
	{
		std::stringstream ss;
		ss << i;
		glUniformMatrix4fv(GetUniformLocation(name+"["+ss.str()+"]"), 1, GL_TRUE, (const GLfloat*)matrix[i]);
	}
}
void Shader::SetUniformMaterial(const std::string& name, Material& material)
{
	SetUniform3f((name + ".diffuse"), material.diffuse.x, material.diffuse.y, material.diffuse.z);
	SetUniform3f((name + ".specular"), material.specular.x, material.specular.y, material.specular.z);
	SetUniform1f((name + ".shininess"), material.shininess);
	SetUniform1f((name + ".opacity"), material.opacity);
}
void Shader::SetUniformLight(const std::string& name, Light& light)
{
	SetUniform3f((name + ".ambient"), light.ambient.x, light.ambient.y, light.ambient.z);
	SetUniform3f((name + ".diffuse"), light.diffuse.x, light.diffuse.y, light.diffuse.z);
	SetUniform3f((name + ".specular"), light.specular.x, light.specular.y, light.specular.z);
	SetUniform4f((name + ".LightSource"), light.LightSource.x, light.LightSource.y, light.LightSource.z, light.LightSource.w);
	SetUniform1f((name + ".Constant"), light.Constant);
	SetUniform1f((name + ".Linear"), light.Linear);
	SetUniform1f((name + ".Quadratic"), light.Quadratic);

}
void Shader::SetUniformLightArray(const std::string& name, Light lights[], unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		std::stringstream ss;
		ss << i;
		SetUniformLight(name + "[" + ss.str() + "]", lights[i]);
	}
	SetUniform1i("nLights", (int)size);
}
void Shader::SetUniformLightClass(const std::string& name, LightClass& lights)
{
	std::vector<Light> vec = lights.GetActiveLightsVector();
	SetUniformLightArray(name, vec.data(), lights.GetActiveLightsNumber());
}
void Shader::SetUniformVec3(const std::string& name, glm::vec3 vec)
{
	SetUniform3f(name, vec.x, vec.y, vec.z);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform (" << name << ") doesn't exist!\n";
	m_UniformLocationCache[name] = location;
	return location;
}