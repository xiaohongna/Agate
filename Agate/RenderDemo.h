#pragma once
#include "DrawingContext.h"


class RenderDemo
{
public:
	RenderDemo();
	void Render(DrawingContext& canvs);
private:
	Geometry  _Simple;
};



