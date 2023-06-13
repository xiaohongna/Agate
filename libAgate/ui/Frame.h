#pragma once
#include "Component.h"
#include "graphic/Geometry.h"

namespace agate
{
	class AgateAPI Frame: public Component
	{
	public:
		Frame();

		void setBackgroundColor(const Color& clr);

		void setBounds(const Rect& rect);

		const Rect& bounds()
		{
			return _bounds;
		}
	protected:
		void selfDraw(Canvas* canvas) override;
	private:
		Rect _bounds;
		Share_Ptr<Geometry> _background;
	};
}
