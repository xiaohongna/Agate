#pragma once
#include "DrawingContext.h"

class TimelineObject
{
public:
	virtual void Update(uint32_t elasped) = 0;
	virtual void Draw(agate::DrawingContext& context) = 0;
protected:
	int64_t _Begin;
	int64_t _End;
};

