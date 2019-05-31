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
CellSystem::CellSystem(const uint& width,const uint& height,const float& h,SSBO& particles,uint& nParticles)
	:width(width),height(height),h(h),particles(particles),nParticles(nParticles){
	
	UnsortedSorter = std::make_unique<Shader>("Resources/ParticleComputes/Sorting/UnsortedSorter.shader"
		, DSIZE(PARTICLE_DISPATCH_SIZE));

	Bitonic1 = std::make_unique<Shader>("Resources/ParticleComputes/Sorting/Bitonic1.shader",
		DSIZE(BITONIC_COMPARASION_SIZE));
	
	Bitonic2 = std::make_unique<Shader>("Resources/ParticleComputes/Sorting/Bitonic2.shader",
		DSIZE(BITONIC_COMPARASION_SIZE));
	
	OffsetCalculator = std::make_unique<Shader>("Resources/ParticleComputes/Sorting/OffsetCalculator.shader"
		,DSIZE(PARTICLE_DISPATCH_SIZE));

	OffsetACB = std::make_unique<AtomicCounterBuffer>(nullptr, sizeof(uint)*width*height);
	IndexList = std::make_unique<SSBO>(nullptr,sizeof(uint)*2*2*100000);
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

}
void CellSystem::SortBitonic() {
	const int N = GetMinPowOf2(nParticles);
	Bitonic1->BindSSBO(*IndexList, "List", 4);
	for (uint subsize = 2; subsize < N; subsize *= 2)
		for (uint compare_distance = subsize / 2; compare_distance > 0; compare_distance /= 2)
		{
			Bitonic1->SetUniform1ui("subsize", subsize);
			Bitonic1->SetUniform1ui("compare_distance", compare_distance);

			Bitonic1->DispatchCompute(GetDX(N/2,BITONIC_COMPARASION_SIZE), 1, 1);
		}
	Bitonic2->BindSSBO(*IndexList, "List", 4);
	for (uint compare_distance = N/2; compare_distance>0; compare_distance/=2)
	{
		Bitonic2->SetUniform1ui("compare_distance", compare_distance);
		Bitonic2->DispatchCompute(GetDX(N / 2, BITONIC_COMPARASION_SIZE),1,1);
	}
}
void CellSystem::GenOffsetList() {
	OffsetACB->Set(0xFFFFFFFF);

	OffsetCalculator->BindSSBO(*IndexList, "IndexList", 4);
	OffsetCalculator->BindACB(*OffsetACB, 3);

	OffsetCalculator->DispatchCompute(GetDX(nParticles, PARTICLE_DISPATCH_SIZE), 1, 1);

	uint* ptr = (uint*)OffsetACB->GetData(); uint count = 0;
	std::cout << *ptr << std::endl;
	std::cin.get();
	OffsetACB->Unmap();
	
}