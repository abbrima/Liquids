#include "OpenGL/UBO.h"

#include "OpenGL/Renderer.h"
#include <memory>

UBO::UBO(const void* data, const uint& size) {
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW));
	Unbind();
}
UBO::~UBO() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}
void UBO::Bind() const {
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
}
void UBO::Unbind() const {
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
void UBO::Write(const void* data, const uint& size) {
	Bind();
	GLvoid* p;
	GLCall(p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
	memcpy(p, data, size);
	GLCall(glUnmapBuffer(GL_UNIFORM_BUFFER));
	Unbind();
}
void UBO::Append(const void* data, const uint& size, const uint& offset) {
	Bind();
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
	Unbind();
}
void* UBO::GetData() {
	Bind();
	GLvoid* p;
	GLCall(p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
	return p;
}
void UBO::Unmap() const {
	GLCall(glUnmapBuffer(GL_UNIFORM_BUFFER));
	Unbind();
}