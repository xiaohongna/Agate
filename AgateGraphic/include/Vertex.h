#pragma once
#include <stdint.h>
#include <vector>
#include "Vector.h"

typedef unsigned short DrawIndex;

enum class PiplineType
{
	Color,
	Texture,
	TextureColor,
};

enum class BlendMode
{
	/// <summary>
	/// Alpha ���
	/// </summary>
	Blend,  
	/// <summary>
	/// ����
	/// </summary>
	Additive,
	/// <summary>
	/// ���
	/// </summary>
	Subtract,
	/// <summary>
	/// ���
	/// </summary>
	Multiply

};

enum class VertexType
{
	XYColor,
	XYUV,
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
