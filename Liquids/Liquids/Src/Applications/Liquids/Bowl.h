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
#define DISPATCHSIZE 1024


namespace app
{   
	class Bowl :public Application
	{
	private:
		glm::mat4 projection;
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
		uint nParticles;
		Shader *ParticleRenderer;
		SSBO *particles;

		void computeChanges();
		void initParticles();
		void renderParticles();

		void ApplyGravity(); Shader *Gravity;
		void Advect();       Shader *Advection;

	public:
		Bowl();
		~Bowl();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
		void FreeGuiRender() override;
	};

}