#include "Pipe.h"
#include <memory>

Pipe::Pipe(const float& base, const float& power, const float& dx, const float& dy, const float& dr,const bool& upper)
	:base(base),power(power),dX(dx),dY(dy),dampeningRatio(dr),upper(upper?1:0)
{
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
VerticalPipe::VerticalPipe(const float& x, const float& lowy, const float& highy, const float& dampeningRatio)
	:x(x), lowY(lowy), highY(highy), dampeningRatio(dampeningRatio) {}

SinusoidalPipe::SinusoidalPipe(const float& base, const float& power, const float& xCo, const float& dx, const float& dy, const float& dr, const bool& upper)
	: base(base), xPower(power), xCo(xCo), dX(dx), dY(dy), dampeningRatio(dr), upper(upper)
{
	memset(this, 0, sizeof(float) * 4);
}
void SinusoidalPipe::setConstraints(const float& lowX, const float& lowY, const float& highX, const float& highY) {
	this->lowX = lowX;
	this->lowY = lowY;
	this->highX = highX;
	this->highY = highY;
}
float SinusoidalPipe::f(const float& x) {
	return base * glm::sin(xCo*pow(x + dX, xPower)) + dY;
}
