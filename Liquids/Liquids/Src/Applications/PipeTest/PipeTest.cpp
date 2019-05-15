#include "PipeTest.h"
#include <iostream>

namespace app
{
	PipeTest::PipeTest() :
		nParticles(0),nPipes(0),timeFactor(0.005f), kd(0.01f), stiffness(50),
		nearStiffness(50), restDensity(0.1), gravity(-0.98f),
		linVis(0.1), quadVis(0.2),lineWidth(20.f)
	{
		projection = glm::ortho(-1, 1, -1, 1);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);

		initParticles();
		initPipes();
	}
	void PipeTest::initParticles()
	{
		particles = new SSBO(nullptr, sizeof(Particle) * MAX_PARTICLES);
		
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
	void PipeTest::initPipes()
	{
		pipes = new SSBO(nullptr, sizeof(Pipe)*MAX_PIPES);

		pipeRenderer = new Shader("Resources/Shaders/Color.shader");
		Pipe* arr[MAX_PIPES];
		arr[nPipes] = new Pipe(1.f, 5.f, -0.5f, -0.5f, 1.f);
		arr[nPipes++]->setConstraints(-1.f, -1.f, 1.f, 1.f);
		arr[nPipes] = new Pipe(1.f, 5.f, -0.4f, -0.4f, 1.f);
		arr[nPipes++]->setConstraints(-1.f, -1.f, 1.f, 1.f);
		for (uint i = 0; i < nPipes; i++)
		{
			pipes->Append(arr[i], sizeof(Pipe), i * sizeof(Pipe));
			VertexBufferLayout layout;
			layout.Push<float>(2);
			std::vector<float> points;
			points.clear();
			bool lasty = false;
			for (float x = arr[i]->lowX; x <= arr[i]->highX; x += 0.01)
			{
				float y = arr[i]->f(x);
				if (y < -1)
					continue;
				if (y > 1 && lasty)
					break;
				if (y > 1 && !lasty)
					lasty = true;
				points.push_back(x);
				points.push_back(y);
			}
			pipesVB[i] = new VertexBuffer(points.data(), sizeof(float)*points.size());
			pipesVA[i] = new VertexArray();
			pipesVA[i]->AddBuffer(*(pipesVB[i]), layout);
			pipesVB[i]->Unbind();
			pipesVA[i]->Unbind();
		}
	}
	PipeTest::~PipeTest()
	{

	}
	void PipeTest::OnUpdate()
	{
		if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
		{
			glm::vec3 pos = getWorldPos();
			particles->Append(new Particle(pos.x, pos.y), sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
		}
		computeChanges();
	}
	void PipeTest::OnImGuiRender()
	{
		ImGui::Text("Number of Particles: %d", nParticles);
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
		ImGui::SliderFloat("Pipe Thickness", &lineWidth, 5.0f, 40.f);
	}
	void PipeTest::FreeGuiRender()
	{
		
	}
	void PipeTest::OnRender()
	{
		GLCall(glClearColor(0.7, 0.7, 0.7, 1.0));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		glLineWidth(1.0f);
		renderParticles();
		renderPipes();
	}
	void PipeTest::renderParticles()
	{
		particleRenderer->Bind();
		particleRenderer->SetUniformMat4f("u_MVP", projection);
		particleRenderer->SetUniform4f("u_Color", 0.0, 0.0, 1.0, 1.0);
		particleRenderer->SetUniform1f("radius", 15);

		renderer.DrawPoints(*particles, *particleRenderer, nParticles);
	}
	void PipeTest::renderPipes()
	{
		glLineWidth(lineWidth);
		pipeRenderer->Bind();
		pipeRenderer->SetUniformMat4f("u_MVP", projection);
		pipeRenderer->SetUniform4f("u_Color", 1.0, 1.0, 1.0, 1.0);
		for (uint i = 0; i < nPipes; i++)
		{
			renderer.DrawLineStrip(*(pipesVA[i]), *pipeRenderer,
				((pipesVB[i]->GetSize()) / (sizeof(float)*2)));
			pipesVA[i]->Unbind();
			pipesVB[i]->Unbind();
		}
	}
	void PipeTest::computeChanges()
	{
		applyExternalForces();
		//calculateViscosity();
		advect();
		findNeighbors();
		calculateDP();
		displace();
		resolveCollisions();
	}
	void PipeTest::calculateDP()
	{
		pdCalculator->BindSSBO(*particles, "Data", 2);
		pdCalculator->SetUniform1f("restDensity", restDensity);
		pdCalculator->SetUniform1f("stiffness", stiffness);
		pdCalculator->SetUniform1f("nearStiffness", nearStiffness);
		pdCalculator->SetUniform1f("k", kd);
		pdCalculator->DispatchCompute(getDX(), 1, 1);
	}
	void PipeTest::findNeighbors()
	{
		neighborFinder->BindSSBO(*particles, "Data", 2);
		neighborFinder->SetUniform1f("k", kd);
		neighborFinder->SetUniform1i("nParticles", nParticles);
		neighborFinder->DispatchCompute(getDX(), 1, 1);
	}
	void PipeTest::calculateViscosity()
	{
		viscosityCalculator->BindSSBO(*particles, "Data", 2);
		viscosityCalculator->SetUniform1f("k", kd);
		viscosityCalculator->SetUniform1f("linVis", linVis);
		viscosityCalculator->SetUniform1f("quadVis", quadVis);
		viscosityCalculator->SetUniform1f("deltaTime", timeFactor);
		viscosityCalculator->DispatchCompute(getDX(), 1, 1);
	}
	void PipeTest::advect()
	{
		advector->BindSSBO(*particles, "Data", 2);
		advector->SetUniform1f("deltaTime", timeFactor);
		advector->DispatchCompute(getDX(), 1, 1);
	}
	void PipeTest::applyExternalForces()
	{
		externalForces->BindSSBO(*particles, "Data", 2);
		externalForces->SetUniform1f("gravity", gravity);
		externalForces->SetUniform1f("deltaTime", timeFactor);
		externalForces->DispatchCompute(getDX(), 1, 1);
	}
	void PipeTest::displace()
	{
		displacor->BindSSBO(*particles, "Data", 2);
		displacor->SetUniform1f("deltaTime", timeFactor);
		displacor->SetUniform1f("k", kd);
		displacor->DispatchCompute(getDX(), 1, 1);
	}
	void PipeTest::resolveCollisions()
	{
		collisionResolver->BindSSBO(*particles, "Data", 2);
		collisionResolver->BindSSBO(*pipes, "Pipes", 3);
		collisionResolver->SetUniform1i("nPipes", nPipes);
		collisionResolver->DispatchCompute(getDX(), 1, 1);
	}
}