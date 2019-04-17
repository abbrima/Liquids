#pragma once

typedef unsigned int uint;
class VertexBufferLayout;
class SSBO
{
private:
	uint m_RendererID;
	VertexBufferLayout* layout;
public:
	SSBO(const void* data,uint size);
	~SSBO();
	inline uint GetID() { return m_RendererID; }
	void Bind() const;
	void BindToDraw() const;
	void Unbind() const;
	void Write(const void* data,uint size);
	void* GetData();
	void Unmap() const;
	void SetLayout(VertexBufferLayout& l);
};