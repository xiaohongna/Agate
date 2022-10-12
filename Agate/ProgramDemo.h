#pragma once
#include "Program.h"
#include "Image.h"
#include <chrono>

class ProgramDemo
{
public:
	ProgramDemo();

	void Render(agate::DrawingContext& canvs);

private:
	void AddSpirit();

	void AddParticle();
private:
	agate::Program _Program;
	agate::Image	_Background;
	std::chrono::steady_clock::time_point _BeginTime;
};

