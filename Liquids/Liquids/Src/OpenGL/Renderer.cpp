#include "OpenGL/Renderer.h"
#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error]: {0x" << std::hex << error << "}" <<std::dec<< "\nFunction: " << function << "\nFile: " << file
			<< "\nLine: " << line << std::endl; return false;
	}
	return true;
}

void Renderer::DrawTriangles(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
void Renderer::DrawTriangles(SSBO& ssbo, const IndexBuffer& ib, Shader& shader) const
{
	shader.Bind();
	ssbo.BindToDraw();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
void Renderer::DrawTriangleFan(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLE_FAN, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
void Renderer::DrawQuads(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_QUADS, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
void Renderer::DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
void Renderer::DrawPoints(SSBO& ssbo,Shader& shader,uint count) const
{
	shader.Bind();
	ssbo.BindToDraw();
	GLCall(glDrawArrays(GL_POINTS, 0, count));
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}