#include "Applications/Hello Triangle/HelloTriangle.h"
#include "Tools/MyExternals.h"
#include <iostream>

namespace app
{
	HelloTriangle::HelloTriangle():
		value(0.0f)
	{
		std::vector<float> data = {
			200.0f, -100.0f,
			-200.0f,-100.0f,
			0.0f,   200.0f
		};
		//m_VB = new VertexBuffer(data.data(),sizeof(float) * data.size());
		unsigned int indicies[] = {
			0,1,2
		};
		m_IB = new IndexBuffer(indicies, 3);
		VertexBufferLayout layout;
		layout.Push<float>(2);

		//
		ssbo = new SSBO(NULL, (uint)data.size() * sizeof(float));
		ssbo->Append(data.data(), data.size() * sizeof(float), 0);
		ssbo->SetLayout(layout);
		//

		//m_VA = new VertexArray();
		//m_VA->AddBuffer(*m_VB, layout);
		comp = new Shader("Resources/Shaders/Test.shader");
		m_Shader = new Shader("Resources/Shaders/Color.shader");
	}
	HelloTriangle::~HelloTriangle()
	{
		delete m_Shader, m_VA, m_IB, m_VB;
	}
	void HelloTriangle::OnUpdate() {
		projMatrix = glm::ortho(-(float)glfwWindowWidth / 2, (float)glfwWindowWidth / 2,
			-(float)glfwWindowHeight / 2, (float)glfwWindowHeight / 2);
		color = glm::vec4(1.0f);
		/*void* data = ssbo->GetData();
		float* ptr = (float*)data;
		for (int i = 0; i < 6; i++)
		{
			*ptr += 0.1f;
			ptr++;
		}
		ssbo->Unmap();*/
		comp->BindSSBO(*ssbo, "Data", 2);
		comp->SetUniform1f("val", value);
		comp->DispatchCompute(3,1,1);

	}
	void HelloTriangle::OnRender() {
		
		
		
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", projMatrix);
		m_Shader->SetUniformVec4("u_Color", color);

		renderer.DrawTriangles(*ssbo, *m_IB, *m_Shader);
		//renderer.DrawPoints(*ssbo, *m_Shader, 3);
	}
	void HelloTriangle::OnImGuiRender() {
		ImGui::SliderFloat("Value", &value, -1.0f, 1.0f);
	}
}