#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/constants.hpp"
#include "Tools/Materials.h"
#include "Tools/Lights.h"

#include "OpenGL/SSBO.h"
#include "OpenGL/AtomicCounterBuffer.h"
#include "OpenGL/UBO.h"

#define DSIZE(x) std::string("#define WORK_GROUP_SIZE ") + std::to_string(x)
#define XSIZE(x) std::string("#define XSIZE ") + std::to_string(x)
#define YSIZE(x) std::string("#define YSIZE ") + std::to_string(x)
#define ZSIZE(x) std::string("#define ZSIZE ") + std::to_string(x)

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string ComputeSource;
	std::string GeometrySource;
};
class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
	std::unordered_map<std::string, uint> m_BlockLocationCache;
	std::unordered_map<std::string, uint> m_UniformBlockLocationCache;
public:
	Shader(const std::string& filepath,const std::string& extra = "");
	~Shader();

	void Bind()const;
	void DispatchCompute(const uint& x,const uint& y=1,const uint& z=1 );
	void Unbind() const;

	//set uniforms
	void SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3);
	void SetUniform3f(const std::string& name, const float& v0, const float& v1, const float& v2);
	void SetUniform2f(const std::string& name, const float& v0, const float& v1);
	void SetUniform1f(const std::string& name, const float& value);
	void SetUniform1ui(const std::string& name,const uint& value);
	void SetUniform1i(const std::string& name,const int& value);
	void SetUniformMat4f(const std::string& name,const glm::mat4& matrix, bool trans = false);
	void SetUniformMaterial(const std::string& name, Material& material);
	void SetUniformLight(const std::string& name,Light& light);
	void SetUniformLightArray(const std::string& name, Light lights[],const uint& size);
	void SetUniformLightClass(const std::string& name, LightClass& lights);
	void SetUniformVec2(const std::string& name,const glm::vec2& vec);
	void SetUniformVec3(const std::string& name,const glm::vec3& vec);
	void SetUniformVec4(const std::string& name,const glm::vec4& vec);
	void SetUniform1b(const std::string& name, const bool& b);

	void BindSSBO(SSBO& ssbo,const std::string& name,const uint& BindingPoint);
	void BindACB(AtomicCounterBuffer& acb,const uint& BindingPoint);
	void BindUBO(UBO& ubo, const std::string& name, const uint& BindingPoint);
private:
	ShaderProgramSource ParseShader(const std::string& filepath,const std::string& extra);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexshader, const std::string& fragmentshader,
		const std::string& computeshader, const std::string& geometryShader);
	int GetUniformLocation(const std::string& name);
	uint GetBlockLocation(const std::string& name);
	uint GetUniformBlockLocation(const std::string& name);
};