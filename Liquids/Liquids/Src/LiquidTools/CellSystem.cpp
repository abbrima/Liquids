#include "CellSystem.h"
#include <iostream>

static int GetMinPowOf2(const uint& nParticles) {
	int k = 1;
	while (k < nParticles)
		k <<= 1;
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

	Bitonic1 = std::make_unique<Shader>("Resources/"+loc+"/Bitonic1.shader",
		DSIZE(BITONIC_COMPARASION_SIZE));
	
	Bitonic2 = std::make_unique<Shader>("Resources/"+loc+"/Bitonic2.shader",
		DSIZE(BITONIC_COMPARASION_SIZE));
	
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
	UnsortedSorter->SetUniform1ui("nParticles", nParticles);
	UnsortedSorter->SetUniform1ui("height", height);
	UnsortedSorter->SetUniform1ui("width", width);

	UnsortedSorter->DispatchCompute(GetDX(nParticles,PARTICLE_DISPATCH_SIZE), 1, 1);
	//if (nParticles > 300)
		//system("pause");
}

struct UnsortedList;
void bsort(UnsortedList* arr, const uint& N);
void hsort(UnsortedList* arr, const uint& n);
void bbsort(UnsortedList* arr, const uint& N);


void CellSystem::SortBitonic() {
	const int N = GetMinPowOf2(nParticles);
	/*Bitonic1->BindSSBO(*IndexList, "List", 4);
	for (uint subsize = 2; subsize <= N; subsize <<= 1)
		for (uint compare_distance = subsize >> 1; compare_distance > 0; compare_distance >>= 1)
		{
			Bitonic1->SetUniform1ui("subsize", subsize);
			Bitonic1->SetUniform1ui("compare_distance", compare_distance);

			Bitonic1->DispatchCompute(GetDX(N/2,BITONIC_COMPARASION_SIZE), 1, 1);
		}
	Bitonic1->Unbind();
	IndexList->Unbind();
	Bitonic2->BindSSBO(*IndexList, "List", 4);
	for (uint compare_distance = N>>1; compare_distance>1; compare_distance>>=1)
	{
		    Bitonic2->SetUniform1ui("compare_distance", compare_distance);
		    Bitonic2->DispatchCompute(GetDX(N / 2, BITONIC_COMPARASION_SIZE),1,1);

	}
	Bitonic2->Unbind();
	IndexList->Unbind();*/
	uint* ptr = (uint*)IndexList->GetData();

	hsort((UnsortedList*)ptr, nParticles);

	IndexList->Unmap();
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