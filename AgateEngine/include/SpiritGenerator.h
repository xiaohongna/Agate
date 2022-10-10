#pragma once
#include "TimelineObject.h"

class SpiritGenerator: public TimelineObject
{
public:
	void Update(uint32_t elasped) override;

	void Draw(agate::DrawingContext& context) override;

};


