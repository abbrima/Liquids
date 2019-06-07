#include "Emitter.h"
#define MAX_P_EMIT 500
Emitter::Emitter(glm::vec2& pos):pos(pos) {}
Emitter::~Emitter() {}
static glm::vec2 GenRandomPos(const glm::vec2& pos, const float& offset) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	return glm::vec2(pos.x + xo, pos.y + yo);
}
static glm::vec3 GenRandomPos3D(const glm::vec3& pos, const float& offset) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float zo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	return glm::vec3(pos.x + xo, pos.y + yo, pos.z + zo);
}
template<typename T> T* Emitter::Emit(const unsigned int& n) {
	static_assert(false);
}
template<> NormalParticle* Emitter::Emit<NormalParticle>(const unsigned int& n) {
	NormalParticle* particles;
	particles = new NormalParticle[n];
	for (int i = 0; i < n; i++)
	   particles[i].position = GenRandomPos(pos, 0.04f);
	return particles;
}
template<typename T> void Emitter::EmitIntoSSBO(const unsigned int& n, unsigned int& nParticles, SSBO& ssbo) {
	static_assert(false);
}
template<> void Emitter::EmitIntoSSBO<NormalParticle>(const unsigned int& n,unsigned int& nParticles, SSBO& ssbo) {
	NormalParticle* particles = Emit<NormalParticle>(n);
	ssbo.Append(particles, n * sizeof(NormalParticle), sizeof(NormalParticle)*nParticles);
	nParticles += n;
}
template<typename T> T* Emitter::EmitLiquid(const unsigned int& n, const LiquidType& type) {
	static_assert(false);
}
template<> AdvancedParticle* Emitter::EmitLiquid(const unsigned int& n, const LiquidType& type) {
	AdvancedParticle* particles;
	particles = new AdvancedParticle[n];
	for (int i = 0; i < n; i++)
	{
		particles[i].position = GenRandomPos(pos, 0.04f); AdvancedParticle::SetType(particles[i],type);
	}
	return particles;
}
template<typename T> void Emitter::EmitLiquidIntoSSBO(const unsigned int& n, unsigned int& nParticles, SSBO& ssbo, const LiquidType& type) {
	static_assert(false);
}
template<> void Emitter::EmitLiquidIntoSSBO<AdvancedParticle>(const unsigned int& n, unsigned int& nParticles, SSBO& ssbo, const LiquidType& type) {
	AdvancedParticle* particles = EmitLiquid<AdvancedParticle>(n, type);
	ssbo.Append(particles, n * sizeof(AdvancedParticle), sizeof(AdvancedParticle)*nParticles);
	nParticles += n;
}

Emitter3D::Emitter3D(const glm::vec3& pos) :pos(pos) {}
Emitter3D::~Emitter3D() {}
template <typename T> T* Emitter3D::Emit(const unsigned int& n) {
	static_assert(false);
}
template <> Particle3D* Emitter3D::Emit<Particle3D>(const unsigned int& n) {
	Particle3D* particles;
	particles = new Particle3D[n];
	for (int i = 0; i < n; i++)
	{
		particles[i].position = glm::vec4(GenRandomPos3D(pos, 0.04f),0);
	}
	return particles;
}
template <typename T> void Emitter3D::EmitIntoSSBO(const unsigned int& n, unsigned int& nParticles, SSBO& ssbo) {
	static_assert(false);
}
template <> void Emitter3D::EmitIntoSSBO<Particle3D>(const unsigned int& n, unsigned int& nParticles, SSBO& ssbo) {
	Particle3D* particles = Emit<Particle3D>(n);
	ssbo.Append(particles, n * sizeof(Particle3D), sizeof(Particle3D)*nParticles);
	nParticles += n;
}
