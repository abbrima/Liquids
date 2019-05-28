#pragma once
#include "OpenGL/Shader.h"
#include "OpenGL/SSBO.h"
#include "OpenGL/AtomicCounterBuffer.h"
#define MAX_PINC 1000
#include <memory>

class CellSystem {
private:
	std::unique_ptr<SSBO> cells;
	std::unique_ptr<Shader> sorter;
	std::unique_ptr<AtomicCounterBuffer> acb;
	uint height; uint width; uint h;
public:
	CellSystem(uint height, uint width, uint harg);
	~CellSystem();

	void SortParticles(SSBO& particles,const uint& nParticles,const uint& dSize);

	inline uint GetHeight() { return height; }
	inline uint GetWidth() { return width; }
	inline SSBO GetCells() { return *cells; }
	inline AtomicCounterBuffer GetACB() { return *acb; }
};
