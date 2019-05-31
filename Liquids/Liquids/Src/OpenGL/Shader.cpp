#include "OpenGL/Shader.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "OpenGL/Renderer.h"

Shader::Shader(const std::string& filepath,const std::string& extra):m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath,extra);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource,source.ComputeSource);
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
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& computeShader)
{

	unsigned int program = glCreateProgram();

	if (computeShader.empty())
	{
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
	else
	{
		unsigned int cs = CompileShader(GL_COMPUTE_SHADER, computeShader);

		glAttachShader(program, cs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(cs);
		return program;
	}
	
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath,const std::string& extra)
//gets shader code from files and puts it in a string
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1,COMPUTE = 2
	};
	ShaderType type = ShaderType::NONE;
	std::ifstream stream(filepath);

	if (!stream.is_open())
		std::cout << "NOT FOUND!: " << filepath << std::endl;

	std::string line;
	std::stringstream ss[3];
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
			else if (line.find("compute") != std::string::npos)
				type = ShaderType::COMPUTE;
		}
		else if (line.find("#external") != std::string::npos)
		{
			ss[(int)type] << extra << '\n';
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}	
	return { ss[0].str() , ss[1].str() ,ss[2].str()};

}
void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}
void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}
void Shader::DispatchCompute(const uint& x , const uint& y ,const uint& z)
{
	Bind();
	GLCall(glDispatchCompute(x, y, z));
	GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
}
void Shader::SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3)
{
	GLCall(glUniform4f(GetUniformLocation(name),v0,v1,v2,v3));
}
void Shader::SetUniform3f(const std::string& name, const float& v0, const float& v1, const float& v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}
void Shader::SetUniform2f(const std::string& name, const float& v0, const float& v1) {
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}
void Shader::SetUniform1f(const std::string& name, const float& value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}
void Shader::SetUniform1ui(const std::string& name, const uint& value) {
	GLCall(glUniform1ui(GetUniformLocation(name), value));
}
void Shader::SetUniform1i(const std::string& name, const int& value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}
void Shader::SetUniformMat4f(const std::string& name,const glm::mat4& matrix,bool trans)
{
	if (!trans)
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
	else
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE, &matrix[0][0]));
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
void Shader::SetUniformLightArray(const std::string& name, Light lights[], const uint& size)
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
void Shader::SetUniformVec2(const std::string& name,const glm::vec2& vec)
{
	SetUniform2f(name, vec.x, vec.y);
}
void Shader::SetUniformVec3(const std::string& name,const glm::vec3& vec)
{
	SetUniform3f(name, vec.x, vec.y, vec.z);
}
void Shader::SetUniformVec4(const std::string& name,const glm::vec4& vec)
{
	SetUniform4f(name, vec.x, vec.y, vec.z, vec.w);
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
uint Shader::GetBlockLocation(const std::string& name)
{
	if (m_BlockLocationCache.find(name) != m_BlockLocationCache.end())
		return m_BlockLocationCache[name];
	GLCall(uint location = glGetProgramResourceIndex(m_RendererID, GL_SHADER_STORAGE_BLOCK, name.c_str()));
	m_BlockLocationCache[name] = location;
	return location;
}
uint Shader::GetUniformBlockLocation(const std::string& name)
{
	if (m_UniformBlockLocationCache.find(name) != m_BlockLocationCache.end())
		return m_UniformBlockLocationCache[name];
	GLCall(uint location = glGetUniformBlockIndex(m_RendererID, name.c_str()));
	m_UniformBlockLocationCache[name] = location;
	return location;
}
void Shader::BindSSBO(SSBO& ssbo,const std::string& name, const uint& BindingPoint)
{
	Bind();
	ssbo.Bind();
	const uint& index = GetBlockLocation(name);
	GLCall(glShaderStorageBlockBinding(m_RendererID, index, BindingPoint));
	GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BindingPoint, ssbo.GetID()));
}
void Shader::BindACB(AtomicCounterBuffer& acb, const uint& BindingPoint) {
	Bind();
	acb.Bind();
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, BindingPoint, acb.GetID());
}
void Shader::BindUBO(UBO& ubo, const std::string& name, const uint& BindingPoint) {
	Bind();
	ubo.Bind();
	const uint& index = GetUniformBlockLocation(name);
	GLCall(glUniformBlockBinding(m_RendererID,index,BindingPoint));
	glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, ubo.GetID());
}