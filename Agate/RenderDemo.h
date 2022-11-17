#pragma once
#include "DrawingContext.h"
#include "Image.h"
#include "Geometry.h"
#include <memory>

class RenderDemo
{
public:
	RenderDemo();
	void Render(agate::DrawingContext& canvs);
	void SetViewSize(uint32_t width, uint32_t height);
private:
	void RenderGeomegry(agate::DrawingContext& canvs);
	void RenderSpirit(agate::DrawingContext& canvs);
	void RenderSpiritColor(agate::DrawingContext& canvs);
	void OffScreen(agate::DrawingContext& canvas);
	void DrawRect(agate::DrawingContext& canvas, int count);
	void DisplacementRender(agate::DrawingContext& canvas);
private:
	agate::Geometry  _Line;
	agate::Geometry  _Bezier;
	agate::Geometry  _Rectangle;
	agate::Geometry  _RoundedRect;
	agate::Geometry  _Ellipse;
	float _Rotation;
	agate::Vector2 _Scale;
	agate::Color   _SpiritColor;
	std::shared_ptr<agate::ImageAsset> _Texture;
	agate::Image	_Spirit;
	agate::Image	_Background;
	std::shared_ptr<agate::ImageAsset> _OffScreen;
	agate::Image	_OffScreenImage;
	std::unique_ptr<agate::Image>	_DisplacementImage;
};


//Generator


