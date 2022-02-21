#include "pch.h"
#include "ShapeVisual.h"
#include "D2DRenderEngine.h"

namespace Agate
{
	void RectangeVisual::Render(D2DRenderEngine* renderer)
	{
		
		if (!m_brush)
		{
			D2D1_COLOR_F  color{0};
			color.r = 1.0f;
			color.a = 1.0f;
			renderer->m_pDeviceContext->CreateSolidColorBrush(color, &m_brush);
		}
		D2D1_RECT_F rect{ 20, 30, 150, 160};
		renderer->m_pDeviceContext->DrawRectangle(rect, m_brush);
	}
}
