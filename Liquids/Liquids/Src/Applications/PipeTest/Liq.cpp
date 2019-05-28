#include "Liq.h"
#include <iostream>

namespace app
{
	Liq::Liq() :
		nParticles(0),nPipes(0),k(2000.f),pr(1000.f),pRadius(0.005f),mass(0.02f),
		viscosity(3000.f),startingParticles(4000),gravity(glm::vec2(0, -9806.65) ){
		projection = glm::ortho(-1.f,1.f,-1.f,1.f);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);

		initParticles();
		initPipes();
	}
	Liq::~Liq(){
		
	}
	void Liq::OnUpdate(){
		if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT] || mouseButtons[GLFW_MOUSE_BUTTON_RIGHT])
		{
			glm::vec2 pos = getWorldPos();
			particles->Append(new Particle(glm::vec2(pos)), sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
			mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = false;
		}

		cellsys->SortParticles(*particles,nParticles,getDX());

		computeDP();
		computeForces();
		integrate();
	}
	void Liq::OnRender(){
		GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glLineWidth(1.0f);
		renderParticles();
		//renderPipes();
	}
	void Liq::OnImGuiRender(){
		ImGui::Text("nParticles: %d", nParticles);
		ImGui::InputFloat("Stiffness", &k);
		ImGui::InputFloat("Resting Density", &pr);
		ImGui::InputFloat("ParticleRadius", &pRadius);
		ImGui::InputFloat("Mass", &mass);
		ImGui::InputFloat("Viscosity", &viscosity);
		if (ImGui::Button("Reset"))
			initParticles();
		ImGui::InputInt("startingParticles", &startingParticles);
		ImGui::SliderFloat("Gravity X: ", &gravity.x, -10000.f, 10000.f);
		ImGui::SliderFloat("Gravity Y: ", &gravity.y, -10000.f, 10000.f);
	
	}
	void Liq::FreeGuiRender(){
		
	}
	void Liq::renderPipes()
	{
		glLineWidth(20.f);
		pipeRenderer->Bind();
		pipeRenderer->SetUniformMat4f("u_MVP", projection);
		pipeRenderer->SetUniform4f("u_Color", 1.0, 1.0, 1.0, 1.0);
		for (uint i = 0; i < nPipes; i++)
		{
			renderer.DrawLineStrip(*(pipesVA[i]), *pipeRenderer,
				((pipesVB[i]->GetSize()) / (sizeof(float) * 2)));
			pipesVA[i]->Unbind();
			pipesVB[i]->Unbind();
		}
	}
	void Liq::initPipes()
	{
		pipes = std::make_unique<SSBO>(nullptr, sizeof(Pipe)*MAX_PIPES);

		pipeRenderer = std::make_unique<Shader>("Resources/Shaders/Color.shader");
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
			pipesVB[i] = std::make_unique<VertexBuffer>(points.data(), sizeof(float)*points.size());
			pipesVA[i] = std::make_unique<VertexArray>();
			pipesVA[i]->AddBuffer(*(pipesVB[i]), layout);
			pipesVB[i]->Unbind();
			pipesVA[i]->Unbind();
		}
	}
	void Liq::initParticles(){
		nParticles = 0;
		particles = std::make_unique<SSBO>(nullptr, sizeof(Particle) * MAX_PARTICLES);
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		layout.Push<float>(2);
		layout.Push<float>(2);
		
		for (auto i = 0, x = 0, y = 0; i < startingParticles; i++)
		{
			particles->Append(new Particle(glm::vec2(-0.625f + SPH_PARTICLE_RADIUS * 2 * x, 1 - SPH_PARTICLE_RADIUS * 2 * y)),
				sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
			x++;
			if (x >= 125)
			{
				x = 0;
				y++;
			}
		}

		particles->SetLayout(layout);

		PR = std::make_unique<Shader>("Resources/Shaders/Particle.shader");
		DP = std::make_unique<Shader>("Resources/ParticleComputes/DP.shader");
		Force = std::make_unique<Shader>("Resources/ParticleComputes/Forces.shader");
		Integrator = std::make_unique<Shader>("Resources/ParticleComputes/Integration.shader");
		initCells();
	}
	void Liq::initCells() {
		cellsys = std::make_unique<CellSystem>(2 / (4 * pRadius), 2 / (4 * pRadius), 4 * pRadius);
	}
	void Liq::renderParticles(){
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", projection);
		PR->SetUniform1f("radius", 2000.f * pRadius);
		PR->SetUniform4f("u_Color", 0.f, 0.f, 1.0f, 1.0f);
		renderer.DrawPoints(*particles, *PR, nParticles);
	}
	void Liq::computeDP() {
		DP->BindSSBO(*particles, "Data", 0);
		DP->SetUniform1i("nParticles", nParticles);
		DP->SetUniform1f("k", k);
		DP->SetUniform1f("pr", pr);
		DP->SetUniform1f("pRadius", pRadius);
		DP->SetUniform1f("mass", mass);
		DP->DispatchCompute(getDX(), 1, 1);
	}
	void Liq::computeForces() {
		Force->BindSSBO(*particles, "Data", 0);
		Force->SetUniform1i("nParticles", nParticles);
		Force->SetUniform1f("viscosity", viscosity);
		Force->SetUniform1f("mass", mass);
		Force->SetUniform1f("pRadius", pRadius);
		Force->SetUniformVec2("gravity", gravity);
		Force->DispatchCompute(getDX(), 1, 1);
	}
	void Liq::integrate() {
		Integrator->BindSSBO(*particles, "Data", 0);
		Integrator->SetUniform1i("nPipes", nPipes);
		Integrator->BindSSBO(*pipes, "Pipes", 1);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}
}