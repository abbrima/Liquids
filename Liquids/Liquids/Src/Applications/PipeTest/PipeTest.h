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
#include "Applications/Liquids/Particle.h"
#include "Applications/PipeTest/Pipe.h"
#define DISPATCHSIZE 128
#define MAX_PARTICLES 100000
#define MAX_PIPES 20

namespace app
{
	class PipeTest : public Application
	{
	private:
		glm::mat4 projection;
		Renderer renderer;
		void initParticles();


		inline uint getDX() {
			uint d = nParticles / DISPATCHSIZE;
			if (nParticles%DISPATCHSIZE > 0)
				d += 1;
			return d;
		}
		inline glm::vec3 getWorldPos() {
			double x = 2.0 * xPos / glfwWindowWidth - 1;
			double y = 2.0 * yPos / glfwWindowHeight - 1;
			glm::vec4 screenPos = glm::vec4(x, -y, -1.0, 1.0f);

			//    glm::mat4 ProjectView = GlobalProjection * GlobalView;
			glm::mat4 inv = glm::inverse(projection);
			glm::vec4 worldpos = inv * screenPos;
			return glm::vec3(worldpos);
		}

		SSBO *particles;
		uint nParticles;

		float kd, stiffness, nearStiffness, restDensity, 
			linVis,lineWidth, quadVis, gravity, timeFactor;

		Shader *particleRenderer;


		Shader *neighborFinder;
		void findNeighbors();
		Shader *pdCalculator;
		void calculateDP();
		Shader *displacor;
		void displace();
		Shader *viscosityCalculator;
		void calculateViscosity();
		Shader *advector;
		void advect();
		Shader *externalForces;
		void applyExternalForces();
		Shader *collisionResolver;
		void resolveCollisions();



		void computeChanges();
		void renderParticles();

		void initPipes();
		void renderPipes();

		uint nPipes;
		SSBO *pipes;
		VertexBuffer* pipesVB[MAX_PIPES];
		VertexArray* pipesVA[MAX_PIPES];
		Shader *pipeRenderer;


	public:
		PipeTest();
		~PipeTest();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
		void FreeGuiRender() override;
	};
}