#include "Liquids3D.h"
#include <iostream>

namespace app {

	static void printVec3(const glm::vec3& v) {
		printf("%10.5f   %10.5f   %10.5f\n", v.x, v.y, v.z);
	}

	static glm::vec4 calcNormal(const glm::vec3& p1,const glm::vec3& p2,const glm::vec3& p3) {
		glm::vec3 v1 = p2 - p1;
		glm::vec3 v2 = p3 - p2;
		return glm::vec4(glm::normalize(glm::cross(v1, v2)),1.f);
	}


	Liquids3D::Liquids3D() :
		view(glm::mat4(1.f)),
		k(2000.f),
		viscosity(3000.f),
		pr(1000.f),
		mass(0.02f),
		gravity(glm::vec3(0.f, -9806.65f,0.f))
	{
		GLCall(glEnable(GL_DEPTH_TEST));



		Camera = std::make_unique <FPSCamera>();
		Camera->SetCameraSpeed(1.f);
		Camera->SetSensitivity(40.4f);
		Camera->SetPitch(-16.1f); Camera->SetYaw(-22.f);
		Camera->SetCameraPosition(-2.671100f, 0.9919f, 1.1386f);
		CameraModel = glm::translate(glm::mat4(1.0f), Camera->GetCameraPosition());
		Camera->SetCameraDirection(0, 0, 0);

		float* farr = new float[4];
		farr[0] = 4 * SPH_PARTICLE_RADIUS;
		farr[1] = 315.f / (64.f * glm::pi<float>() * pow(farr[0], 9));
		farr[2] = -45.f / (glm::pi<float>() * pow(farr[0], 6));
		farr[3] = farr[2] * -1;

		constants = std::make_unique<UBO>(farr, sizeof(float) * 4);

		{
			using namespace glm;
			const vec3 _zero = vec3(0.f, 0.f, 900.f * SPH_PARTICLE_RADIUS);
			const vec3 _one = vec3(900.f * SPH_PARTICLE_RADIUS, 0.f, 0.f);
			const vec3 _two = vec3(0.f, 0.f, -900.f * SPH_PARTICLE_RADIUS);
			const vec3 _three = vec3(-900.f * SPH_PARTICLE_RADIUS, 0.f, 0.f);
			const vec3 _four = vec3(0.f, 900.f * SPH_PARTICLE_RADIUS, 0.f);
			const vec3 _five = vec3(0.f, -900.f * SPH_PARTICLE_RADIUS, 0.f);

			vec4* normalss = new vec4[8];
			normalss[0] = calcNormal(_zero, _one, _four);
			normalss[1] = calcNormal(_one, _two, _four);
			normalss[2] = calcNormal(_two, _three, _four);
			normalss[3] = calcNormal(_three, _zero, _four);
			normalss[4] = calcNormal(_five, _one, _zero);
			normalss[5] = calcNormal(_five, _two, _one);
			normalss[6] = calcNormal(_five, _three, _two);
			normalss[7] = calcNormal(_five, _zero, _three);


			normals = std::make_unique<UBO>(normalss, sizeof(vec4) * 8);
		}

		Light light;
		light.setAmbient(0.2f, 0.2f, 0.2f);
		light.setDiffuse(0.6f, 0.6f, 0.6f);
		light.setSpecular(1.0f, 1.0f, 1.0f);
		light.setSource(940.0f, 336.0f, -1450.0f, 1.0f);

		lights.AddLight(light, "CUBE");

		initCube();
		initParticles();
	}
	Liquids3D::~Liquids3D() {
		glDisable(GL_DEPTH_TEST);
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
		projection = glm::perspective(glm::radians(fov), (float)glfwWindowWidth / (float)glfwWindowHeight, 0.01f, 10.0f);
		CameraModel = glm::translate(glm::mat4(1.0f), Camera->GetCameraPosition());

		pv = projection * view;

		cellsys->Sort();

		computeDP();
		computeForces();
		integrate();
	}
	void Liquids3D::OnRender() {
		GLCall(glClearColor(0.07f, 0.07f, 0.07f, 1.0f));

		renderCube();
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
		PR->SetUniformMat4f("u_MVP", pv);
		PR->SetUniform1f("radius",  2*SPH_PARTICLE_RADIUS);
		PR->SetUniformVec3("u_ViewPos", Camera->GetCameraPosition());
		PR->SetUniformLightClass("lights", lights);
		PR->SetUniformMaterial("material", BlueGlass);
		PR->BindUBO(*normals, "ubo_Normals", 3);

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
		DP->DispatchCompute(getDX(), 1, 1);
	}
	void Liquids3D::computeForces() 
	{
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
	void Liquids3D::initCube() {

		cube_Model = glm::translate(glm::mat4(1.f),glm::vec3(0.4,0.4,-0.4));

		std::vector<float> points;
		for (float x = -0.075f ; x<=0.075f ; x+=0.15f)
			for (float y = -0.075f; y <= 0.075f; y += 0.15f)
				for (float z = -0.075f; z <= 0.075f; z += 0.15f)
				{
					points.push_back(x);
					points.push_back(y);
					points.push_back(z);
				}
		cube_VB = std::make_unique<VertexBuffer>(points.data(), points.size() * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		cube_VA = std::make_unique<VertexArray>();
		cube_VA->AddBuffer(*cube_VB, layout);

		std::vector<uint> indicies = {
			1,3,5,5,3,7,5,7,4,4,7,6,4,6,0,0,6,2,0,2,1,1,2,3,3,2,7,7,2,6,1,0,5,5,0,4
		};
		cube_IB = std::make_unique<IndexBuffer>(indicies.data(), indicies.size());
		cube_Renderer = std::make_unique<Shader>("Resources/Shaders/Color.shader");
	}
	void Liquids3D::renderCube() {
		cube_Renderer->Bind();

		cube_Renderer->SetUniformMat4f("u_MVP",pv * cube_Model);
		cube_Renderer->SetUniform4f("u_Color", 1.f, 1.f, 1.f, 1.f);

		renderer.DrawTriangles(*cube_VA, *cube_IB, *cube_Renderer);
	}
}