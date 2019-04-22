#include "OpenGL/SSBO.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/VertexBufferLayout.h"
#include <memory>

SSBO::SSBO(const void* data, uint size)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW));
	Unbind();
}

SSBO::~SSBO()
{
	GLCall(glDeleteBuffers(1,&m_RendererID));
}
void SSBO::Bind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
}
void SSBO::BindToDraw() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	const auto& elements = layout->GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type,
			element.normalized, layout->GetStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}
void SSBO::SetLayout(VertexBufferLayout& l)
{
	layout = new VertexBufferLayout();
	*layout = l;
}
void SSBO::Unbind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}
void SSBO::Write(const void* data,uint size)
{
	Bind();
	GLvoid* p;
	GLCall(p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
	memcpy(p, data, size);
	GLCall(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));
	Unbind();
}
void SSBO::Append(const void* data, uint size, uint offset)
{

	Bind(); 
	GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data));
	Unbind();
}
void* SSBO::GetData()
{
	Bind();
	GLvoid* p;
	GLCall(p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE));
	return p;
}
void SSBO::Unmap() const
{
	GLCall(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));
	Unbind();
}