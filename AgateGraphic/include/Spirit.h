#pragma once
#include <memory>
#include "Vector.h"
#include "Matrix.h"
#include "Drawingable.h"



class Spirit: public Drawingable
{
public:
	Spirit():_Matrix {},
		_Flags{0}
	{
		_Matrix._11 = 1.0f;
		_Matrix._22 = 1.0f;
		_RasterData.pipline = PipelineType::TextureColor;
	}
	void SetBounds(const Vector4& bounds)
	{
		_Bounds = bounds;
	}

	void SetImage(const std::shared_ptr<Image>& img);

	void SetClip(const Vector4& clip);

	void SetBlendMode(BlendMode blend);

	void SetTransform(const Matrix3X2 matrix);

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
private:
	std::shared_ptr<Image>	_Image;
	Vector4 _Bounds;
	Vector4 _Clip;
	Matrix3X2 _Matrix;
	RasterizeData	_RasterData;
	uint32_t	_Flags;
};

