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

		Skybox skybox; glm::mat4 skyboxModel;

		Renderer renderer;




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
		std::unique_ptr<SSBO> particles; void initParticles(); void renderParticles();
		std::unique_ptr<Shader> PR;
		std::unique_ptr<CellSystem3D> cellsys;
	};
}