#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Size;
public:
	inline unsigned int GetSize() { return m_Size; }

	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};