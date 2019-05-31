#pragma once

typedef unsigned int uint;
class UBO {
private:
	uint m_RendererID;
public:
	UBO(const void* data, const uint& size);
	~UBO();
	inline uint GetID() { return m_RendererID; }
	void Bind() const;
	void Unbind() const;
	void Write(const void* data, const uint& size);
	void Append(const void* data, const uint& size, const uint& offset);
	void* GetData();
	void Unmap() const;
};