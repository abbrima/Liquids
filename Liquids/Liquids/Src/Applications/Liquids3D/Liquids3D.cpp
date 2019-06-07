#include "Liquids3D.h"
#include <iostream>

namespace app {
	Liquids3D::Liquids3D() :
		view(glm::mat4(1.f)),
		skyboxModel(glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 100))),
		k(2000.f),
		viscosity(3000.f),
		pr(1000.f),
		mass(0.02f),
		gravity(glm::vec3(0.f, -9806.65f,0.f))
	{
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glEnable(GL_PROGRAM_POINT_SIZE));
		GLCall(glEnable(GL_POINT_SMOOTH));
		GLCall(glEnable(GL_LINE_SMOOTH));

		skybox.ChooseBox(2);

		Camera = std::make_unique <FPSCamera>();
		Camera->SetCameraSpeed(200.0f);
		Camera->SetSensitivity(40.4f);
		Camera->SetPitch(0.0f); Camera->SetYaw(-235.0f);
		Camera->SetCameraPosition(0, 0, -10.f);
		CameraModel = glm::translate(glm::mat4(1.0f), Camera->GetCameraPosition());
		Camera->SetCameraDirection(0, 0, 0);

		float* farr = new float[4];
		farr[0] = 4 * SPH_PARTICLE_RADIUS;
		farr[1] = 315.f / (64.f * glm::pi<float>() * pow(farr[0], 9));
		farr[2] = -45.f / (glm::pi<float>() * pow(farr[0], 6));
		farr[3] = farr[2] * -1;

		constants = std::make_unique<UBO>(farr, sizeof(float) * 4);

		initParticles();
	}
	Liquids3D::~Liquids3D() {

	}
	void Liquids3D::OnUpdate() {
		if (keys[GLFW_KEY_E]) {

			Emitter3D emitter(glm::vec3(0, 0, 0));
			emitter.EmitIntoSSBO<Particle3D>(200, nParticles, *particles);
			keys[GLFW_KEY_E] = false;
		}

		Camera->Update(view);
		if (fov <= 30.0f)
			fov = 30.0f;
		projection = glm::perspective(glm::radians(fov), (float)glfwWindowWidth / (float)glfwWindowHeight, 0.1f, 100000.0f);
		CameraModel = glm::translate(glm::mat4(1.0f), Camera->GetCameraPosition());

		pv = projection * view;

		cellsys->Sort();

		computeDP();
		computeForces();
		integrate();
	}
	void Liquids3D::OnRender() {
		skybox.Render(pv * skyboxModel);
		renderParticles();
	}
	void Liquids3D::OnImGuiRender() {
		ImGui::Text("nParticles: %d", nParticles);

		Camera->OnImGuiRender();
	}
	void Liquids3D::FreeGuiRender() {
		
	}
	void Liquids3D::initParticles() {
		nParticles = 0;
		particles = std::make_unique<SSBO>(nullptr, sizeof(Particle3D)*MAX_PARTICLES);
		particles->SetLayout(Particle3D::GetLayout());

		PR = std::make_unique<Shader>("Resources/Liquids3D/Particle.shader");
		DP = std::make_unique<Shader>("Resources/Liquids3D/DP.shader",DSIZE(PARTICLE_DISPATCH_SIZE));
		Forces = std::make_unique<Shader>("Resources/Liquids3D/Forces.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Integrator = std::make_unique<Shader>("Resources/Liquids3D/Integration.shader",DSIZE(PARTICLE_DISPATCH_SIZE));

		cellsys = std::make_unique<CellSystem3D>(2 / (4 * SPH_PARTICLE_RADIUS), 2 / (4 * SPH_PARTICLE_RADIUS),2/(4*SPH_PARTICLE_RADIUS)
			, 4 * SPH_PARTICLE_RADIUS, *particles, nParticles, "3DSorting");

	}
	void Liquids3D::renderParticles() {
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", pv * glm::scale(glm::mat4(1.0f),glm::vec3(100,100,100)));
		PR->SetUniform1ui("nParticles", nParticles);
		PR->SetUniform1f("radius", 2000.f * SPH_PARTICLE_RADIUS);
		PR->SetUniform3f("u_Color", 0.0f, 0.0f, 1.0f);

		renderer.DrawPoints(*particles, *PR, nParticles);
		PR->Unbind();
		particles->Unbind();
	}
	void Liquids3D::computeDP() {
		DP->BindSSBO(*particles, "Data", 0);
		DP->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*DP);
		DP->SetUniform1f("k", k);
		DP->SetUniform1f("pr", pr);
		DP->SetUniform1f("mass", mass);
		DP->SetUniform1ui("width", cellsys->GetWidth());
		DP->SetUniform1ui("height", cellsys->GetHeight());
		DP->SetUniform1ui("depth", cellsys->GetDepth());
		DP->SetUniform1ui("nParticles", nParticles);
		DP->DispatchCompute(getDX(), 1, 1);
	}
	void Liquids3D::computeForces() {
		Forces->BindSSBO(*particles, "Data", 0);
		Forces->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*Forces);
		Forces->SetUniform1ui("width", cellsys->GetWidth());
		Forces->SetUniform1ui("height", cellsys->GetHeight());
		Forces->SetUniform1ui("depth", cellsys->GetDepth());
		Forces->SetUniform1f("viscosity", viscosity);
		Forces->SetUniform1f("mass", mass);
		Forces->SetUniformVec3("gravity", gravity);

		Forces->SetUniform1b("enableInteraction", keys[GLFW_KEY_P]);
		Forces->SetUniformVec3("worldPos", glm::vec3(0.f,0.4f,0.f));
		Forces->SetUniform1f("interactionForce", 100000);

		Forces->DispatchCompute(getDX(), 1, 1);
	}
	void Liquids3D::integrate() {
		Integrator->BindSSBO(*particles, "Data", 0);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}
}