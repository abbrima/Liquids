#pragma once

typedef unsigned int uint;
class AtomicCounterBuffer
{
private:
	uint m_RendererID; uint length;
public:
	AtomicCounterBuffer(void* data,const uint& size);
	~AtomicCounterBuffer();
	inline uint GetID() { return m_RendererID; }
	void Bind() const;
	void Unbind() const;
	void Reset();
	void Set(const uint& val);
	void Unmap();
	void* GetData();
};