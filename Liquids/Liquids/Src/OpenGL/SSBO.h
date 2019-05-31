#pragma once

typedef unsigned int uint;
class VertexBufferLayout;
class SSBO
{
private:
	uint m_RendererID;
	VertexBufferLayout* layout;
public:
	SSBO(const void* data,const uint& size);
	~SSBO();
	inline uint GetID() { return m_RendererID; }
	void Bind() const;
	void BindToDraw() const;
	void Unbind() const;
	void Write(const void* data,const uint& size);
	void Append(const void* data,const uint& size,const uint& offset);
	void* GetData();
	void Unmap() const;
	void SetLayout(VertexBufferLayout& l);
};