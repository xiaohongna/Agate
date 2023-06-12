#pragma once
#include "Component.h"
#include "graphic/Geometry.h"

namespace agate
{
	class AgateAPI Frame: public Component
	{
	protected:
		void selfDraw(Canvas* canvas) override;
	private:
		Share_Ptr<Geometry> _background;
	};
}
