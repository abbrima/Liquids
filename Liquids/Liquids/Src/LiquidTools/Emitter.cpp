#include "Emitter.h"
#define MAX_P_EMIT 500
Emitter::Emitter(glm::vec2& pos):pos(pos) {}
Emitter::~Emitter() {}
static glm::vec2 GenRandomPos(const glm::vec2& pos, const float& offset) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	return glm::vec2(pos.x + xo, pos.y + yo);
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