#include "Particle.h"
#include "imgui/imgui.h"
#include <memory>
#include <random>

NormalParticle::NormalParticle()
{
	memset(this, 0, sizeof(NormalParticle));
}
NormalParticle::NormalParticle(const glm::vec2& pos)
{
	memset(this, 0, sizeof(NormalParticle));
	position = pos;
}
NormalParticle::NormalParticle(const glm::vec2& pos, const float& offset) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	memset(this, 0, sizeof(NormalParticle));
	position = glm::vec2(pos.x + xo, pos.y + yo);
}
NormalParticle::NormalParticle(const glm::vec2& pos,const glm::vec2& vel)
{
	memset(this, 0, sizeof(NormalParticle));
	position = pos;
	velocity = vel;
}
VertexBufferLayout NormalParticle::GetLayout() {
	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	layout.Push<float>(2);
	layout.Push<float>(2);

	return layout;
}
AdvancedParticle::AdvancedParticle() {
	memset(this, 0, sizeof(AdvancedParticle));
}
AdvancedParticle::AdvancedParticle(const glm::vec2& pos,const LiquidType& type) {
	memset(this, 0, sizeof(AdvancedParticle));
	this->position = pos;
	SetType(*this, type);
}

AdvancedParticle::AdvancedParticle(const glm::vec2& pos, const float& offset,const LiquidType& type) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	memset(this, 0, sizeof(AdvancedParticle));
	position = glm::vec2(pos.x + xo, pos.y + yo);
	SetType(*this, type);
}
AdvancedParticle::AdvancedParticle(const glm::vec2& pos, const glm::vec2& vel,const LiquidType& type)
{
	memset(this, 0, sizeof(AdvancedParticle));
	position = pos;
	velocity = vel;
	SetType(*this,type);
}
void AdvancedParticle::ImGuiRender() {
	ImGui::Text("%10f  %10f  %10f  %10f  %10f  %10f", this->density, this->pressure, this->k, this->pr, this->mass, this->viscosity);
}
void AdvancedParticle::SetType(AdvancedParticle& particle, const LiquidType& type) {
	switch (type) {
	case LiquidType::Water:
		particle.mass = 0.02f;
		particle.viscosity = 3000.f;
		particle.k = 2000.f;
		particle.pr = 1000.f;
		particle.color = glm::vec4(0.0, 0.0, 1.0,1.0);
		break;
	case LiquidType::Oil:
		particle.mass = 0.017f;
		particle.viscosity = 3500.f;
		particle.k = 3000.f;
		particle.pr = 910.f;
		particle.color = glm::vec4(1.0, 1.0, 0.0, 1.0);
		break;
	case LiquidType::Blood:
		particle.mass = 0.02f;
		particle.viscosity = 3000.f;
		particle.k = 2000.f;
		particle.pr = 1000.f;
		particle.color = glm::vec4(1.0, 0.0, 0.0, 1.0);
	}
}
VertexBufferLayout AdvancedParticle::GetLayout() {
	VertexBufferLayout layout;
	layout.Push<float>(2); //position
	layout.Push<float>(2); //velocity
	layout.Push<float>(2); //forces
	layout.Push<float>(4); //color
	layout.Push<float>(2); //DP
	layout.Push<float>(2); //PRK
	layout.Push<float>(2); //MV
	return layout;
}


Particle3D::Particle3D()
{
	memset(this, 0, sizeof(Particle3D));
}
Particle3D::Particle3D(const glm::vec3& pos)
{
	memset(this, 0, sizeof(Particle3D));
	position = pos;
}
Particle3D::Particle3D(const glm::vec3& pos, const float& offset) {
	float xo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float yo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	float zo = -offset + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * offset)));
	memset(this, 0, sizeof(Particle3D));
	position = glm::vec3(pos.x + xo, pos.y + yo,pos.z+zo);
}
Particle3D::Particle3D(const glm::vec3& pos, const glm::vec3& vel)
{
	memset(this, 0, sizeof(Particle3D));
	position = pos;
	velocity = vel;
}
VertexBufferLayout Particle3D::GetLayout() {
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(2);

	return layout;
}