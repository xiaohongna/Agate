#include "pch.h"
#include "RenderDemo.h"

void RenderDemo::Render(DrawingContext& canvs)
{
	canvs.BeginDraw(true, 0xFFFF0000);
	canvs.EndDraw(1);
}
