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
		Image();

		void SetAntialiasing(bool enable);

		void SetEffect(PipelineType effect);

		void SetBounds(const Vector4& bounds);

		void SetTexture(uint32_t index, const std::shared_ptr<ImageAsset>& img, SamplerMode sampler = SamplerMode::PointClamp);

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
		SamplerMode	_ImageSampler[MaxTextureCount];
		Vector4 _Bounds;
		Vector4 _Clip;
		bool _NormalUV;
		Color	_Color;
		Matrix3X2 _Matrix;
		RasterizeData _RasterData;
		uint32_t	_Flags;
	};

}