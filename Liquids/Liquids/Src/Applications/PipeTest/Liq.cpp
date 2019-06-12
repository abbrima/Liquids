#include "Liq.h"
#include <iostream>
#include <memory>
namespace app
{
	Liq::Liq() :
		nParticles(0),nPipes(0),k(5000.f),pr(1000.f),mass(0.02f),
		viscosity(3000.f),gravity(glm::vec2(0, -9806.65) ){
		projection = glm::ortho(-1.f,1.f,-1.f,1.f);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);


		test = std::make_unique<SSBO>(nullptr,MAX_PARTICLES * sizeof(uint) * 2);


		initParticles();
		initLines();
		initPipes();
	}
	Liq::~Liq(){
		
	}
	void Liq::initLines() {
		std::vector<glm::vec2> linePoints;
		for (float x = -1.f; x <= 1.f; x += 0.02f)
		{
			linePoints.emplace_back(glm::vec2(x,-1.f));
			linePoints.emplace_back(glm::vec2(x, 1.f));
		}
		for (float x = -1.f; x <= 1.f; x += 0.02f)
		{
			linePoints.emplace_back(glm::vec2(-1.f, x));
			linePoints.emplace_back(glm::vec2(1.f, x));
		}
		LinesVB = std::make_unique<VertexBuffer>(linePoints.data(), linePoints.size() * sizeof(glm::vec2));
		LinesVA = std::make_unique<VertexArray>();
		VertexBufferLayout layout;
		layout.Push<float>(2);
		LinesVA->AddBuffer(*LinesVB, layout);
		LinesSH = std::make_unique<Shader>("Resources/Shaders/Color.shader");
		LinesVB->Unbind();
		LinesVA->Unbind();
	}
	void Liq::renderLines() {
		LinesSH->Bind();
		GLCall(glLineWidth(1.f));
		LinesSH->SetUniformMat4f("u_MVP", projection);
		LinesSH->SetUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);

		renderer.DrawLines(*LinesVA, *LinesSH, LinesVB->GetSize()/sizeof(glm::vec2));
		LinesVA->Unbind();
		LinesVB->Unbind();
	}
	void Liq::OnUpdate(){
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
	void Liq::OnRender(){
		GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glLineWidth(1.0f);
		renderParticles();
		//renderLines();
		renderPipes();
	}
	void Liq::OnImGuiRender(){
	

		ImGui::Text("nParticles: %d", nParticles);
		ImGui::InputFloat("Stiffness", &k);
		ImGui::InputFloat("Resting Density", &pr);
		ImGui::InputFloat("Mass", &mass);
		ImGui::InputFloat("Viscosity", &viscosity);
		if (ImGui::Button("Reset"))
			initParticles();
		ImGui::SliderFloat("Gravity X: ", &gravity.x, -10000.f, 10000.f);
		ImGui::SliderFloat("Gravity Y: ", &gravity.y, -10000.f, 10000.f);

		glm::vec2 pos = getWorldPos();
		ImGui::Text("x: %5.3f  y: %5.3f", pos.x, pos.y);
	
	}
	void Liq::FreeGuiRender(){
		/*ImGui::Begin("DATA");
		//cellsys->GuiRender();

		float* ptr = (float*)debug->GetData();
		for (uint i = 0; i < nParticles; i++)
		{
			ImGui::Text("F(X) = %10.5f   Slope(X) = %10.5f", *ptr, *(ptr + 1));
			ptr += 2;
		}
		debug->Unmap();

		ImGui::End();*/
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
		pipes = std::make_unique<SSBO>(nullptr, sizeof(SinusoidalPipe)*MAX_PIPES);

		pipeRenderer = std::make_unique<Shader>("Resources/Shaders/Color.shader");
		SinusoidalPipe* arr[MAX_PIPES];
		arr[nPipes] = new SinusoidalPipe(0.3333f, 3, 4.f, 0.f, -0.1f, 0.95f, false);
		arr[nPipes++]->setConstraints(-1.2f, -0.45f, 0.8f, 0.24f); 
		arr[nPipes] = new SinusoidalPipe(0.3333f, 3, 4.f, 0.f, 0.05f, 0.95f, true);
		arr[nPipes++]->setConstraints(-1.2f, -0.29f, 1.f, 0.39f);
		for (uint i = 0; i < nPipes; i++)
		{
			pipes->Append(arr[i], sizeof(SinusoidalPipe), i * sizeof(SinusoidalPipe));
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
		for (int i = 0; i < nPipes; i++)
			delete arr[i];
	}
	void Liq::initParticles(){
		nParticles = 0;
		particles = std::make_unique<SSBO>(nullptr, sizeof(NormalParticle) * MAX_PARTICLES);
		particles->SetLayout(NormalParticle::GetLayout());

		debug = std::make_unique<SSBO>(nullptr, sizeof(float) * 2 * MAX_PARTICLES);

		PR = std::make_unique<Shader>("Resources/PipeTest/Particle.shader");
		DP = std::make_unique<Shader>("Resources/PipeTest/DP.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Force = std::make_unique<Shader>("Resources/PipeTest/Forces.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		Integrator = std::make_unique<Shader>("Resources/PipeTest/Integration.shader", DSIZE(PARTICLE_DISPATCH_SIZE));
		initConstants();
		initCells();
	}
	void Liq::initCells() {
		cellsys = std::make_unique<CellSystem>(2/(4*SPH_PARTICLE_RADIUS),2/(4*SPH_PARTICLE_RADIUS)
			,4 * SPH_PARTICLE_RADIUS,*particles,nParticles,"NPSorting");
	}
	void Liq::initConstants() {
		float* farr = new float[4];
		farr[0] = 4 * SPH_PARTICLE_RADIUS;
		farr[1] = 315.f / (64.f * glm::pi<float>() * pow(farr[0], 9));
		farr[2] = -45.f / (glm::pi<float>() * pow(farr[0], 6));
		farr[3] = farr[2] * -1;
		
		constants = std::make_unique<UBO>(farr, sizeof(float) * 4);
	}
	void Liq::renderParticles(){
		PR->Bind();
		PR->SetUniformMat4f("u_MVP", projection);
		PR->SetUniform1f("radius", 2000.f * SPH_PARTICLE_RADIUS);
		PR->SetUniform3f("u_Color", 1.f, 0.f, 0.0f);
		PR->SetUniform1ui("nParticles", nParticles);
		renderer.DrawPoints(*particles, *PR, nParticles);	
	}
	void Liq::computeDP() {
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
	void Liq::computeForces() {
		Force->BindSSBO(*particles, "Data", 0);
		Force->BindUBO(*constants, "Constants", 2);
		cellsys->SetShaderSSBOs(*Force);
		Force->SetUniform1ui("width", cellsys->GetWidth());
		Force->SetUniform1ui("height", cellsys->GetHeight());
		Force->SetUniform1i("nParticles", nParticles);
		Force->SetUniform1f("viscosity", viscosity);
		Force->SetUniform1f("mass", mass);
		Force->SetUniformVec2("gravity", gravity);
		Force->DispatchCompute(getDX(), 1, 1);
	}
	void Liq::integrate() {
		Integrator->BindSSBO(*particles, "Data", 0);
		Integrator->BindSSBO(*debug, "DEBUG", 2);
		Integrator->SetUniform1i("nPipes", nPipes);
		Integrator->BindSSBO(*pipes, "Pipes", 1);
		Integrator->DispatchCompute(getDX(), 1, 1);
	}
}