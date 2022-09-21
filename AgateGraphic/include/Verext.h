#pragma once
#include <stdint.h>
#include "Vector.h"

enum class VertextType
{
	XYColor,
	XYUV,
	XYUVColor,
};

struct VerextXYColor
{
	Vector2 pos;
	uint32_t col;
};

struct VerextXYUV
{
	Vector2 pos;
	Vector2 uv;
};

struct VerextXYUVColor
{
	Vector2 pos;
	Vector2 uv;
	uint32_t col;
};

