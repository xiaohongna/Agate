#pragma once
#include "pch.h"
#include <d2d1.h>
namespace Agate
{
	class D2DRenderEngine;

	class RectangeVisual
	{
	public:
		void Render(D2DRenderEngine* renderer);
	private:
		ID2D1SolidColorBrush* m_brush;
	};
}

