#pragma once

struct Pipe{
	float lowX, lowY, highX, highY, base, dY, dX, power, dampeningRatio;
	Pipe(float base, float power, float dx, int dy, float dr);
};

Pipe setConstraints(Pipe* p, float lowX, float lowY, float highX, float highY);