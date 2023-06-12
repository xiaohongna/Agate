#include "Canvas.h"
namespace agate
{
	void Canvas::beginPaint(uint32_t width, uint32_t height)
	{

	}

	void Canvas::paint(Visual* visual)
	{
		_visuals.emplace_back(visual);
	}

	void Canvas::endPaint()
	{
		_visuals.clear();
	}
}