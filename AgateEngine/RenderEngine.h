#pragma once
#include <memory>
#include "ShapeVisual.h"
namespace Agate
{
	class RenderEngine
	{
	public:
		RenderEngine(HWND wnd) {};
		virtual ~RenderEngine() {};
		virtual void Render(RectangeVisual* root) = 0;
		virtual std::shared_ptr<RectangeVisual> CreateRectangeVisual() = 0;
	};
}

