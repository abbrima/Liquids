#include "Liq.h"
#include <iostream>

namespace app
{
	Liq::Liq() :
		nParticles(0), lineWidth(20.f),k(5000.f),radius(1.f),pr(1.f),viscosity(0.018),mass(100.f){
		projection = glm::ortho(-100.f,100.f,-100.f,100.f);
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
			particles->Append(new Particle(glm::vec2(pos), mass), sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
			mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = false;
		}

		computeDP();
		computeForces();
		//integrate();
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
		ImGui::Begin("Data");
		void* p = particles->GetData();
		Particle *ptr = (Particle*)p;
		for (uint i = 0; i < nParticles; i++)
		{
			ImGui::Text("%10.5f  %10.5f  %10.5f", ptr->density, ptr->pressure, ptr->mass); ptr++;
		}
		particles->Unmap();
		ImGui::End();
	}
	void Liq::initParticles(){
		particles = new SSBO(nullptr, MAX_PARTICLES * sizeof(Particle));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		layout.Push<float>(4);
		layout.Push<float>(4);
		particles->SetLayout(layout);
		PR = new Shader("Resources/Shaders/Particle.shader");
		DP = new Shader("Resources/ParticleComputes/DP.shader");
		Forces = new Shader("Resources/ParticleComputes/Forces.shader");
		Integrator = new Shader("Resources/ParticleComputes/Integration.shader");
	}
	void Liq::renderParticles(){
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", projection);
		PR->SetUniform1f("radius", 15.f);
		PR->SetUniform4f("u_Color", 1.0f, 1.0f, 0.3f, 1.0f);
		renderer.DrawPoints(*particles, *PR, nParticles);
	}
	void Liq::computeDP() {
		DP->BindSSBO(*particles, "Data", 2);
		DP->SetUniform1f("k", k);
		DP->SetUniform1f("pr", pr);
		DP->SetUniform1i("nParticles", nParticles);
		DP->SetUniform1f("radius", radius);
		DP->DispatchCompute(getDX(), 1, 1);
	}
	void Liq::computeForces() {
		Forces->BindSSBO(*particles, "Data", 2);
		Forces->SetUniform1f("radius", radius);
		Forces->SetUniform1i("nParticles", nParticles);
		Forces->SetUniform1f("viscosity", viscosity);
		Forces->DispatchCompute(getDX(), 1, 1);
	}
	void Liq::integrate() {
		Integrator->BindSSBO(*particles, "Data", 2);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}
}