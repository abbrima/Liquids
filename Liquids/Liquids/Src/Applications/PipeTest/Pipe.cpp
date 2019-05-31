#include "Pipe.h"
#include <memory>

Pipe::Pipe(const float& base, const float& power, const float& dx, const float& dy, const float& dr) {
	this->base = base;
	this->power = power;
	this->dX = dx;
	this->dY = dy;
	this->dampeningRatio = dr;
	memset(this, 0, sizeof(float) * 4);
}
void Pipe::setConstraints(const float& lowX, const float& lowY, const float& highX, const float& highY) {
	this->lowX = lowX; 
	this->lowY = lowY; 
	this->highX = highX; 
	this->highY = highY;
}
float Pipe::f(const float& x) {
	return (base * pow(glm::e<float>(),(x+dX)*power)) + dY;
}