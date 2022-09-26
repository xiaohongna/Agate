#include "pch.h"
#include "RenderDemo.h"

RenderDemo::RenderDemo()
{
	Figure figure;
	figure.StartAt(200, 200);
	figure.LineTo({ 400, 300 });
	_Simple.AddFigure(std::move(figure));
}

void RenderDemo::Render(DrawingContext& canvs)
{
	canvs.BeginDraw(true, 0xFFFF0000);
	canvs.Draw(_Simple);
	canvs.EndDraw(1);
}
