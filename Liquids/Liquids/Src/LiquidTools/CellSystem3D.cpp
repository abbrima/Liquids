#include "CellSystem3D.h"
#include <iostream>

static int GetMinPowOf2(const uint& nParticles) {
	int k = 1;
	while (k < nParticles)
		k <<= 1;
	if (k < BITONIC_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE)
		return BITONIC_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE;
	return k;
}
static uint GetDX(uint total, const uint& dsize) {
	uint ret = total / dsize;
	if (total%dsize > 0)
		ret++;
	return ret;
}
CellSystem3D::CellSystem3D(const uint& width, const uint& height, const uint& depth, const float& h, SSBO& particles, uint& nParticles, const std::string& loc)
	:width(width), height(height), depth(depth),h(h), particles(particles), nParticles(nParticles) {

	UnsortedSorter = std::make_unique<Shader>("Resources/" + loc + "/UnsortedSorter.shader"
		, DSIZE(PARTICLE_DISPATCH_SIZE));

#ifdef BITONIC

	Bitonic = std::make_unique<Shader>("Resources/" + loc + "/Bitonic.shader",
		DSIZE(BITONIC_BLOCK_SIZE));
	Transposer = std::make_unique<Shader>("Resources/" + loc + "/Transposer.shader",
		XSIZE(TRANSPOSE_BLOCK_SIZE) + "\n" + YSIZE(TRANSPOSE_BLOCK_SIZE));
	SecondaryIndexList = std::make_unique<SSBO>(nullptr, sizeof(uint) * 2 * 2 * MAX_PARTICLES);

#endif

	OffsetCalculator = std::make_unique<Shader>("Resources/" + loc + "/OffsetCalculator.shader"
		, DSIZE(PARTICLE_DISPATCH_SIZE));

	IndexList = std::make_unique<SSBO>(nullptr, sizeof(uint) * 2 * 2 * MAX_PARTICLES);
	OffsetList = std::make_unique<SSBO>(nullptr, sizeof(uint)*width*height*depth);
}
CellSystem3D::~CellSystem3D() {

}
void CellSystem3D::Sort() {
	SortUnsorted();
	SortBitonic();
	GenOffsetList();
}
void CellSystem3D::SortUnsorted() {

	UnsortedSorter->BindSSBO(particles, "Data", 0);
	UnsortedSorter->BindSSBO(*IndexList, "List", 4);
	UnsortedSorter->SetUniform1ui("height", height);
	UnsortedSorter->SetUniform1ui("width", width);
	UnsortedSorter->SetUniform1ui("depth", depth);

	UnsortedSorter->DispatchCompute(GetDX(nParticles, PARTICLE_DISPATCH_SIZE), 1, 1);
	const uint N = GetMinPowOf2(nParticles);
	IndexList->WriteVal1uiOffset(0xFFFFFFFF, sizeof(uint) * 2 * (N - nParticles), sizeof(uint) * 2 * nParticles);
	
}

struct UnsortedList;
void bsort(UnsortedList* arr, const uint& N);
void hsort(UnsortedList* arr, const uint& n);
void bbsort(UnsortedList* arr, const uint& N);


void CellSystem3D::SortBitonic() {
	const int N = GetMinPowOf2(nParticles);
#ifdef BITONIC
	const uint MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
	const uint MATRIX_HEIGHT = N / BITONIC_BLOCK_SIZE;

	Bitonic->BindSSBO(*IndexList, "IndexList", 4);
	for (uint level = 2; level <= BITONIC_BLOCK_SIZE; level <<= 1) {
		Bitonic->SetUniform1ui("u_Level", level);
		Bitonic->SetUniform1ui("u_LevelMask", level);

		Bitonic->DispatchCompute(N / BITONIC_BLOCK_SIZE, 1, 1);
	}

	for (uint level = BITONIC_BLOCK_SIZE << 1; level <= N; level <<= 1) {
		// Transpose the data from buffer 1 into buffer 2

		Transposer->BindSSBO(*IndexList, "InputBUF", 4);
		Transposer->BindSSBO(*SecondaryIndexList, "OutputBUF", 5);
		Transposer->SetUniform1ui("u_Width", MATRIX_WIDTH);
		Transposer->SetUniform1ui("u_Height", MATRIX_HEIGHT);

		Transposer->DispatchCompute(MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);

		// Sort the transposed column data
		Bitonic->BindSSBO(*SecondaryIndexList, "IndexList", 4);
		Bitonic->SetUniform1ui("u_Level", level / BITONIC_BLOCK_SIZE);
		Bitonic->SetUniform1ui("u_LevelMask", (level & ~N) / BITONIC_BLOCK_SIZE);

		Bitonic->DispatchCompute(N / BITONIC_BLOCK_SIZE, 1, 1);

		// Transpose the data from buffer 2 back into buffer 1
		Transposer->BindSSBO(*SecondaryIndexList, "InputBUF", 4);
		Transposer->BindSSBO(*IndexList, "OutputBUF", 5);
		Transposer->SetUniform1ui("u_Width", MATRIX_HEIGHT);
		Transposer->SetUniform1ui("u_Height", MATRIX_WIDTH);

		Transposer->DispatchCompute(MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);

		// Sort the row data
		Bitonic->BindSSBO(*IndexList, "IndexList", 4);
		Bitonic->SetUniform1ui("u_Level", BITONIC_BLOCK_SIZE);
		Bitonic->SetUniform1ui("u_LevelMask", level);

		Bitonic->DispatchCompute(N / BITONIC_BLOCK_SIZE, 1, 1);

	}
#else
	uint* ptr = (uint*)IndexList->GetData();

	hsort((UnsortedList*)ptr, nParticles);

	IndexList->Unmap();
#endif
}
void CellSystem3D::GenOffsetList() {
	OffsetList->WriteVal1ui(0xFFFFFFFF, height*width*depth * sizeof(uint));

	OffsetCalculator->BindSSBO(*IndexList, "IndexList", 4);
	OffsetCalculator->BindSSBO(*OffsetList, "OffsetList", 5);
	OffsetCalculator->DispatchCompute(GetDX(nParticles, PARTICLE_DISPATCH_SIZE), 1, 1);

}
void CellSystem3D::SetShaderSSBOs(Shader& shader) {
	shader.Bind();

	shader.BindSSBO(*IndexList, "IndexList", 4);
	shader.BindSSBO(*OffsetList, "OffsetList", 5);
}
void CellSystem3D::GuiRender() {
	uint* ptr = (uint*)IndexList->GetData();
	const int N = GetMinPowOf2(nParticles);
	for (int i = 0; i < N; i++)
	{
		ImGui::Text("%4d  %10d  %10d", i, *ptr, *(ptr + 1));
		ptr += 2;
	}

	IndexList->Unmap();
}