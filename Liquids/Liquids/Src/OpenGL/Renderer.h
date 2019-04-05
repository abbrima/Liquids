#pragma once

#include <gl/glew.h> 
#include "OpenGL/VertexArray.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#if DEBUG
#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall(#x, __FILE__ , __LINE__ ))
#else
#define GLCall(x) x
#endif
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{

public:
	void Clear() const;
	void DrawTriangles (const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawTriangleFan(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawQuads(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};