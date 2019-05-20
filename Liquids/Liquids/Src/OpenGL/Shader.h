#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Tools/Materials.h"
#include "Tools/Lights.h"
#include "OpenGL/SSBO.h"
#include "OpenGL/AtomicCounterBuffer.h"
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string ComputeSource;
};
class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
	std::unordered_map<std::string, uint> m_BlockLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind()const;
	void DispatchCompute(uint x,uint y,uint z);
	void Unbind() const;

	//set uniforms
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform1ui(const std::string& name, uint value);
	void SetUniform1i(const std::string& name, int value);
	void SetUniformMat4f(const std::string& name, glm::mat4 matrix, bool trans = false);
	void SetUniformMaterial(const std::string& name, Material& material);
	void SetUniformLight(const std::string& name,Light& light);
	void SetUniformLightArray(const std::string& name, Light lights[],unsigned int size);
	void SetUniformLightClass(const std::string& name, LightClass& lights);
	void SetUniformVec2(const std::string& name, glm::vec2 vec);
	void SetUniformVec3(const std::string& name, glm::vec3 vec);
	void SetUniformVec4(const std::string& name, glm::vec4 vec);

	//set SSBO
	void BindSSBO(SSBO& ssbo, std::string name, uint BindingPoint);
	//set ACB
	void BindACB(AtomicCounterBuffer& acb, uint BindingPoint);
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexshader, const std::string& fragmentshader, const std::string& computeshader);
	int GetUniformLocation(const std::string& name);
	uint GetBlockLocation(const std::string& name);
};