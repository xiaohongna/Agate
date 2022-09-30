#pragma once
#include <memory>
#include "Vector.h"
#include "Matrix.h"
#include "Drawingable.h"

class Spirit: public Drawingable
{
public:
	Spirit():_Matrix {}
	{
		_Matrix._11 = 1.0f;
		_Matrix._22 = 1.0f;
	
	}

	void SetImage(std::shared_ptr<Image> img);

	void SetClip(const Vector4& clip);

	void SetBlendMode(BlendMode blend);

	void SetTransform(const Matrix3X2 matrix);

	uint32_t Rasterize() override;

	const RasterizeData& GetRasterizeData(uint32_t index) override;
private:
	std::shared_ptr<Image>	_Image;
	Vector2 _Size;
	Vector4 _ImageClip;
	Matrix3X2 _Matrix;
	RasterizeData	_RasterData;
};

