#pragma once
#pragma once
#include "Applications/Application.h"
#include "imgui/imgui.h"
#include <memory>

#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/SSBO.h"
#include "OpenGL/UBO.h"

#include "Tools/MyExternals.h"
#include "Tools/FPSCamera.h"
#include "Tools/Skybox.h"
#include "Tools/Lights.h"

#include "LiquidTools/Particle.h"
#include "LiquidTools/Pipe.h"
#include "LiquidTools/Emitter.h"
#include "LiquidTools/CellSystem3D.h"

namespace app {
	class Liquids3D : public Application
	{
	private:
		glm::mat4 projection,view,pv;

		std::unique_ptr<FPSCamera> Camera; glm::mat4 CameraModel;


		Renderer renderer;
		LightClass lights;

		glm::mat4 cube_Model;
		std::unique_ptr<VertexBuffer> cube_VB;
		std::unique_ptr<VertexArray> cube_VA;
		std::unique_ptr<IndexBuffer> cube_IB;
		std::unique_ptr<Shader> cube_Renderer;

		void initCube(), renderCube();


	public:

		Liquids3D();
		~Liquids3D();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
		void FreeGuiRender() override;


	private:
		inline glm::vec3 getWorldPos() {
			double x = 2.0 * xPos / glfwWindowWidth - 1;
			double y = 2.0 * yPos / glfwWindowHeight - 1;
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
	private:
		uint nParticles;
		std::unique_ptr<SSBO> particles;
		
		void initParticles(); void renderParticles();
		std::unique_ptr<Shader> PR,DP,Forces,Integrator;
		std::unique_ptr<CellSystem3D> cellsys;
		float mass, k, pr, viscosity;
		glm::vec3 gravity;
		void computeDP(), computeForces(), integrate();

		std::unique_ptr<UBO> constants;
		std::unique_ptr<UBO> normals;
	};
}