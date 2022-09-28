#pragma once
#include "Vertex.h"

union Color
{
	struct 
	{
		uint8_t alpha;
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
	uint32_t color;
};


class Brush
{
public:
	Brush(Color color);

	VertexType GetVertexType();
};

