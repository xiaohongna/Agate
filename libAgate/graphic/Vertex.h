#pragma once
#include <stdint.h>
#include <vector>
#include "base/Vector.h"

namespace agate
{
	typedef unsigned short DrawIndex;

	enum class VertexType
	{
		XYColor,
		XYUVColor,
	};

	struct VertexXYColor
	{
		Vector2 pos;
		uint32_t col;
	};

	struct VertexXYUV
	{
		Vector2 pos;
		Vector2 uv;
	};

	struct VertexXYUVColor
	{
		Vector2 pos;
		Vector2 uv;
		uint32_t col;
	};
}