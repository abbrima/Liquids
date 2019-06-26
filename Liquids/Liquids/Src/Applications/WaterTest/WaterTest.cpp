#include "WaterTest.h"
#include <iostream>

namespace app
{
	WaterTest::WaterTest()
		:projection(glm::ortho(-1.f, 1.f, -1.f, 1.f)),
		nParticles(0),
		k(2000.f),
		viscosity(3000.f),
		pr(1000.f),
		mass(0.02f),
	    gravity(glm::vec2(0.f, -9806.65f)),
		startingParticles(4000),
		color(glm::vec3(0.f,0.f,1.f))

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
	WaterTest::~WaterTest() {

	}
	void WaterTest::OnUpdate() {
		if (mouseButtons[GLFW_MOUSE_BUTTON_RIGHT])
		{
			glm::vec2 pos = getWorldPos();
			Emitter emitter(pos); emitter.EmitIntoSSBO<NormalParticle>(200, nParticles, *particles);
			mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = false;
		}



		cellsys->Sort();

		computeDP();
		computeForces();
		integrate();

	}
	void WaterTest::OnRender() {
		GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		renderParticles();
	}
	void WaterTest::OnImGuiRender() {
		ImGui::Text("nParticles: %d", nParticles);
		ImGui::SliderFloat("Stiffness", &k,1000.f,5000.f);
		ImGui::SliderFloat("Resting Density", &pr,1000.f,5000.f);
		ImGui::SliderFloat("Mass", &mass,0.015f,0.03f);
		ImGui::SliderFloat("Viscosity", &viscosity,2500.f,3500.f);
		if (ImGui::Button("Reset"))
			initParticles();
		ImGui::InputInt("startingParticles", &startingParticles);
		ImGui::SliderFloat("Gravity X: ", &gravity.x, -10000.f, 10000.f);
		ImGui::SliderFloat("Gravity Y: ", &gravity.y, -10000.f, 10000.f);
		ImGui::ColorPicker3("Liquid Color", (float*)(&color));
	}
	void WaterTest::FreeGuiRender() {
		

	}

	void WaterTest::initParticles() {
		nParticles = 0;
		particles = std::make_unique<SSBO>(nullptr, sizeof(NormalParticle)*MAX_PARTICLES);

		particles->SetLayout(NormalParticle::GetLayout());

		PR = std::make_unique<Shader>("Resources/WaterTest/Particle.shader");
		DP = std::make_unique<Shader>("Resources/WaterTest/DP.shader",DSIZE(PARTICLE_DISPATCH_SIZE));
		Forces = std::make_unique<Shader>("Resources/WaterTest/Forces.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Integrator = std::make_unique<Shader>("Resources/WaterTest/Integration.shader", DSIZE(PARTICLE_DISPATCH_SIZE));

		cellsys = std::make_unique<CellSystem>(2 / (4 * SPH_PARTICLE_RADIUS), 2 / (4 * SPH_PARTICLE_RADIUS)
			, 4 * SPH_PARTICLE_RADIUS, *particles, nParticles, "NPSorting");

		for (auto i = 0, x = 0, y = 0; i < startingParticles; i++)
		{
			particles->Append(new NormalParticle(glm::vec2(-0.625f + SPH_PARTICLE_RADIUS * 2 * x, 1 - SPH_PARTICLE_RADIUS * 2 * y)),
				sizeof(NormalParticle), nParticles * sizeof(NormalParticle));
			nParticles++;
			x++;
			if (x >= 125)
			{
				x = 0;
				y++;
			}
		}


	}
	void WaterTest::renderParticles() {
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", projection);
		PR->SetUniform1ui("nParticles", nParticles);
		PR->SetUniform1f("radius",2* SPH_PARTICLE_RADIUS);
		PR->SetUniformVec3("u_Color", color);
		
		renderer.DrawPoints(*particles, *PR, nParticles);
	}
	void WaterTest::computeDP() {
		DP->BindSSBO(*particles, "Data", 0);
		DP->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*DP);
		DP->SetUniform1i("nParticles", nParticles);
		DP->SetUniform1f("k", k);
		DP->SetUniform1f("pr", pr);
		DP->SetUniform1f("mass", mass);
		DP->SetUniform1ui("width", cellsys->GetWidth());
		DP->SetUniform1ui("height", cellsys->GetHeight());
		DP->DispatchCompute(getDX(), 1, 1);
	}
	void WaterTest::computeForces() {
		Forces->BindSSBO(*particles, "Data", 0);
		Forces->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*Forces);
		Forces->SetUniform1ui("width", cellsys->GetWidth());
		Forces->SetUniform1ui("height", cellsys->GetHeight());
		Forces->SetUniform1i("nParticles", nParticles);
		Forces->SetUniform1f("viscosity", viscosity);
		Forces->SetUniform1f("mass", mass);
		Forces->SetUniformVec2("gravity", gravity);
		Forces->SetUniform1b("enableInteraction", keys[GLFW_KEY_P]);
		Forces->SetUniformVec2("worldPos", getWorldPos());
		Forces->SetUniform1f("interactionForce", 100000);
		Forces->DispatchCompute(getDX(), 1, 1);
	}
	void WaterTest::integrate() {
		Integrator->BindSSBO(*particles, "Data", 0);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}
}