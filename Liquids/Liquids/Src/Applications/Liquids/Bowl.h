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
		Renderer renderer;
		float k,gravity,restDensity,stiffness,nearStiffness,linVis,quadVis,dt,dtf,bounceCO;
		glm::vec3 getWorldPos();

	//Walls
		//Bounds are y=-300,x=-200,x=200
		VertexBuffer *wallVB;
		IndexBuffer *wallIB;
		Shader *wallShader;
		VertexArray *wallVA;
		SSBO* bounds;
		void initWalls();
		void renderWalls();
		
	//Particles
		SSBO *particles;
		SSBO *cells;
		uint nParticles;
		uint nCells;
		uint sizei; uint sizej;

		Shader *particleRenderer;
		//Shader *cellClearer;
		//Shader *cellSorter;
		Shader *neighborFinder;
		Shader *pdCalculator;
		Shader *displacor;
		Shader *viscosityCalculator;
		Shader *advector;
		Shader *externalForces;
		Shader *collisionResolver;

		void computeChanges();
		void initParticles();
		void renderParticles();

	public:
		Bowl();
		~Bowl();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
	};

	static uint getDispatchX(int n,int size);
}