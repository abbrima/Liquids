#pragma once
#define BITONIC
#include "OpenGL/SSBO.h"
#include "OpenGL/Shader.h"
#include "LiquidTools/CellSystem.h"
#include "LiquidTools/Particle.h"
#include "imgui/imgui.h"

#ifndef BITONIC_COMPARASION_SIZE
#define BITONIC_COMPARASION_SIZE 512
#endif
#ifndef BITONIC_TRANSPOSE_SIZE
#define BITONIC_TRANSPOSE_SIZE 16
#endif
class CellSystem {
private:
	SSBO& particles; uint& nParticles;
	uint width, height; float h;
	
	std::unique_ptr<Shader> UnsortedSorter,OffsetCalculator,Bitonic,Transposer;
	std::unique_ptr<SSBO> IndexList,OffsetList;

	void setTransConstants(const uint& lvl, const uint& lvlmask, const uint& width, const uint& height);

	void SortUnsorted();
	void SortBitonic();
	void GenOffsetList();

public:
	CellSystem(const uint& width,const uint& height,const float& h,SSBO& particles,uint& nParticles,const std::string& loc);
	~CellSystem();
	void SetShaderSSBOs(Shader& shader);
	//inline SSBO GetIndexList() { return *IndexList; }
	//inline SSBO GetOffsetList() { return *OffsetList; }
	inline uint GetHeight() { return height; }
	inline uint GetWidth() { return width; }
	void Sort();
	void GuiRender();
};