#pragma once

typedef unsigned int uint;
class AtomicCounterBuffer
{
private:
	uint m_RendererID; uint length;
public:
	AtomicCounterBuffer(void* data,uint size);
	~AtomicCounterBuffer();
	inline uint GetID() { return m_RendererID; }
	void Bind() const;
	void Unbind() const;
	void Reset();
	void Unmap();
	void* GetData();
};