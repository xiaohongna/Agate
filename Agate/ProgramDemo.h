#pragma once
#include "Program.h"
#include "Image.h"
#include <chrono>
#include "ParticleComponent.h"
class ProgramDemo
{
public:
	ProgramDemo();

	void Render(agate::DrawingContext& canvs);

private:
	void AddSpirit();

	void AddParticle();

	void AddTextureParticle();

	void BuildComponent();
private:
	agate::Program _Program;
	agate::ParticleComponent _Particle;
	agate::Image	_Background;
	std::chrono::steady_clock::time_point _BeginTime;
};

