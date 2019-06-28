#pragma once
//#define BITONIC
#include "OpenGL/SSBO.h"
#include "OpenGL/Shader.h"
#include "LiquidTools/Particle.h"
#include "imgui/imgui.h"

#ifndef BITONIC_BLOCK_SIZE
#define BITONIC_BLOCK_SIZE 512
#endif
#ifndef TRANSPOSE_BLOCK_SIZE
#define TRANSPOSE_BLOCK_SIZE 16
#endif
class CellSystem {
private:
	SSBO& particles; uint& nParticles;
	uint width, height; float h;
	
	std::unique_ptr<Shader> UnsortedSorter,OffsetCalculator,Bitonic,Transposer;
	std::unique_ptr<SSBO> IndexList,SecondaryIndexList,OffsetList;

	void SortUnsorted();
	void SortBitonic();
	void GenOffsetList();

public:
	CellSystem(const uint& width,const uint& height,const float& h,SSBO& particles,uint& nParticles,const std::string& loc);
	~CellSystem();
	void SetShaderSSBOs(Shader& shader);
	inline uint GetHeight() { return height; }
	inline uint GetWidth() { return width; }
	void Sort();
	void GuiRender();
};