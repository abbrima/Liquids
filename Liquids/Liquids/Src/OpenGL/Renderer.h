#pragma once

#include <gl/glew.h> 
#include "OpenGL/VertexArray.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/SSBO.h"
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
	inline void DrawPoints(SSBO& ssbo,const IndexBuffer& ib,Shader& shader)
	{
		shader.Bind(); ib.Bind();
		ssbo.BindToDraw();
		glDrawElements(GL_POINTS, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}
	inline void DrawPoints(VertexArray& va, Shader& shdr,const uint& count) {
		shdr.Bind();
		va.Bind();
		glDrawArrays(GL_POINTS, 0, count);
	}
	inline void DrawTriangles(SSBO& ssbo, Shader& shader,const uint& count)
	{
		shader.Bind(); 
		ssbo.BindToDraw();
		glDrawArrays(GL_TRIANGLES, 0, count);
	}
	void DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawTriangleFan(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawQuads(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawTriangles(SSBO& ssbo,const IndexBuffer& ib, Shader& shader) const;
	void DrawPoints(SSBO& ssbo, Shader& shader,const uint& count) const;
	void DrawLineStrip(const VertexArray& va, Shader& shader,const uint& count) const;
	void DrawTriangles(const VertexArray& va, Shader& shader,const uint& count) const;
};