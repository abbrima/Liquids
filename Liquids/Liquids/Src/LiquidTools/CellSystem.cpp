#include "CellSystem.h"
#include "Tools/Timer.h"
#include <iostream>


static int GetMinPowOf2(const uint& nParticles) {
	int k = 1;
	while (k < nParticles)
		k <<= 1;
	if (k < BITONIC_COMPARASION_SIZE * BITONIC_TRANSPOSE_SIZE)
		return BITONIC_COMPARASION_SIZE * BITONIC_TRANSPOSE_SIZE;

	return k;
}
static uint GetDX(uint total, const uint& dsize){
	uint ret = total / dsize;
	if (total%dsize> 0)
		ret++;
	return ret;
}
CellSystem::CellSystem(const uint& width,const uint& height,const float& h,SSBO& particles,uint& nParticles,const std::string& loc)
	:width(width),height(height),h(h),particles(particles),nParticles(nParticles){
	
	UnsortedSorter = std::make_unique<Shader>("Resources/"+loc+"/UnsortedSorter.shader"
		, DSIZE(PARTICLE_DISPATCH_SIZE));


#ifdef BITONIC

	Bitonic = std::make_unique<Shader>("Resources/" + loc + "/Bitonic.shader",
		DSIZE(BITONIC_COMPARASION_SIZE));
	Transposer = std::make_unique<Shader>("Resources/" + loc + "/Transposer.shader",
		XSIZE(BITONIC_TRANSPOSE_SIZE) + "\n" + YSIZE(BITONIC_TRANSPOSE_SIZE));

#endif
	OffsetCalculator = std::make_unique<Shader>("Resources/" + loc + "/OffsetCalculator.shader"
		,DSIZE(PARTICLE_DISPATCH_SIZE));

	IndexList = std::make_unique<SSBO>(nullptr,sizeof(uint)*2*2*MAX_PARTICLES);
	OffsetList = std::make_unique<SSBO>(nullptr, sizeof(uint)*width*height);
}
CellSystem::~CellSystem() {

}
void CellSystem::Sort() {
	SortUnsorted();
	SortBitonic();
 	GenOffsetList();
}
void CellSystem::SortUnsorted() {
	//if nParticles > 1
	UnsortedSorter->BindSSBO(particles, "Data", 0);
	UnsortedSorter->BindSSBO(*IndexList, "List", 4);
	UnsortedSorter->SetUniform1ui("height", height);
	UnsortedSorter->SetUniform1ui("width", width);

	UnsortedSorter->DispatchCompute(GetDX(nParticles,PARTICLE_DISPATCH_SIZE), 1, 1);
	const uint N = GetMinPowOf2(nParticles);
	IndexList->WriteVal1uiOffset(0xFFFFFFFF, sizeof(uint) * 2 * (N - nParticles), sizeof(uint) * 2 * nParticles);
	//if (nParticles > 300)
		//system("pause");
}

struct UnsortedList;
void bsort(UnsortedList* arr, const uint& N);
void hsort(UnsortedList* arr, const uint& n);
void bbsort(UnsortedList* arr, const uint& N);

void CellSystem::setTransConstants(const uint& lvl, const uint& lvlmask, const uint& width, const uint& height) {
	Transposer->Bind();
	Transposer->SetUniform1ui("u_Width", width);
	Transposer->SetUniform1ui("u_Height", height);
}
void CellSystem::SortBitonic() {
	const int N = GetMinPowOf2(nParticles);
	//Timer timer;
#ifdef BITONIC



	// The number of elements to sort is limited to an even power of 2
// At minimum 8,192 elements - BITONIC_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE
// At maximum 262,144 elements - BITONIC_BLOCK_SIZE * BITONIC_BLOCK_SIZE
	const uint MATRIX_WIDTH = BITONIC_COMPARASION_SIZE;
	const uint MATRIX_HEIGHT = N / BITONIC_COMPARASION_SIZE;

	Bitonic->BindSSBO(*IndexList, "IndexList", 4);
	for (uint level = 2; level <= BITONIC_COMPARASION_SIZE; level <<= 1)
	{

		Bitonic->SetUniform1ui("u_Level", level);

		Bitonic->DispatchCompute(GetDX(N, BITONIC_COMPARASION_SIZE));
	}
	for (uint level = BITONIC_COMPARASION_SIZE << 1; level <= N; level <<= 1) {
		Transposer->BindSSBO(*IndexList, "IndexList", 4);
		setTransConstants(level / BITONIC_COMPARASION_SIZE, (level & ~N) /
			BITONIC_COMPARASION_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT);
		Transposer->DispatchCompute(MATRIX_WIDTH / BITONIC_TRANSPOSE_SIZE, MATRIX_HEIGHT / BITONIC_TRANSPOSE_SIZE);

		Bitonic->BindSSBO(*IndexList, "IndexList", 4);
		Bitonic->SetUniform1ui("u_Level", level / BITONIC_COMPARASION_SIZE);
		Bitonic->DispatchCompute(N / BITONIC_COMPARASION_SIZE);

		Transposer->BindSSBO(*IndexList, "IndexList", 4);
		setTransConstants(BITONIC_COMPARASION_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH);
		Transposer->DispatchCompute(MATRIX_HEIGHT / BITONIC_TRANSPOSE_SIZE, MATRIX_WIDTH / BITONIC_TRANSPOSE_SIZE);

		Bitonic->BindSSBO(*IndexList, "IndexList", 4);
		Bitonic->SetUniform1ui("u_Level", BITONIC_COMPARASION_SIZE);
		Bitonic->DispatchCompute(N / BITONIC_COMPARASION_SIZE);
	}
	

#else
	//Timer timer;
	uint* ptr = (uint*)IndexList->GetData();

	hsort((UnsortedList*)ptr, nParticles);

	IndexList->Unmap();
#endif
}
void CellSystem::GenOffsetList() {
	OffsetList->WriteVal1ui(0xFFFFFFFF, height*width * sizeof(uint));

	OffsetCalculator->BindSSBO(*IndexList, "IndexList", 4);
	OffsetCalculator->BindSSBO(*OffsetList, "OffsetList", 5);
    OffsetCalculator->DispatchCompute(GetDX(nParticles, PARTICLE_DISPATCH_SIZE), 1, 1);
	
}
void CellSystem::SetShaderSSBOs(Shader& shader) {
	shader.Bind();

	shader.BindSSBO(*IndexList, "IndexList", 4);
	shader.BindSSBO(*OffsetList, "OffsetList", 5);
}
void CellSystem::GuiRender() {
	uint* ptr = (uint*)IndexList->GetData();
	const int N = GetMinPowOf2(nParticles);
	for (int i = 0; i < N; i++)
	{
		ImGui::Text("%4d  %10d  %10d",i, *ptr, *(ptr + 1));
		ptr += 2;
	}

	IndexList->Unmap();
}