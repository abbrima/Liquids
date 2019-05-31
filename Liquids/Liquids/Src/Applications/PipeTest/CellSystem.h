#pragma once
#include "OpenGL/SSBO.h"
#include "OpenGL/Shader.h"
#include "Applications/PipeTest/Particle.h"
#define BITONIC_COMPARASION_SIZE 128
class CellSystem {
private:
	SSBO& particles; uint& nParticles;
	uint width, height; float h;
	
	std::unique_ptr<AtomicCounterBuffer> OffsetACB;
	std::unique_ptr<Shader> UnsortedSorter,Bitonic1,Bitonic2,OffsetCalculator;
	std::unique_ptr<SSBO> IndexList;
	void SortUnsorted();
	void SortBitonic();
	void GenOffsetList();

public:
	CellSystem(const uint& width,const uint& height,const float& h,SSBO& particles,uint& nParticles);
	~CellSystem();
	inline SSBO& GetIndexList() { return *IndexList; }
	inline AtomicCounterBuffer& GetOffsetACB() { return *OffsetACB; }
	void Sort();
};