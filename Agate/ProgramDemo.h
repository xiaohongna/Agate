#pragma once
#include "Program.h"
#include <chrono>

class ProgramDemo
{
public:
	ProgramDemo();

	void Render(agate::DrawingContext& canvs);

private:
	void AddSpirit();
private:
	agate::Program _Program;
	std::chrono::steady_clock::time_point _BeginTime;
};

