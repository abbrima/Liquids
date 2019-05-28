#include "CellSystem.h"
#include <memory>
#include <iostream>

CellSystem::CellSystem(uint height, uint width,uint harg) :width(width),h(h),height(height){
	sorter = std::make_unique<Shader>("Resources/ParticleComputes/CellSorter.shader");
	cells = std::make_unique<SSBO>(nullptr, sizeof(int)*MAX_PINC*height*width);
	acb = std::make_unique<AtomicCounterBuffer>(nullptr,height*width*sizeof(uint));
}
CellSystem::~CellSystem() {

}
void CellSystem::SortParticles(SSBO& particles,const uint& nParticles,const uint& dSize) {
	acb->Reset();
	
	sorter->BindSSBO(particles, "Data", 0);
	sorter->BindSSBO(*cells, "Cells", 2);
	sorter->BindACB(*acb, 3);
	sorter->SetUniform1ui("width", width);
	sorter->SetUniform1ui("height", height);
	sorter->SetUniform1ui("nParticles", nParticles);

	sorter->DispatchCompute(dSize, 1, 1);
}
