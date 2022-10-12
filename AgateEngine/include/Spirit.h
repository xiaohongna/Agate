#pragma once
#include "SpiritBase.h"
#include "Image.h"
#include "RotationParameter.h"
#include "ScalingParameter.h"
#include "TranslateParameter.h"
#include "ColorParameter.h"

namespace agate
{
	class Spirit:
		public SpiritBase
	{
	public:
		Spirit()
		{
			_RotationParam.type = RotationAnimationType::Fixed;
			_RotationParam.params.fixed = 0.0f;
			_ScalingParam.type = ScalingAnimationType::Fixed;
			_ScalingParam.params.fixed = { 1.0f, 1.0f };
			_TranslateParam.type = TranslateAnimationType::Fixed;
			_TranslateParam.params.fixed = { 0.0f, 0.0f };
			_ColorParam.type = ColorAnimationType::Fixed;
			_ColorParam.params.fixed.color = 0xFFFFFFFF;
		}

		void SetSource(const std::wstring& file);

		void SetRotation(const RotationAnimationParameter& param);

		void SetScaling(const ScalingAnimationParameter& param);

		void SetTranslate(const TranslateAnimationParameter& param);

		void SetColor(const ColorAnimationParameter& color);

		int Update(int64_t time) override;

		void Draw(DrawingContext& context) override;

		void SetBlendMode(BlendMode mode)
		{
			_Image.SetBlendMode(mode);
		}
	private:
		void UpdateRotation(int64_t time, float& angle, Vector2& center);

		void UpdateScaling(int64_t time, Vector2& scal);

		void UpdateTranslate(int64_t time, Vector2& trans);

		void UpdateColor(int64_t time);
	protected:
		Image  _Image;
		Vector2	_Size;
		RotationAnimationParameter _RotationParam;
		ScalingAnimationParameter _ScalingParam;
		TranslateAnimationParameter _TranslateParam;
		ColorAnimationParameter _ColorParam;
	};
}

