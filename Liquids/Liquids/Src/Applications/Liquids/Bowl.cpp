#include "Bowl.h"
#include "Tools/MyExternals.h"
#include <iostream>

namespace app
{
	Bowl::Bowl()
		:nParticles(0),k(1.0f)
	{
		GLCall(glEnable(GL_PROGRAM_POINT_SIZE));


		initWalls();
	}
	Bowl::~Bowl()
	{
		delete(wallIB, wallVA, wallVB, wallShader);

	}
	void Bowl::OnUpdate()
	{
		//Update Projection Matrix
		projection = glm::ortho(-(float)glfwWindowWidth / 2, (float)glfwWindowWidth / 2,
			-(float)glfwWindowHeight / 2, (float)glfwWindowHeight / 2);

		
	}
	void Bowl::OnRender()
	{
		renderWalls();
	}
	void Bowl::OnImGuiRender()
	{
	
	}
	void Bowl::initWalls()
	{
		std::vector<float> data = {
			-200.0f,300.0f,   //left  upper right
			-210.0f,300.0f,   //left  upper left
			-200.0f,-300.0f,  //left  lower right
			-210.0f,-310.0f,  //left  lower left
			200.0f,300.0f,    //right upper left
			210.0f,300.0f,    //right upper right
			200.0f,-300.0f,   //right lower left
			210.0f,-310.0f,   //right lower right

			-210.0f,-300.0f,  //left  lower left upper
			210.0f,-300.0f    //right lower right upper
		};
		std::vector<unsigned int> indicies = {
			0,1,2,1,2,8,3,8,9,3,7,9,9,4,5,4,9,6
		};
		wallVB = new VertexBuffer(data.data(), sizeof(float)*(uint)data.size());
		wallIB = new IndexBuffer(indicies.data(), (uint)indicies.size());
		VertexBufferLayout layout; layout.Push<float>(2);
		wallVA = new VertexArray();
		wallVA->AddBuffer(*wallVB, layout);
		
		wallShader = new Shader("Resources/Shaders/Color.shader");
	}
	void Bowl::renderWalls()
	{
		wallShader->Bind();
		wallShader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
		wallShader->SetUniformMat4f("u_MVP", projection);
		renderer.DrawTriangles(*wallVA, *wallIB, *wallShader);
		wallVA->Unbind();
		wallIB->Unbind();
	}
}
