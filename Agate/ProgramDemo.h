#pragma once
#include "Image.h"
#include <chrono>
#include "ParticleComponent.h"
class ProgramDemo
{
public:
	ProgramDemo();

	void Render(agate::DrawingContext& canvs);

private:
	void BuildComponent();
private:
	agate::ParticleComponent _Particle;
	agate::Image	_Background;
	std::chrono::steady_clock::time_point _BeginTime;
};

