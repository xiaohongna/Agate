#pragma once
#include <memory>
#include "Vector.h"
#include "Matrix.h"
#include "Drawingable.h"
#include "Brush.h"

class Spirit: public Drawingable
{
public:
	Spirit():_Matrix {},
		_Flags{0},
		_Color{0xFFFFFFFF}
	{
		_Matrix._11 = 1.0f;
		_Matrix._22 = 1.0f;
		_RasterData.pipline = PipelineType::TextureColor;
	}

	void SetBounds(const Vector4& bounds);

	void SetImage(const std::shared_ptr<Image>& img);

	void SetClip(const Vector4& clip);

	void SetColor(const Color clr);

	void SetBlendMode(BlendMode blend)
	{
		_RasterData.blend = blend;
	}

	void SetTransform(const Matrix3X2& matrix);

	uint32_t Rasterize(DrawingContext& context) override;

	const RasterizeData& GetRasterizeData(uint32_t index) override;
private:
	void AddFlag(uint32_t flag)
	{
		_Flags = _Flags | flag;
	}

	void RemoveFlag(uint32_t flag)
	{
		_Flags = _Flags & ~flag;
	}

	bool HaveFlag(uint32_t flag)
	{
		return (_Flags & flag) == flag;
	}

	void RasterizeNoAntiAliasing(Vector2* rect);

	void Rasterize();

	void UpdateUV();

	void UpdateColor();

private:
	std::shared_ptr<Image>	_Image;
	Vector4 _Bounds;
	Vector4 _Clip;
	Color	_Color;
	Matrix3X2 _Matrix;
	RasterizeData _RasterData;
	uint32_t	_Flags;
};

