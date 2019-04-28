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
#define DISPATCHSIZE 64


namespace app
{   
	class Bowl :public Application
	{
	private:
		glm::mat4 projection;
		glm::mat4 model;
		float kd,linVis,quadVis,gravity,timeFactor,restDensity,stiffness,nearStiffness;
		Renderer renderer;
		glm::vec3 getWorldPos();

		inline uint getDX() {
			uint d = nParticles / DISPATCHSIZE;
			if (nParticles%DISPATCHSIZE > 0)
				d += 1;
			return d;
		}

	//Walls
		//Bounds are y=-300,x=-200,x=200
		VertexBuffer *wallVB;
		IndexBuffer *wallIB;
		Shader *wallShader;
		VertexArray *wallVA;
		void initWalls();
		void renderWalls();
		
	//Particles
		SSBO *particles;
		uint nParticles;

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

		//test
		SSBO *out;

	public:
		Bowl();
		~Bowl();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
		void FreeGuiRender() override;
	};

}