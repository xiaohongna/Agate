#include "Spirit.h"

namespace agate
{
	void Spirit::SetSource(const std::wstring& file)
	{
		auto texture = agate::Texture::CreateFromFile(file);
		_Image.SetTexture(texture);
		auto& img = texture->GetImageData();
		_Image.SetAntialiasing(true);
		_Image.SetBounds({ 0.0f, 0.0f, (float)img.width, (float)img.height });
		_Image.SetClip({ 0.0f, 0.0f, (float)img.width, (float)img.height });
	}

	void Spirit::SetRotation(const RotationAnimationParameter& param)
	{
		_RotationParam = param;
		if (_RotationParam.type == RotationAnimationType::Fixed)
		{

		}
	}

	inline float Step(float step, float from, float to)
	{
		return (to - from)* step + from;
	}

	bool Spirit::Update(int64_t time)
	{
		if (time < _Begin)
		{
			return false;
		}
		else if (time > _End)
		{
			return false;
		}
		float angle = 0.0f;
		switch (_RotationParam.type)
		{
		case RotationAnimationType::Fixed:
			angle = _RotationParam.Params.fixed;
			break;
		case RotationAnimationType::FromTo:
		{
			float progress = (time - _Begin) / (float)(_End - _Begin);
			angle = Step(progress, _RotationParam.Params.from, _RotationParam.Params.to);
		}
			break;
		case RotationAnimationType::PVA:
			break;
		default:
			break;
		} 
		auto matrix = agate::Matrix3X2::Rotation(angle, _RotationParam.pointer);
		_Image.SetTransform(matrix);
		return true;
	}

	void Spirit::Draw(DrawingContext& context)
	{
		context.Draw(_Image);
	}
}