#include "Bowl.h"
#include "Tools/MyExternals.h"
#include <iostream>

namespace app
{
	Bowl::Bowl()
		:nParticles(0), k(20.0f), stiffness(20), nearStiffness(20), restDensity(1), linVis(0), quadVis(1), gravity(9.8), dtf(10), bounceCO(0.4)
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		//glEnable(GL_POINT_SMOOTH);

		initWalls();
		initParticles();
	}
	Bowl::~Bowl()
	{
		delete(wallIB, wallVA, wallVB, wallShader);
	}
	void Bowl::OnUpdate()
	{
		dt = dtf * deltaTime;
		//Update Projection Matrix
		projection = glm::ortho(-(float)glfwWindowWidth / 2, (float)glfwWindowWidth / 2,
			-(float)glfwWindowHeight / 2, (float)glfwWindowHeight / 2);

		glm::vec3 worldPos = getWorldPos();
		if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT] && worldPos.x > -200 && worldPos.x < 200 && worldPos.y>-300)
		{
			Particle p(worldPos.x, worldPos.y);
			particles->Append(&p, sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
			//mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = false;
		}

		computeChanges();
	}
	void Bowl::OnRender()
	{
		renderWalls();
		renderParticles();
	}
	void Bowl::OnImGuiRender()
	{
		ImGui::Text("Number of Particles: %d", nParticles);
		ImGui::SliderFloat("Stiffness", &stiffness, 0, 100);
		ImGui::SliderFloat("Near Stiffness", &nearStiffness, 0, 100);
		ImGui::SliderFloat("Rest Density", &restDensity, 0, 20);
		ImGui::SliderFloat("linVis", &linVis,0,10);
		ImGui::SliderFloat("linQuad", &quadVis, 0, 10);
		ImGui::SliderFloat("Bounce Coefficient", &bounceCO, 0, 1);
	}
	void Bowl::initWalls()
	{
		std::vector<float> data = {
			-200.0f,300.0f,   //left  upper right
			-210.0f,300.0f,   //left  upper left
			-200.0f,-300.0f,  //left  lower right
			-210.0f,-310.0f,  //left  lower left
			200.0f,300.0f,    //right upper left
			210.0f,300.0f,    //right upper right
			200.0f,-300.0f,   //right lower left
			210.0f,-310.0f,   //right lower right

			-210.0f,-300.0f,  //left  lower left upper
			210.0f,-300.0f    //right lower right upper
		};
		std::vector<unsigned int> indicies = {
			0,1,2,1,2,8,3,8,9,3,7,9,9,4,5,4,9,6
		};
		wallVB = new VertexBuffer(data.data(), sizeof(float)*(uint)data.size());
		wallIB = new IndexBuffer(indicies.data(), (uint)indicies.size());
		VertexBufferLayout layout; layout.Push<float>(2);
		wallVA = new VertexArray();
		wallVA->AddBuffer(*wallVB, layout);
		
		wallShader = new Shader("Resources/Shaders/Color.shader");

		bounds = new SSBO(nullptr, sizeof(Bound) * 3);
		bounds->Append(new Bound(-200, -200, -300, 300), sizeof(Bound), 0);
		bounds->Append(new Bound(200, 200, -300, 300), sizeof(Bound), sizeof(Bound));
		bounds->Append(new Bound(-200, 200, -300, -300), sizeof(Bound), 2 * sizeof(Bound));
	}
	void Bowl::renderWalls()
	{
		wallShader->Bind();
		wallShader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
		wallShader->SetUniformMat4f("u_MVP", projection);
		renderer.DrawTriangles(*wallVA, *wallIB, *wallShader);
		wallVA->Unbind();
		wallIB->Unbind();
	}
	void Bowl::initParticles()
	{

	
		particles = new SSBO(NULL, sizeof(Particle) * 100000);
		VertexBufferLayout layout;
		layout.Push<float>(2);    //Position
		layout.Push<float>(2);    //Previous Position
		layout.Push<float>(2);    //Velocity
		layout.Push<float>(4);    // pressure and density values
		
		short neibyfour = MAX_NEIGHBORS / 4;
		short remainder = MAX_NEIGHBORS % 4;

		for (short i = 0; i < neibyfour; i++)
			layout.Push<int>(4);
		if (remainder > 0)
			layout.Push<int>(remainder);
		particles->SetLayout(layout);

		Particle p(0, 0);
		for (int i = 0; i < 1000; i++)
		{
			particles->Append(&p, sizeof(Particle), nParticles * sizeof(Particle));
			nParticles++;
		}

		//init cells
		std::vector<Cell> cellsvector;
		sizei = 48; sizej = 64;
		for (int j = -8; j < 8; j++) //length size is 48
			for (int i = -4; i < 4; i++) // width size is 64
				cellsvector.push_back(Cell(j, i));
		cells = new SSBO(cellsvector.data(), sizeof(Cell)*cellsvector.size());
		nCells = 8 * 16;


		particleRenderer = new Shader("Resources/Shaders/Particle.shader");
		//cellClearer = new Shader("Resources/ParticleComputes/CellClearer.shader");
		//cellSorter = new Shader("Resources/ParticleComputes/CellSorter.shader");
		neighborFinder = new Shader("Resources/ParticleComputes/Neighbors.shader");
		pdCalculator = new Shader("Resources/ParticleComputes/DensityPressure.shader");
		displacor = new Shader("Resources/ParticleComputes/Displacement.shader");
		viscosityCalculator = new Shader("Resources/ParticleComputes/Viscosity.shader");
		advector = new Shader("Resources/ParticleComputes/Advection.shader");
		externalForces = new Shader("Resources/ParticleComputes/ExternalForces.shader");
		collisionResolver = new Shader("Resources/ParticleComputes/Collision.shader");
	}
	void Bowl::renderParticles()
	{
		particleRenderer->Bind();
		particleRenderer->SetUniformMat4f("u_MVP", projection);
		particleRenderer->SetUniform1f("radius", k/2);

		renderer.DrawPoints(*particles, *particleRenderer, nParticles);
	}

	glm::vec3 Bowl::getWorldPos()
	{
		double x = 2.0 * xPos / glfwWindowWidth - 1;
		double y = 2.0 * yPos / glfwWindowHeight - 1;
		glm::vec4 screenPos = glm::vec4(x, -y, -1.0, 1.0f);

		//    glm::mat4 ProjectView = GlobalProjection * GlobalView;
		glm::mat4 inv = glm::inverse(projection);
		glm::vec4 worldpos = inv * screenPos;
		return glm::vec3(worldpos);
	}

	void Bowl::computeChanges()
	{
		/*
		//Clear the cells
		cellClearer->BindSSBO(*cells, "CellData", 3);
		cellClearer->DispatchCompute(getDispatchX(nCells,16), 1, 1);

		//sort the cells
		cellSorter->BindSSBO(*cells, "CellData", 3);
		cellSorter->BindSSBO(*particles, "Data", 2);
		cellSorter->SetUniform1i("sizei", sizei);
		cellSorter->SetUniform1i("sizej", sizej);
		cellSorter->SetUniform1i("nCells", nCells);
		cellSorter->SetUniform1i("width", 16);
		cellSorter->SetUniform1i("height", 8);
		cellSorter->DispatchCompute(getDispatchX(nParticles, DISPATCHSIZE), 1, 1);
		*/
		
		neighborFinder->BindSSBO(*particles, "Data", 2);
		neighborFinder->SetUniform1f("k", k);
		neighborFinder->SetUniform1i("nParticles", nParticles);
		neighborFinder->DispatchCompute(getDispatchX(nParticles , DISPATCHSIZE), 1, 1);
		
		pdCalculator->BindSSBO(*particles, "Data", 2);
		pdCalculator->SetUniform1f("k", k);
		pdCalculator->SetUniform1f("stiffness", stiffness);
		pdCalculator->SetUniform1f("nearStiffness", nearStiffness);
		pdCalculator->SetUniform1f("restDensity", restDensity);
		pdCalculator->DispatchCompute(getDispatchX(nParticles, DISPATCHSIZE), 1, 1);
		
		displacor->BindSSBO(*particles, "Data", 2);
		displacor->SetUniform1f("k", k);
		displacor->SetUniform1f("deltaTime", dt);
		displacor->DispatchCompute(getDispatchX(nParticles, DISPATCHSIZE), 1, 1);

		viscosityCalculator->BindSSBO(*particles, "Data", 2);
		viscosityCalculator->SetUniform1f("k", k);
		viscosityCalculator->SetUniform1f("deltaTime", dt);
		viscosityCalculator->SetUniform1f("linVis", linVis);
		viscosityCalculator->SetUniform1f("quadVis", quadVis);
		viscosityCalculator->DispatchCompute(getDispatchX(nParticles, DISPATCHSIZE), 1, 1);

		advector->BindSSBO(*particles, "Data", 2);
		advector->SetUniform1f("deltaTime", dt);
		advector->DispatchCompute(getDispatchX(nParticles, DISPATCHSIZE), 1, 1);

		externalForces->BindSSBO(*particles, "Data", 2);
		externalForces->SetUniform1f("deltaTime", dt);
		externalForces->SetUniform1f("gravity", gravity);
		externalForces->DispatchCompute(getDispatchX(nParticles, DISPATCHSIZE), 1, 1);

		collisionResolver->BindSSBO(*particles, "Data", 2);
		//collisionResolver->BindSSBO(*bounds, "Bounds", 3);
		//collisionResolver->SetUniform1i("nBounds", 3);
		collisionResolver->SetUniform1f("k", k);
		collisionResolver->SetUniform1f("bounceCO", bounceCO);
		collisionResolver->DispatchCompute(getDispatchX(nParticles, DISPATCHSIZE), 1, 1);

	}
	uint getDispatchX(int n,int size)
	{
		uint l = n / size;
		if (n%size > 0)
			l++;
		return l;
	}
}
