#include "Frame.h"

namespace agate
{
	Frame::Frame():_bounds(0, 0, 100, 100)
	{
		_background = Geometry::initAsRectangle(_bounds);
	}

	void Frame::setBackgroundColor(const Color& clr)
	{
		_background->setFillColor(clr);
	}

	void Frame::setBounds(const Rect& rect)
	{
		_bounds = rect;
		_background->setRect(_bounds);
	}

	void Frame::selfDraw(Canvas* canvas)
	{
		_background->draw(canvas);
	}
}


