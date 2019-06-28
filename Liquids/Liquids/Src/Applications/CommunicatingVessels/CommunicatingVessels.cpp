#include "Applications/CommunicatingVessels/CommunicatingVessels.h"
#include <memory>

namespace app
{
	CommunicatingVessels::CommunicatingVessels()
		:projection(glm::ortho(-1.f, 1.f, -1.f, 1.f)),
		nParticles(0),
		k(2000.f),
		viscosity(3000.f),
		mass(0.02f),
		pr(1000.f),
		gravity(glm::vec2(0.f, -9806.65f)),
		nVerticalPipes(0),
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

		initVerticalPipes(true);
		initParticles();
	}
	CommunicatingVessels::~CommunicatingVessels() {

	}
	void CommunicatingVessels::OnUpdate() {
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
	void CommunicatingVessels::OnRender() {
		GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		renderParticles();
		renderPipes();
	}
	void CommunicatingVessels::OnImGuiRender() {
		ImGui::Text("nParticles: %d", nParticles);
		if (ImGui::Button("Reset"))
			initParticles();
		if (ImGui::Button("OpenBarrier"))
			initVerticalPipes(false);
		if (ImGui::Button("CloseBarrier"))
			initVerticalPipes(true);
	}
	void CommunicatingVessels::FreeGuiRender() {

	}
	void CommunicatingVessels::initParticles() {
		nParticles = 0;

		particles = std::make_unique<SSBO>(nullptr, sizeof(NormalParticle)*MAX_PARTICLES);
		particles->SetLayout(NormalParticle::GetLayout());

		PR = std::make_unique<Shader>("Resources/CommunicatingVessels/Particle.shader");
		DP = std::make_unique<Shader>("Resources/CommunicatingVessels/DP.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Forces = std::make_unique<Shader>("Resources/CommunicatingVessels/Forces.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Integrator = std::make_unique<Shader>("Resources/CommunicatingVessels/Integration.shader", DSIZE(PARTICLE_DISPATCH_SIZE));

		cellsys = std::make_unique<CellSystem>(2 / (4 * SPH_PARTICLE_RADIUS), 2 / (4 * SPH_PARTICLE_RADIUS)
			, 4 * SPH_PARTICLE_RADIUS, *particles, nParticles, "NPSorting");
	}
	void CommunicatingVessels::initVerticalPipes(const bool& close) {
		nVerticalPipes = 0;
		VerticalPipe* arr[MAX_PIPES];
		if (close)
		    arr[nVerticalPipes++] = new VerticalPipe(0.f, -2.f, 0.2f, 1.f);
		else
			arr[nVerticalPipes++] = new VerticalPipe(0.f, -0.9f, 0.2f, 1.f);
		verticalpipes = std::make_unique<SSBO>(nullptr, sizeof(VerticalPipe)*MAX_PIPES);
		std::vector<glm::vec2> points;
		for (uint i = 0; i < nVerticalPipes; i++)
		{
			points.emplace_back(glm::vec2(arr[i]->x,arr[i]->lowY));
			points.emplace_back(glm::vec2(arr[i]->x, arr[i]->highY));
			verticalpipes->Append(arr[i], sizeof(VerticalPipe), sizeof(VerticalPipe)*i);
		}
		VerticalPipesVB = std::make_unique<VertexBuffer>(points.data(), sizeof(glm::vec2)*points.size());
		VerticalPipesVA = std::make_unique<VertexArray>();
		VertexBufferLayout layout; layout.Push<float>(2); VerticalPipesVA->AddBuffer(*VerticalPipesVB, layout);
		PipesShader = std::make_unique<Shader>("Resources/Shaders/Color.shader");
		VerticalPipesVB->Unbind();
		VerticalPipesVA->Unbind();
	}
	void CommunicatingVessels::renderPipes() {
		PipesShader->Bind();

		GLCall(glLineWidth(20.f));

		PipesShader->SetUniformMat4f("u_MVP", projection);
		PipesShader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

		renderer.DrawLines(*VerticalPipesVA, *PipesShader, VerticalPipesVB->GetSize() / (sizeof(float)*2));

		VerticalPipesVA->Unbind();
		VerticalPipesVB->Unbind();
	}
	void CommunicatingVessels::renderParticles() {
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", projection);
		PR->SetUniform1ui("nParticles", nParticles);
		PR->SetUniform1f("radius", 2.f * SPH_PARTICLE_RADIUS);
		PR->SetUniform3f("u_Color", 0.0f, 0.0f, 1.0f);

		renderer.DrawPoints(*particles, *PR, nParticles);
	}
	void CommunicatingVessels::computeDP() {
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
	void CommunicatingVessels::computeForces() {
		Forces->BindSSBO(*particles, "Data", 0);
		Forces->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*Forces);
		Forces->SetUniform1ui("width", cellsys->GetWidth());
		Forces->SetUniform1ui("height", cellsys->GetHeight());
		Forces->SetUniform1i("nParticles", nParticles);
		Forces->SetUniform1f("viscosity", viscosity);
		Forces->SetUniform1f("mass", mass);
		Forces->SetUniformVec2("gravity", gravity);
		Forces->DispatchCompute(getDX(), 1, 1);
	}
	void CommunicatingVessels::integrate() {
		Integrator->BindSSBO(*particles, "Data", 0);
		Integrator->BindSSBO(*verticalpipes, "Pipes", 1);
		Integrator->SetUniform1ui("nPipes", nVerticalPipes);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}
}