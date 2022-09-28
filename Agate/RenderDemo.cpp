#include "pch.h"
#include "RenderDemo.h"

RenderDemo::RenderDemo()
{
	{
		Figure figure;
		figure.StartAt(200, 200);
		figure.LineTo({ 400, 300 });
		_Line.SetStrokeWidth(5);
		_Line.AddFigure(std::move(figure));
	}

	{
		Figure figure;
		figure.StartAt(156, 257);
		figure.BezierTo({ 51, 278 }, {252, 114}, {139, 107});
		_Bezier.SetStrokeWidth(2);
		_Bezier.AddFigure(std::move(figure));
	}

	_Rectangle.AddFigure(Figure::InitAsRectangle(Vector4(250, 20, 390, 500)));
	//_Rectangle.SetStrokeWidth(2.0f);
	_RoundedRect.AddFigure(Figure::InitAsRoundedRectangle(Vector4(230.5f, 10.5f, 380.5, 180.5), 20, 20));
	_RoundedRect.SetStrokeWidth(2.0f);
}

void RenderDemo::Render(DrawingContext& canvs)
{
	canvs.BeginDraw(true, 0xFFFF0000);
	
	//canvs.Draw(_Bezier);
	//canvs.Draw(_Rectangle);
	//canvs.SetClip({ 250, 20, 390, 500 });
	canvs.Draw(_RoundedRect);
	//canvs.Draw(_Line);
	canvs.EndDraw(1);
}
