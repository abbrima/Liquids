#include "Applications/Application.h"
#include "imgui/imgui.h"

#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/SSBO.h"
#include "Applications/Liquids/Particle.h"
#define DISPATCHSIZE 128
#define MAX_PARTICLES 100000

namespace app
{   
	class Bowl :public Application
	{
	private:
		glm::mat4 projection;
		Renderer renderer;
		glm::vec3 getWorldPos();

		inline uint getDX() {
			uint d = nParticles / DISPATCHSIZE;
			if (nParticles%DISPATCHSIZE > 0)
				d += 1;
			return d;
		}

	//Particles
		SSBO *particles;
		uint nParticles;

		float kd, stiffness, nearStiffness, restDensity, linVis, quadVis,gravity,timeFactor;

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
		void initParticles();
		void renderParticles();
		void emitLeft();
		void emitCenter();
		void emitRight();
		
	public:
		Bowl();
		~Bowl();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
		void FreeGuiRender() override;
	};

}