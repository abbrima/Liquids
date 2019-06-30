#include "Applications/test.h"
#include <random>
#include <algorithm>
#include "Tools/MyExternals.h"
#include "Tools/Timer.h"
#include <stdio.h>

void mCudaSort(unsigned int* ptr, unsigned int length);
void mCudaPairSort(unsigned int* keys,unsigned int length);
struct UnsortedList;
void hsort(UnsortedList* arr, const uint& n);

namespace app {
	void test::OnRender() {
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
		renderer.Clear();
	}
	test::test() {
		ssbo = std::make_unique<SSBO>(nullptr, sizeof(uint)*mNumParticles);
		keys_ssbo = std::make_unique<SSBO>(nullptr, sizeof(uint)*mNumParticles / 2);
		values_ssbo = std::make_unique<SSBO>(nullptr, sizeof(uint)*mNumParticles / 2);
		cudaGraphicsGLRegisterBuffer(&cuda_keys, keys_ssbo->GetID(), cudaGraphicsMapFlagsNone);
		cudaGraphicsGLRegisterBuffer(&cuda_values, values_ssbo->GetID(), cudaGraphicsMapFlagsNone);
		cudaGraphicsGLRegisterBuffer(&cuda_res, ssbo->GetID(), cudaGraphicsMapFlagsNone);
		genRandomVals();
	}
	void test::genRandomVals() {
		uint* ptr = (uint*)ssbo->GetData();
		for (uint i = 0; i < mNumParticles; i++)
			ptr[i] = rand();
		ssbo->Unmap();

		uint*ptr1 = (uint*)keys_ssbo->GetData();
		uint*ptr2 = (uint*)values_ssbo->GetData();

		for (uint i = 0; i < mNumParticles / 2; i++)
		{
			ptr1[i] = rand();
			ptr2[i] = i;
		}

		keys_ssbo->Unmap();
		values_ssbo->Unmap();

	}
	test::~test() {

	}
	void test::OnUpdate() {

	}
	void test::OnImGuiRender() {
		if (ImGui::Button("Generate Random Values"))
			genRandomVals();
		if (ImGui::Button("CPU sort") && sortable)
		{
			log += sortCPU();
		}
		if (ImGui::Button("GPU sort") && sortable)
		{
			log += sortCUDA();
		}
	}
	char* test::sortCPU() {
		sortable = false;
		Timer timer;
		uint* ptr = (uint*)ssbo->GetData();
		std::sort(ptr, ptr + mNumParticles);
		float duration = timer.GetDuration().count();
		ssbo->Unmap();
		sortable = true;
		char* txt = new char[50];
		sprintf_s(txt, 50, "cpu  sort took %10.2f ms\n",duration * 1000);
		return txt;
	}

	char* test::sortCUDA() {
		sortable = false;
		Timer timer;
		size_t  size;
		size_t size1;

		uint* ptr;
		cudaGraphicsMapResources(1, &cuda_res, nullptr);
		cudaGraphicsResourceGetMappedPointer((void**)&ptr, &size, cuda_res);

		mCudaSort(ptr, mNumParticles);

		cudaGraphicsUnmapResources(1,&cuda_res,nullptr);

		/*
		uint* keysPtr;
		uint* valuesPtr;
		
		cudaGraphicsMapResources(1, &cuda_keys, NULL);
		cudaGraphicsResourceGetMappedPointer((void**)&keysPtr, &size, cuda_keys);
		
		cudaGraphicsMapResources(1, &cuda_values, NULL);
		cudaGraphicsResourceGetMappedPointer((void**)&valuesPtr, &size1, cuda_values);

		mCudaPairSort(keysPtr,valuesPtr, mNumParticles/2);

		cudaGraphicsUnmapResources(1, &cuda_values, NULL);
		cudaGraphicsUnmapResources(1, &cuda_keys, NULL);
		*/

		char* txt = new char[50];
		float duration = timer.GetDuration().count();
		sprintf_s(txt, 50, "CUDA sort took %10.2f ms\n", duration * 1000);
		sortable = true;
		return txt;
	}

	void test::FreeGuiRender() {
		ImGui::Begin("Data");
		uint* ptr = (uint*)ssbo->GetData();

		for (uint i = 0; i <50; i++)
		{
			ImGui::Text("%d", *ptr);
			ptr += 1;
		}

		ssbo->Unmap();
		ImGui::End();

		ImGui::Begin("Log");
		ImGui::Text(log.c_str());
		ImGui::End();
	}
}