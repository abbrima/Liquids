#include "OpenGL/AtomicCounterBuffer.h"
#include "OpenGL/Renderer.h"
#include <memory>

AtomicCounterBuffer::AtomicCounterBuffer(void* data,uint length) :length(length){
	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint)*length, data, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
}
AtomicCounterBuffer::~AtomicCounterBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}
void AtomicCounterBuffer::Bind() const{
	GLCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_RendererID));
}
void AtomicCounterBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
}
void AtomicCounterBuffer::Reset() {
	Bind();
	GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,0,	sizeof(GLuint) * length,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT
	);
	memset(ptr, 0, sizeof(GLuint)*length);
	GLCall(glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER));
}
void* AtomicCounterBuffer::GetData() {
	Bind();
	void* p = glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint)*length
		, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	return p;
}
void AtomicCounterBuffer::Unmap() {
	GLCall(glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER));
}