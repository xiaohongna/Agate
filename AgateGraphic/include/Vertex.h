#pragma once
#include <stdint.h>
#include <vector>
#include "Vector.h"

typedef unsigned short DrawIndex;

enum class PipelineType
{
	Color,
	TextureColor,
};

enum class BlendMode
{
	/// <summary>
	/// Alpha 混合
	/// </summary>
	Blend,
	/// <summary>
	/// 不透明
	/// </summary>
	Opacity,
	/// <summary>
	/// 叠加
	/// </summary>
	Additive,
	/// <summary>
	/// 相减
	/// </summary>
	Subtract,
	/// <summary>
	/// 相乘
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
