#pragma once
#include "base/Ref.h"
#include "Visual.h"
namespace agate
{
	class AgateAPI Canvas: public Ref
	{
	public:
		void bindContext(void* context)
		{
			_context = context;
		}

		void beginPaint(uint32_t width, uint32_t height);

		void paint(Visual* visual);

		void endPaint();
	private:
		void* _context{};
		std::vector<Share_Ptr<Visual>> _visuals;
	};
}

