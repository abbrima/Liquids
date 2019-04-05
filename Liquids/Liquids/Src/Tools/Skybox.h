#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include <string>

class Skybox
{
public:
	Skybox();
	~Skybox();
	void Render(glm::mat4& mvp) const;
	void ChooseBox(int i);
private:
	VertexArray *m_VAO;
	VertexBuffer *m_VBO;
	IndexBuffer *m1, *m2, *m3, *m4, *m5, *m6;
	Shader *m_Shader;
	Renderer renderer;
	Texture *t1, *t2, *t3, *t4, *t5, *t6;
	bool enable;
};

