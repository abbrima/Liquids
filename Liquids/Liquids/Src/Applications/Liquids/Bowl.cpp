#include "Bowl.h"
#include "Tools/MyExternals.h"
#include <iostream>

namespace app
{
	Bowl::Bowl()
		:nParticles(0), kd(20), linVis(1), quadVis(1), gravity(9.8)
		, stiffness(10), nearStiffness(10), restDensity(0.2), timeFactor(3)
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);

		initWalls();
		initParticles();
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

		glm::vec3 pos = getWorldPos();
		if (pos.x > -200 && pos.x<200 && pos.y>-300 && mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
		{
			particles->Append(new Particle(pos.x, pos.y), sizeof(Particle), nParticles++ * sizeof(Particle));
		}

		computeChanges();
	}
	void Bowl::OnRender()
	{
		GLCall(glClearColor(0.7, 0.7, 0.7, 1.0));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		renderWalls();
		renderParticles();
	}
	void Bowl::OnImGuiRender()
	{
		ImGui::Text("Number of Particles: %d", nParticles);

		ImGui::InputFloat("Stiffness", &stiffness, 0.0f, 10000.0f);
		ImGui::InputFloat("Near Stiffness", &nearStiffness, 0.0f, 1000.0f);
		ImGui::SliderFloat("Gravity", &gravity,-10.0f,10.0f);

	}
	void Bowl::FreeGuiRender()
	{
		/*
		ImGui::Begin("DATA");

		void* p = particles->GetData();
		Particle* ptr = (Particle*)p;
		for (int i = 0; i < nParticles; i++)
		{
			ImGui::Text("Pos.x: %8.2f Pos.y: %8.2f x: %6.2f y: %6.2f    %4.2f %4.2f  %8.2f %8.2f",ptr->position.x,ptr->position.y, ptr->velocity.x, ptr->velocity.y,
				ptr->density, ptr->nearDensity, ptr->pressure, ptr->nearPressure);
			ptr++;
		}
		particles->Unmap();

		ImGui::End();
		*/
		
	}
	void Bowl::initWalls()
	{
		std::vector<float> data = {
			-200.0f,300.0f,   //left  upper right
			-220.0f,300.0f,   //left  upper left
			-200.0f,-300.0f,  //left  lower right
			-220.0f,-320.0f,  //left  lower left
			200.0f,300.0f,    //right upper left
			220.0f,300.0f,    //right upper right
			200.0f,-300.0f,   //right lower left
			220.0f,-320.0f,   //right lower right

			-220.0f,-300.0f,  //left  lower left upper
			220.0f,-300.0f    //right lower right upper
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
		wallShader->SetUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);
		wallShader->SetUniformMat4f("u_MVP", projection);
		renderer.DrawTriangles(*wallVA, *wallIB, *wallShader);
		wallVA->Unbind();
		wallIB->Unbind();
	}
	void Bowl::initParticles()
	{
		particles = new SSBO(NULL, sizeof(Particle) * 100000);

		VertexBufferLayout layout;
		layout.Push<float>(2); 
		layout.Push<float>(2); 		
		layout.Push<float>(2);
		layout.Push<float>(4);
		int n = MAX_NEIGHBORS / 4;
		for (int i = 0; i < n; i++)
			layout.Push<int>(4);
		n = MAX_NEIGHBORS % 4;
		if (n > 0)
			layout.Push<int>(n);
		particles->SetLayout(layout);

		ParticleRenderer = new Shader("Resources/Shaders/Particle.shader");
		Gravity = new Shader("Resources/ParticleComputes/Gravity.shader");
		Advection = new Shader("Resources/ParticleComputes/Advection.shader");
	}
	void Bowl::renderParticles()
	{
		ParticleRenderer->Bind();
		ParticleRenderer->SetUniformMat4f("u_MVP",projection);
		ParticleRenderer->SetUniform4f("u_Color", 0.0, 0.0, 1.0, 1.0);
		ParticleRenderer->SetUniform1f("radius", kd*1.5);

		renderer.DrawPoints(*particles, *ParticleRenderer, nParticles);
	}

	glm::vec3 Bowl::getWorldPos()
	{
		double x = 2.0 * xPos / glfwWindowWidth - 1;
		double y = 2.0 * yPos / glfwWindowHeight - 1;
		glm::vec4 screenPos = glm::vec4(x, -y, -1.0, 1.0f);

		//    glm::mat4 ProjectView = GlobalProjection * GlobalView;
		glm::mat4 inv = glm::inverse(projection);
		glm::vec4 worldpos = inv * screenPos;
		return glm::vec3(worldpos);
	}

	void Bowl::computeChanges()
	{
		ApplyGravity();
		Advect();
	}
	void Bowl::ApplyGravity()
	{
		Gravity->BindSSBO(*particles, "Data", 2);
		Gravity->SetUniform1f("gravity", gravity);
		Gravity->SetUniform1f("deltaTime", deltaTime * timeFactor);

		Gravity->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::Advect()
	{
		Advection->BindSSBO(*particles, "Data", 2);
		Advection->SetUniform1f("deltaTime", deltaTime * timeFactor);

		Advection->DispatchCompute(getDX(), 1, 1);
	}
}