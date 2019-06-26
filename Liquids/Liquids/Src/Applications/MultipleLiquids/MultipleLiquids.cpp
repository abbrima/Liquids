#include "Applications/MultipleLiquids/MultipleLiquids.h"
#include <iostream>

namespace app {

	MultipleLiquids::MultipleLiquids()
		:projection(glm::ortho(-1.f, 1.f, -1.f, 1.f)),
		nParticles(0),
		gravity(glm::vec2(0.f, -9806.65f)),
		startingParticles(10000) 
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);

		float* farr = new float[4];
		farr[0] = 4 * SPH_PARTICLE_RADIUS;
		farr[1] = 315.f / (64.f * glm::pi<float>() * pow(farr[0], 9));
		farr[2] = -45.f / (glm::pi<float>() * pow(farr[0], 6));
		farr[3] = farr[2] * -1;

		constants = std::make_unique<UBO>(farr, sizeof(float) * 4);

		initParticles();
	}
	MultipleLiquids::~MultipleLiquids() {

	}
	void MultipleLiquids::OnUpdate() {

		if (keys[GLFW_KEY_W])
		{
			glm::vec2 pos= getWorldPos();
			Emitter emitter(pos); emitter.EmitLiquidIntoSSBO<AdvancedParticle>(200, nParticles, *particles, LiquidType::Water);
			keys[GLFW_KEY_W] = false;
		}
		if (keys[GLFW_KEY_O])
		{
			glm::vec2 pos = getWorldPos();
			Emitter emitter(pos); emitter.EmitLiquidIntoSSBO<AdvancedParticle>(200, nParticles, *particles, LiquidType::Oil);
			keys[GLFW_KEY_O] = false;
		}
		cellsys->Sort();

		computeDP();
		computeForces();
		integrate();
		
	}
	void MultipleLiquids::OnRender() {
		GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		renderParticles();
	}
	void MultipleLiquids::OnImGuiRender() {
		ImGui::Text("nParticles: %d", nParticles);
		if (ImGui::Button("Reset"))
			initParticles();
	}
	void MultipleLiquids::FreeGuiRender() {

	}
	void MultipleLiquids::initParticles() {
		nParticles = 0;
		particles = std::make_unique<SSBO>(nullptr, sizeof(AdvancedParticle)*MAX_PARTICLES);

		particles->SetLayout(AdvancedParticle::GetLayout());

		PR = std::make_unique<Shader>("Resources/MultipleLiquids/Particle.shader");
		DP = std::make_unique<Shader>("Resources/MultipleLiquids/DP.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Forces = std::make_unique<Shader>("Resources/MultipleLiquids/Forces.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Integrator = std::make_unique<Shader>("Resources/MultipleLiquids/Integration.shader", DSIZE(PARTICLE_DISPATCH_SIZE));

		cellsys = std::make_unique<CellSystem>(2 / (4 * SPH_PARTICLE_RADIUS), 2 / (4 * SPH_PARTICLE_RADIUS)
			, 4 * SPH_PARTICLE_RADIUS, *particles, nParticles, "APSorting");
	}
	void MultipleLiquids::renderParticles() {
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", projection);
		PR->SetUniform1ui("nParticles", nParticles);
		PR->SetUniform1f("radius", 2000.f * SPH_PARTICLE_RADIUS);

		renderer.DrawPoints(*particles, *PR, nParticles);
	}
	void MultipleLiquids::computeDP() {
		DP->BindSSBO(*particles, "Data", 0);
		DP->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*DP);
		DP->SetUniform1i("nParticles", nParticles);
		DP->SetUniform1ui("width", cellsys->GetWidth());
		DP->SetUniform1ui("height", cellsys->GetHeight());
		DP->DispatchCompute(getDX(), 1, 1);
	}
	void MultipleLiquids::computeForces() {
		Forces->BindSSBO(*particles, "Data", 0);
		Forces->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*Forces);
		Forces->SetUniform1ui("width", cellsys->GetWidth());
		Forces->SetUniform1ui("height", cellsys->GetHeight());
		Forces->SetUniform1i("nParticles", nParticles);
		Forces->SetUniformVec2("gravity", gravity);
		Forces->SetUniform1b("enableInteraction", mouseButtons[GLFW_MOUSE_BUTTON_LEFT]);
		Forces->SetUniformVec2("worldPos", getWorldPos());
		Forces->SetUniform1f("interactionForce", 100000);
		Forces->DispatchCompute(getDX(), 1, 1);
	}
	void MultipleLiquids::integrate() {
		Integrator->BindSSBO(*particles, "Data", 0);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}

}