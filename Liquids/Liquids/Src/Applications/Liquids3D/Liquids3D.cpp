#include "Liquids3D.h"
#include <iostream>

namespace app {
	Liquids3D::Liquids3D() :
		view(glm::mat4(1.f)),
		skyboxModel(glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 100)))
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
	}
	void Liquids3D::OnRender() {
		//skybox.Render(pv * skyboxModel);
		renderParticles();
	}
	void Liquids3D::OnImGuiRender() {
		Camera->OnImGuiRender();
	}
	void Liquids3D::FreeGuiRender() {
		ImGui::Begin("Data");

		Particle3D* ptr = (Particle3D*)particles->GetData();

		for (uint i = 0; i < nParticles; i++)
		{
			ImGui::Text("%f %f %f", ptr->position.x, ptr->position.y, ptr->position.z);
		}

		particles->Unmap();

		ImGui::End();

	}
	void Liquids3D::initParticles() {
		nParticles = 0;
		particles = std::make_unique<SSBO>(nullptr, sizeof(Particle3D)*MAX_PARTICLES);
		particles->SetLayout(Particle3D::GetLayout());

		PR = std::make_unique<Shader>("Resources/WaterTest/Particle.shader");
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
}