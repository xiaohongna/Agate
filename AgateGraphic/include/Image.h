#pragma once
#include <memory>
#include "Vector.h"
#include "Matrix.h"
#include "Drawable.h"
#include "Brush.h"
namespace agate
{
	class Image : public Drawable
	{
	public:
		Image() :_Matrix{},
			_Flags{ 0 },
			_Color{ 0xFFFFFFFF },
			_NormalUV{true},
			_Clip{0.0f, 0.0f, 1.0f, 1.0f}
		{
			_Matrix._11 = 1.0f;
			_Matrix._22 = 1.0f;
			_RasterData.pipline = PipelineType::TextureColor;
		}

		void SetAntialiasing(bool enable);

		void SetEffect(PipelineType effect);

		void SetBounds(const Vector4& bounds);

		void SetTexture(uint32_t index, const std::shared_ptr<ImageAsset>& img);

		void SetClip(const Vector4& clip, bool normal = false);

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
		std::shared_ptr<ImageAsset>	_Images[MaxTextureCount];
		Vector4 _Bounds;
		Vector4 _Clip;
		bool _NormalUV;
		Color	_Color;
		Matrix3X2 _Matrix;
		RasterizeData _RasterData;
		uint32_t	_Flags;
	};

}