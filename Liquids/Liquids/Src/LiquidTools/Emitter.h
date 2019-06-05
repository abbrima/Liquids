#pragma once
#include "LiquidTools/Particle.h"
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

	template<typename T> T* EmitLiquid(const unsigned int& n, const LiquidType& type);
	template<typename T> void EmitLiquidIntoSSBO(const unsigned int& n, unsigned int& nParticles, SSBO& ssbo, const LiquidType& type);
};
class Emitter3D{
private:
	glm::vec3 pos;
public:
	Emitter3D(const glm::vec3& pos);
	~Emitter3D();

	template<typename T> T* Emit(const unsigned int& n);
	template<typename T> void EmitIntoSSBO(const unsigned int& n, unsigned int& nParticles, SSBO& ssbo);

};