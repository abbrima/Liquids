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

#define GRAVITY 9.8f

namespace app
{
	class Bowl :public Application
	{
	private:
		glm::mat4 projection;
		Renderer renderer;
		uint nParticles;
		float k;

	//Walls
		//Bounds are y=-300,x=-200,x=200
	private:
		VertexBuffer *wallVB;
		IndexBuffer *wallIB;
		Shader *wallShader;
		VertexArray *wallVA;
		
	//Particles
		

	public:
		Bowl();
		~Bowl();

		void initWalls();
		void renderWalls();


		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}