#pragma once
#include <memory>
#include "DrawingContext.h"

namespace agate
{

	class SpiritBase
	{
	public:
		void Range(int64_t b, int64_t e)
		{
			_Begin = b;
			_End = e;
		}
		virtual bool Update(int64_t time) = 0;
		virtual void Draw(DrawingContext& context) = 0;
	protected:
		int64_t _Begin;
		int64_t _End;
	};

}