#include "Liq.h"
#include <iostream>

namespace app
{
	Liq::Liq() :
		nParticles(0){
		projection = glm::ortho(-1.f,1.f,-1.f,1.f);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);

		initParticles();
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
		computeDP();
		computeForces();
		integrate();
	}
	void Liq::OnRender(){
		GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		renderParticles();
	}
	void Liq::OnImGuiRender(){
		ImGui::Text("nParticles: %d", nParticles);
	}
	void Liq::FreeGuiRender(){
		
	}
	void Liq::initParticles(){
		
		particles = new SSBO(nullptr, sizeof(Particle) * MAX_PARTICLES);
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		layout.Push<float>(2);
		layout.Push<float>(2);
		
		for (auto i = 0, x = 0, y = 0; i < 10000; i++)
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

		PR = new Shader("Resources/Shaders/Particle.shader");
		DP = new Shader("Resources/ParticleComputes/DP.shader");
		Force = new Shader("Resources/ParticleComputes/Forces.shader");
		Integrator = new Shader("Resources/ParticleComputes/Integration.shader");
	}
	void Liq::renderParticles(){
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", projection);
		PR->SetUniform1f("radius", 5.f);
		PR->SetUniform4f("u_Color", 1.0f, 1.0f, 0.3f, 1.0f);
		renderer.DrawPoints(*particles, *PR, nParticles);
	}
	void Liq::computeDP() {
		DP->BindSSBO(*particles, "Data", 0);
		DP->SetUniform1i("nParticles", nParticles);
		DP->DispatchCompute(getDX(), 1, 1);
	}
	void Liq::computeForces() {
		Force->BindSSBO(*particles, "Data", 0);
		Force->SetUniform1i("nParticles", nParticles);
		Force->DispatchCompute(getDX(), 1, 1);
	}
	void Liq::integrate() {
		Integrator->BindSSBO(*particles, "Data", 0);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}
}