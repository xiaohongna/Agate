#pragma once
#include "DrawingContext.h"


class RenderDemo
{
public:
	RenderDemo();
	void Render(DrawingContext& canvs);
private:
	Geometry  _Line;
	Geometry  _Bezier;
	Geometry  _Rectangle;
	Geometry  _RoundedRect;
	Geometry  _Ellipse;
	float _Rotation;
};



