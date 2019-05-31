#pragma once
#include "Applications/Application.h"
#include "imgui/imgui.h"
#include "Tools/MyExternals.h"
#include <memory>
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/SSBO.h"
#include "OpenGL/UBO.h"

#include "Applications/PipeTest/Particle.h"
#include "Applications/PipeTest/Pipe.h"
#include "Applications/PipeTest/Emitter.h"
#include "Applications/PipeTest/CellSystem.h"
namespace app
{
	class Liq :public Application
	{
	private:
		glm::mat4 projection;
		Renderer renderer;
		int startingParticles;

		void initParticles(), renderParticles();
		std::unique_ptr<SSBO> particles;
		uint nParticles;

		std::unique_ptr<SSBO> test;
		std::unique_ptr<CellSystem> cellsys;

		float k, pr, mass, viscosity; glm::vec2 gravity;

		std::unique_ptr<Shader> PR,DP,Force,Integrator;
		std::unique_ptr<UBO> constants;
		void computeDP(), computeForces(), integrate(), initConstants();
		
		//Cells
		void initCells();


		//pipes
		void initPipes(),renderPipes();
		uint nPipes;
		std::unique_ptr<SSBO> pipes;
		std::unique_ptr<VertexBuffer> pipesVB[MAX_PIPES];
		std::unique_ptr<VertexArray> pipesVA[MAX_PIPES];
		std::unique_ptr<Shader> pipeRenderer;


		inline glm::vec3 getWorldPos(){
			double x = 2.0 * xPos / glfwWindowWidth -1;
			double y = 2.0 * yPos / glfwWindowHeight -1;
			glm::vec4 screenPos = glm::vec4(x, -y, 1.f, 1.f);
			glm::mat4 inv = glm::inverse(projection);
			glm::vec4 worldPos = inv * screenPos;
			return glm::vec3(worldPos);
		}
		inline uint getDX() {
			uint d = 0;
			d = nParticles / PARTICLE_DISPATCH_SIZE;
			if (nParticles%PARTICLE_DISPATCH_SIZE > 0)
				d++;
			return d;
		}
	public:
		Liq();
		~Liq();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
		void FreeGuiRender() override;
	};
}