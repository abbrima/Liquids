#include "Applications/Hello Triangle/HelloTriangle.h"
#include "Tools/MyExternals.h"
#include <iostream>

namespace app
{
	HelloTriangle::HelloTriangle()
	{
		std::vector<float> data = {
			200.0f, -100.0f,
			-200.0f,-100.0f,
			0.0f,   200.0f
		};
		m_VB = new VertexBuffer(data.data(),sizeof(float) * data.size());
		unsigned int indicies[] = {
			0,1,2
		};
		m_IB = new IndexBuffer(indicies, 3);
		VertexBufferLayout layout;
		layout.Push<float>(2);

		m_VA = new VertexArray();
		m_VA->AddBuffer(*m_VB, layout);

		m_Shader = new Shader("Resources/Shaders/Color.shader");
	}
	HelloTriangle::~HelloTriangle()
	{
		delete m_Shader, m_VA, m_IB, m_VB;
	}
	void HelloTriangle::OnUpdate() {
		projMatrix = glm::ortho(-(float)glfwWindowWidth / 2, (float)glfwWindowWidth / 2,
			-(float)glfwWindowHeight / 2, (float)glfwWindowHeight / 2);
		color.x = glm::sin(currentTime);
		color.y = glm::cos(currentTime);
		color.z = color.x*color.y;
		color.w = 1.0f;
	}
	void HelloTriangle::OnRender() {
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", projMatrix);
		m_Shader->SetUniformVec4("u_Color", color);
		renderer.DrawTriangles(*m_VA, *m_IB, *m_Shader);
	}
	void HelloTriangle::OnImGuiRender() {

	}
}