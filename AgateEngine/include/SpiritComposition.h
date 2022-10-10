#pragma once
#include "SpiritBase.h"

namespace agate
{

	class SpiritComposition : public SpiritBase
	{
	public:
		bool Update(int64_t elasped) override;

		void Draw(agate::DrawingContext& context) override;
	};
}

