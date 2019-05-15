#include "Bowl.h"
#include "Tools/MyExternals.h"
#include <iostream>

namespace app
{
	Bowl::Bowl()
		:nParticles(0),timeFactor(0.01f),kd(0.01f),stiffness(50)
		,nearStiffness(50),restDensity(0.1),gravity(-0.98f)
		,linVis(0.1),quadVis(0.2)
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);


		initParticles();
	}
	Bowl::~Bowl()
	{
	}
	void Bowl::OnUpdate()
	{
		//Update Projection Matrix
		projection = glm::ortho(-1, 1, -1, 1);
		glm::vec3 worldPos = getWorldPos();
		if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT] && worldPos.x > -200 && worldPos.x < 200 && worldPos.y>-300)
		{
			Particle p(worldPos.x, worldPos.y);
			particles->Append(&p, sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
		}

		
		computeChanges();
	}
	void Bowl::OnRender()
	{
		GLCall(glClearColor(0.7, 0.7, 0.7, 1.0));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		renderParticles();
	}
	void Bowl::OnImGuiRender()
	{
		ImGui::Text("Number of Particles: %d", nParticles);

		if (ImGui::Button("Left"))
			emitLeft();
		if (ImGui::Button("Center"))
			emitCenter();
		if (ImGui::Button("Right"))
			emitRight();
		if (ImGui::Button("Sides"))
		{
			emitRight(); emitLeft();
		}
		if (ImGui::Button("Bullet"))
		{
			particles->Append(new Particle(-0.8f, 0.5f, 10.f, 0.0f), sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
		}
		ImGui::InputFloat("Stiffness: ", &stiffness);
		ImGui::InputFloat("Near Stiffness: ", &nearStiffness);
		ImGui::InputFloat("Rest Density: ", &restDensity);
		ImGui::SliderFloat("linVis: ", &linVis, 0.0f, 1.0f);
		ImGui::SliderFloat("quadVis: ", &quadVis, 0.0f, 1.0f);
		mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = false;
	}
	void Bowl::FreeGuiRender()
	{/*
		ImGui::Begin("DATA");

		void* p = particles->GetData();
		Particle* ptr = (Particle*)p;
		for (int i = 0; i < nParticles; i++)
		{
			ImGui::Text("Pos.x: %8.2f Pos.y: %8.2f x: %6.2f y: %6.2f    %6.2f %6.2f  %8.2f %8.2f",ptr->position.x,ptr->position.y, ptr->velocity.x, ptr->velocity.y,
				ptr->density, ptr->nearDensity, ptr->pressure, ptr->nearPressure);
			ptr++;
		}
		particles->Unmap();

		ImGui::End();
		*/
	}
	void Bowl::initParticles()
	{

		particles = new SSBO(NULL, sizeof(Particle) * MAX_PARTICLES);
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
		particleRenderer->SetUniformMat4f("u_MVP",projection);
		particleRenderer->SetUniform4f("u_Color", 0.0, 0.0, 1.0, 1.0);
		particleRenderer->SetUniform1f("radius", 15);

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
		return glm::vec3(worldpos);
	}

	void Bowl::computeChanges()
	{
		applyExternalForces();
		//calculateViscosity();
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
		viscosityCalculator->SetUniform1f("deltaTime", timeFactor);
		viscosityCalculator->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::advect()
	{
		advector->BindSSBO(*particles, "Data", 2);
		advector->SetUniform1f("deltaTime", timeFactor);
		advector->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::applyExternalForces()
	{
		externalForces->BindSSBO(*particles, "Data", 2);
		externalForces->SetUniform1f("gravity", gravity);
		externalForces->SetUniform1f("deltaTime", timeFactor);
		externalForces->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::displace()
	{
		displacor->BindSSBO(*particles, "Data", 2);
		displacor->SetUniform1f("deltaTime", timeFactor);
		displacor->SetUniform1f("k", kd);
		displacor->DispatchCompute(getDX(),1,1);
	}
	void Bowl::resolveCollisions()
	{
		collisionResolver->BindSSBO(*particles,"Data",2);
		collisionResolver->DispatchCompute(getDX(), 1, 1);
	}
	void Bowl::emitCenter()
	{
		for (float i = -0.9f; i < 0.9f; i += 0.03f)
			for (float j = -0.5f; j < 0.5f; j += 0.03f)
			{
				particles->Append(new Particle(j, i), sizeof(Particle), nParticles * sizeof(Particle));
				nParticles++;
			}
	}
	void Bowl::emitLeft()
	{
		for (float i = -0.9f; i < 0.9f; i += 0.03f)
			for (float j = -0.9f; j < -0.2f; j += 0.03f)
			{
				particles->Append(new Particle(j, i), sizeof(Particle), nParticles * sizeof(Particle));
				nParticles++;
			}
	}
	void Bowl::emitRight()
	{
		for (float i = -0.9f; i < 0.9f; i += 0.03f)
			for (float j = 0.2f; j < 0.9f; j += 0.03f)
			{
				particles->Append(new Particle(j, i), sizeof(Particle), nParticles * sizeof(Particle));
				nParticles++;
			}
	}
}