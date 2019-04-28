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
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-200, 0, 0));


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

		glm::vec3 worldPos = getWorldPos();
		if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT] && worldPos.x > -200 && worldPos.x < 200 && worldPos.y>-300)
		{
			Particle p(worldPos.x, worldPos.y);
			particles->Append(&p, sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
			particles->Append(&p, sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
			particles->Append(&p, sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
			//mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = false;
		}

		if (deltaTime < 0)
			std::cout << "DELTAFUCK" << std::endl;


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

	}
	void Bowl::FreeGuiRender()
	{
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
		wallShader->SetUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);
		wallShader->SetUniformMat4f("u_MVP", projection * model);
		renderer.DrawTriangles(*wallVA, *wallIB, *wallShader);
		wallVA->Unbind();
		wallIB->Unbind();
	}
	void Bowl::initParticles()
	{
		out = new SSBO(NULL, sizeof(Output) * 100000);

		particles = new SSBO(NULL, sizeof(Particle) * 100000);
		VertexBufferLayout layout;
		layout.Push<float>(2);    //Position
		layout.Push<float>(2);    //Previous Position
		layout.Push<float>(2);    //Velocity
		layout.Push<float>(4);    // pressure and density values

		short neibyfour = MAX_NEIGHBORS / 4;
		short remainder = MAX_NEIGHBORS % 4;

		for (short i = 0; i < neibyfour; i++)
			layout.Push<int>(4);
		if (remainder > 0)
			layout.Push<int>(remainder);
		particles->SetLayout(layout);


		particleRenderer = new Shader("Resources/Shaders/Particle.shader");
		neighborFinder = new Shader("Resources/ParticleComputes/Neighbors.shader");
		pdCalculator = new Shader("Resources/ParticleComputes/DensityPressure.shader");
		displacor = new Shader("Resources/ParticleComputes/Displacement.shader");
		viscosityCalculator = new Shader("Resources/ParticleComputes/Viscosity.shader");
		advector = new Shader("Resources/ParticleComputes/Advection.shader");
		externalForces = new Shader("Resources/ParticleComputes/ExternalForces.shader");
		collisionResolver = new Shader("Resources/ParticleComputes/Collision.shader");
	}
	void Bowl::renderParticles()
	{
		particleRenderer->Bind();
		particleRenderer->SetUniformMat4f("u_MVP",projection * model);
		particleRenderer->SetUniform4f("u_Color", 0.0, 0.0, 1.0, 1.0);
		particleRenderer->SetUniform1f("radius", kd*1.5);

		renderer.DrawPoints(*particles, *particleRenderer, nParticles);
	}

	glm::vec3 Bowl::getWorldPos()
	{
		double x = 2.0 * xPos / glfwWindowWidth - 1;
		double y = 2.0 * yPos / glfwWindowHeight - 1;
		glm::vec4 screenPos = glm::vec4(x, -y, -1.0, 1.0f);

		//    glm::mat4 ProjectView = GlobalProjection * GlobalView;
		glm::mat4 inv = glm::inverse(projection);
		glm::vec4 worldpos = inv * screenPos;
		worldpos.x += 200;
		return glm::vec3(worldpos);
	}

	void Bowl::computeChanges()
	{
		applyExternalForces();
		calculateViscosity();
		advect();
		findNeighbors();
		calculateDP();
		displace();
		resolveCollisions();
	}
	void Bowl::calculateDP()
	{
		pdCalculator->BindSSBO(*particles, "Data", 2);
		pdCalculator->SetUniform1f("restDensity", restDensity);
		pdCalculator->SetUniform1f("stiffness", stiffness);
		pdCalculator->SetUniform1f("nearStiffness", nearStiffness);
		pdCalculator->SetUniform1f("k", kd);
		pdCalculator->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::findNeighbors()
	{
		neighborFinder->BindSSBO(*particles, "Data", 2);
		neighborFinder->SetUniform1f("k", kd);
		neighborFinder->SetUniform1i("nParticles", nParticles);
		neighborFinder->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::calculateViscosity()
	{
		viscosityCalculator->BindSSBO(*particles, "Data", 2);
		viscosityCalculator->SetUniform1f("k", kd);
		viscosityCalculator->SetUniform1f("linVis", linVis);
		viscosityCalculator->SetUniform1f("quadVis", quadVis);
		viscosityCalculator->SetUniform1f("deltaTime", timeFactor * deltaTime);
		viscosityCalculator->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::advect()
	{
		advector->BindSSBO(*particles, "Data", 2);
		advector->SetUniform1f("deltaTime", timeFactor* deltaTime);
		advector->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::applyExternalForces()
	{
		externalForces->BindSSBO(*particles, "Data", 2);
		externalForces->SetUniform1f("gravity", gravity);
		externalForces->SetUniform1f("deltaTime", timeFactor* deltaTime);
		externalForces->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::displace()
	{
		displacor->BindSSBO(*particles, "Data", 2);
		displacor->SetUniform1f("deltaTime", timeFactor * deltaTime);
		displacor->SetUniform1f("k", kd);
		displacor->DispatchCompute(getDX(),1,1);
	}
	void Bowl::resolveCollisions()
	{
		collisionResolver->BindSSBO(*particles,"Data",2);
		collisionResolver->DispatchCompute(getDX(), 1, 1);
	}
}