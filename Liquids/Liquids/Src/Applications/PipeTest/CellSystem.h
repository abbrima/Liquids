#pragma once
#include "OpenGL/SSBO.h"
#include "OpenGL/Shader.h"
#include "Applications/PipeTest/Particle.h"
#include "imgui/imgui.h"

#define BITONIC_COMPARASION_SIZE 128
class CellSystem {
private:
	SSBO& particles; uint& nParticles;
	uint width, height; float h;
	
	std::unique_ptr<Shader> UnsortedSorter,Bitonic1,Bitonic2,OffsetCalculator;
	std::unique_ptr<SSBO> IndexList,OffsetList;
	void SortUnsorted();
	void SortBitonic();
	void GenOffsetList();

public:
	CellSystem(const uint& width,const uint& height,const float& h,SSBO& particles,uint& nParticles);
	~CellSystem();
	void SetShaderSSBOs(Shader& shader);
	//inline SSBO GetIndexList() { return *IndexList; }
	//inline SSBO GetOffsetList() { return *OffsetList; }
	inline uint GetHeight() { return height; }
	inline uint GetWidth() { return width; }
	void Sort();
	void GuiRender();
};