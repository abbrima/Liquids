#include "Pipe.h"
#include <memory>

Pipe::Pipe(float base, float power, float dx, float dy, float dr) {
	this->base = base;
	this->power = power;
	this->dX = dx;
	this->dY = dy;
	this->dampeningRatio = dr;
	memset(this, 0, sizeof(float) * 4);
}
void Pipe::setConstraints(float lowX, float lowY, float highX, float highY) {
	this->lowX = lowX; 
	this->lowY = lowY; 
	this->highX = highX; 
	this->highY = highY;
}
float Pipe::f(float x) {
	return (base * pow(e,(x+dX)*power)) + dY;
}