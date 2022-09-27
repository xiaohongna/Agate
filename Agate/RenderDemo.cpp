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
		_bezier.SetStrokeWidth(2);
		_bezier.AddFigure(std::move(figure));
	}
}

void RenderDemo::Render(DrawingContext& canvs)
{
	canvs.BeginDraw(true, 0xFFFF0000);
	canvs.Draw(_Line);
	canvs.Draw(_bezier);
	canvs.EndDraw(1);
}
