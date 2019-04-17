#include "Applications/Application.h"

#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Renderer.h"

namespace app
{
	class HelloTriangle : public Application
	{
	private:
		VertexBuffer *m_VB;
		IndexBuffer *m_IB;
		VertexArray *m_VA;
		Shader *m_Shader;
		Renderer renderer;
		glm::mat4 projMatrix;
		glm::vec4 color;
		SSBO* ssbo;

	public:
		HelloTriangle();
		~HelloTriangle();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;

	};
}