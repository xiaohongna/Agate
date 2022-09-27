#pragma once
#include "DrawingContext.h"


class RenderDemo
{
public:
	RenderDemo();
	void Render(DrawingContext& canvs);
private:
	Geometry  _Line;
	Geometry  _bezier;
};



