#pragma once
#include "DrawingContext.h"
#include "Spirit.h"
#include "Geometry.h"

class RenderDemo
{
public:
	RenderDemo();
	void Render(DrawingContext& canvs);
	void SetViewSize(uint32_t width, uint32_t height);
private:
	void RenderGeomegry(DrawingContext& canvs);
	void RenderSpirit(DrawingContext& canvs);
	void RenderSpiritColor(DrawingContext& canvs);
private:
	Geometry  _Line;
	Geometry  _Bezier;
	Geometry  _Rectangle;
	Geometry  _RoundedRect;
	Geometry  _Ellipse;
	float _Rotation;
	Vector2 _Scale;
	Color   _SpiritColor;
	std::shared_ptr<Image> _Image;
	Spirit	_Spirit;
	Spirit	_Background;
};



