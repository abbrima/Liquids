#pragma once
#include "Applications/Application.h"
#include <gl/glew.h> 
#include "OpenGL/Renderer.h"
#include "OpenGL/SSBO.h"
#include "imgui/imgui.h"

#include <memory>
#define mNumParticles 10000000

#include <cuda_gl_interop.h>

#include <string>

namespace app {
	class test :public Application {
	private:
		Renderer renderer;
		std::string log = std::to_string(mNumParticles) + " Elements\n";
		bool sortable = true;
		std::unique_ptr<SSBO> ssbo;

		std::unique_ptr<SSBO> keys_ssbo, values_ssbo;
		
		void genRandomVals();
		char *sortCPU(), *sortCUDA();
		cudaGraphicsResource *cuda_res,*cuda_keys,*cuda_values;
	public:

		test();
		~test();

		void FreeGuiRender() override;
		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;

	};
}
