#include "Spirit.h"

namespace agate
{
	void Spirit::SetRotation(const RotationAnimationParameter& param)
	{
		_RotationParam = param;
		if (_RotationParam.type == RotationAnimationType::Fixed)
		{

		}
	}

	void Spirit::SetScaling(const ScalingAnimationParameter& param)
	{
		_ScalingParam = param;
	}

	void Spirit::SetTranslate(const TranslateAnimationParameter& param)
	{
		_TranslateParam = param;
	}

	void Spirit::SetColor(const ColorAnimationParameter& color)
	{
		_ColorParam = color;
	}

	void Spirit::SetTexture(const TextureAnimationParameter& texture)
	{
		_TextureParam = texture;
	}

	void Spirit::SetRenderParams(const RenderParameter& param)
	{
		_Image.SetBlendMode(param.blend);
		_Image.SetAntialiasing(param.antialiasing);
		auto texture = agate::Texture::CreateFromFile(param.filePath);
		_Image.SetTexture(texture);
		if (param.size.x < 0.0001 || param.size.y < 0.0001)
		{
			auto& img = texture->GetImageData();
			_Size.x = (float)img.width / 2.0f;
			_Size.y = (float)img.height / 2.0f;
			_Image.SetBounds({ 0.0f, 0.0f, (float)img.width, (float)img.height });
		}
		else
		{
			_Size = param.size * 0.5f;
			_Image.SetBounds({ 0.0f, 0.0f, param.size.x, param.size.y });
		}
	}

	inline float Step(float step, float from, float to)
	{
		return (to - from)* step + from;
	}

	inline Vector2 Step(float step, const Vector2& from, const Vector2& to)
	{
		return (to - from) * step + from;
	}

	float Spirit::Update(int64_t time)
	{
		auto progress = (float)(time - _Beginning) / (_Ending - _Beginning);
		if (progress < 0.0f || progress > 1.0f)
		{
			return progress;
		}
		auto const absTime = (time - _Beginning) / 1000.0f;
		float angle = 0.0f;
		Vector2 center, scale{1.0f, 1.0f}, trans;
		UpdateRotation(absTime, progress, angle, center);
		UpdateScaling(absTime, progress, scale);
		UpdateTranslate(absTime, progress, trans);
		auto matrix = agate::Matrix3X2::Rotation(angle, _RotationParam.center) * 
			agate::Matrix3X2::Scale(scale.x, scale.y, _Size) * agate::Matrix3X2::Translation(trans.x, trans.y);
		_Image.SetTransform(matrix);
		UpdateColor(absTime, progress);
		UpdateTexture(absTime, progress);
		return progress;
	}

	void Spirit::Draw(DrawingContext& context)
	{
		context.Draw(_Image);
	}

	void Spirit::UpdateRotation(float absTime, float progress, float& angle, Vector2& center)
	{
		switch (_RotationParam.type)
		{
		case RotationAnimationType::Fixed:
			angle = _RotationParam.params.fixed;
			break;
		case RotationAnimationType::FromTo:
		{
			angle = Step(progress, _RotationParam.params.from, _RotationParam.params.to);
		}
		break;
		case RotationAnimationType::PVA:
		{
			angle = _RotationParam.params.base + (_RotationParam.params.velocity * absTime) +
				(_RotationParam.params.acceleration * absTime * absTime * 0.5f);
		}
		break;
		default:
			assert(false);
			break;
		}
		center = _RotationParam.center;
	}

	void Spirit::UpdateScaling(float absTime, float progress, Vector2& scal)
	{
		switch (_ScalingParam.type)
		{
		case ScalingAnimationType::Fixed:
			scal = _ScalingParam.params.fixed;
			break;
		case ScalingAnimationType::UniformFromTo:
		{
			scal.x = scal.y = Step(progress, _ScalingParam.params.uniformFrom, _ScalingParam.params.uniformTo);
		}
			break;
		case ScalingAnimationType::UniformPVA:
		{
			scal.x = scal.y = _ScalingParam.params.uniformBase + (_ScalingParam.params.uniformVelocity * absTime) +
				(_ScalingParam.params.uniformAcceleration * absTime * absTime * 0.5f);
		}
			break;
		case ScalingAnimationType::FromTo:
		{
			scal = Step(progress, _ScalingParam.params.from, _ScalingParam.params.to);
		}
		break;
		case ScalingAnimationType::PVA:
		{
			scal = _ScalingParam.params.base + (_ScalingParam.params.velocity * absTime) +
				(_ScalingParam.params.acceleration * absTime * absTime * 0.5f);
		}
		break;
		default:
			assert(false);
			break;
		}
	}
	
	void Spirit::UpdateTranslate(float absTime, float progress, Vector2& trans)
	{
		switch (_TranslateParam.type)
		{
		case TranslateAnimationType::Fixed:
			trans = _TranslateParam.params.fixed;
			break;
		case TranslateAnimationType::FromTo:
		{
			trans = Step(progress, _TranslateParam.params.from, _TranslateParam.params.to);
		}
		break;
		case TranslateAnimationType::PVA:
		{
			trans = _TranslateParam.params.base + (_TranslateParam.params.velocity * absTime) +
				(_TranslateParam.params.acceleration * absTime * absTime * 0.5f);
		}
		break;
		default:
			break;
		}
	}

	inline uint8_t GradientChannel(float progress, uint8_t from, uint8_t to)
	{
		return uint16_t((to - from) * progress + 0.5f) + from;
	}

	Color GradientColor(float progress, Color from, Color to)
	{
		return 
		{
			GradientChannel(progress, from.red, to.red),
			GradientChannel(progress, from.green, to.green),
			GradientChannel(progress, from.blue, to.blue),
			GradientChannel(progress, from.alpha, to.alpha),
		};
	}

	void Spirit::UpdateColor(float absTime, float progress)
	{
		switch (_ColorParam.type)
		{
		case(ColorAnimationType::Fixed):
			_Image.SetColor(_ColorParam.params.fixed);
			break;
		case (ColorAnimationType::FromTo):
		{
			_Image.SetColor(GradientColor(progress, _ColorParam.params.from, _ColorParam.params.to));
		}
			break;
		default:
			break;
		}
	}

	void Spirit::UpdateTexture(float absTime, float progress)
	{
		switch (_TextureParam.type)
		{
		case(TextureAnimationType::Fixed):
			_Image.SetClip(_TextureParam.UVFrame, true);
			break;
		case(TextureAnimationType::VerticalScroll):
		{
			float yoffset = absTime * _TextureParam.params.scallSpeed;
			_Image.SetClip({ _TextureParam.UVFrame.x, _TextureParam.UVFrame.y + yoffset, 
				_TextureParam.UVFrame.right, _TextureParam.UVFrame.bottom + yoffset}, true);
		}
			break;
		case(TextureAnimationType::HorizontalScroll):
		{
			float xoffset = absTime * _TextureParam.params.scallSpeed;
			_Image.SetClip({ _TextureParam.UVFrame.x + xoffset, _TextureParam.UVFrame.y,
				_TextureParam.UVFrame.right + xoffset, _TextureParam.UVFrame.bottom }, true);
		}
			break;
		case(TextureAnimationType::FramebyFrame):
		{
			int frameIndex = (int)(absTime * _TextureParam.params.frameRate + 0.5f) % _TextureParam.params.frameCount;
			float x = (frameIndex % _TextureParam.params.lineFrameCount) * _TextureParam.UVFrame.Width();
			float y = frameIndex / _TextureParam.params.lineFrameCount * _TextureParam.UVFrame.Height();
			_Image.SetClip({ _TextureParam.UVFrame.x + x, _TextureParam.UVFrame.y + y,
				_TextureParam.UVFrame.right + x, _TextureParam.UVFrame.bottom + y }, true);
		}
			break;
		default:
			assert(false);
				break;
		}
	}
}