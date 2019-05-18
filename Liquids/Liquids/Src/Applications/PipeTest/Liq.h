#pragma once
#include "Applications/Application.h"
#include "imgui/imgui.h"
#include "Tools/MyExternals.h"

#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/SSBO.h"
#include "Applications/PipeTest/Particle.h"
#include "Applications/PipeTest/Pipe.h"
#define DISPATCHSIZE 128
#define MAX_PARTICLES 100000
#define MAX_PIPES 20
#define SPH_PARTICLE_RADIUS 0.005f
namespace app
{
	class Liq :public Application
	{
	private:
		glm::mat4 projection;
		Renderer renderer;

		void initParticles(), renderParticles();
		SSBO* particles;
		uint nParticles;

		Shader *PR,*DP,*Force,*Integrator;
		void computeDP(), computeForces(), integrate();
		

		//pipes
		void initPipes(),renderPipes();
		uint nPipes;
		SSBO *pipes;
		VertexBuffer* pipesVB[MAX_PIPES];
		VertexArray* pipesVA[MAX_PIPES];
		Shader *pipeRenderer;


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
			d = nParticles / DISPATCHSIZE;
			if (nParticles%DISPATCHSIZE > 0)
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