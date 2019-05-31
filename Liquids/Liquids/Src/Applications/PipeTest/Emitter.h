#pragma once
#include <Applications/PipeTest/Particle.h>
#include "OpenGL/SSBO.h"
#include "glm/common.hpp"
class Emitter {
private:
	glm::vec2 pos;
public:
	Emitter(glm::vec2& pos);
	~Emitter();


	template<typename T> T* Emit(const unsigned int& n);

	template<typename T> void EmitIntoSSBO(const unsigned int& n,unsigned int& nParticles, SSBO& ssbo);
};