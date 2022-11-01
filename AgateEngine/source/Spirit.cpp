#include "Spirit.h"
#include "ParticleComponent.h"
namespace agate
{
	void Sprite::SetRotation(const RotationAnimationParameter& param)
	{
		_RotationParam = param;
		if (_RotationParam.type == RotationAnimationType::Fixed)
		{

		}
	}

	void Sprite::SetScaling(const ScalingAnimationParameter& param)
	{
		_ScalingParam = param;
	}

	void Sprite::SetTranslate(const TranslateAnimationParameter& param)
	{
		_TranslateParam = param;
	}

	void Sprite::SetColor(const ColorAnimationParameter& color)
	{
		_ColorParam = color;
	}

	void Sprite::SetTexture(const TextureAnimationParameter& texture)
	{
		_TextureParam = texture;
	}

	void Sprite::SetRenderParams(const RenderParameter& param)
	{
		_Image.SetBlendMode(param.blend);
		_Image.SetAntialiasing(param.antialiasing);
		auto texture = agate::Texture::CreateFromFile(param.filePath);
		_Image.SetTexture(texture);
		if (param.size.x < 0.0001 || param.size.y < 0.0001)
		{
			auto& img = texture->GetImageData();
			_Info.scaleCenter = { (float)img.width, (float)img.height };
			_Image.SetBounds({ 0.0f, 0.0f, (float)img.width, (float)img.height });
		}
		else
		{
			_Info.scaleCenter = param.size * 0.5f;
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

	UpdateResult Sprite::Update(int64_t time)
	{
		time -= _Beginning;
		if (time < 0)
		{
			return UpdateResult::Nothing;
		}
		auto result = UpdateResult::Nothing;
		auto progress = (float)(time) / (_Ending - _Beginning);
		auto const absTime = time / 1000.0f;
		if (progress <= 1.0f)
		{
			UpdateRotation(absTime, progress, _Info.rotation, _Info.rotationCenter);
			UpdateScaling(absTime, progress, _Info.scale);
			UpdateTranslate(absTime, progress, _Info.translate);
			UpdataInherite();
			auto matrix = _Info.GetMatrix();
			_Image.SetTransform(matrix);
			UpdateColor(absTime, progress);
			UpdateTexture(absTime, progress);
			result = UpdateResult::NeedRender;
		}
		else
		{
			_Hidden = true;
			result = UpdateResult::Destroyable;
		}

		if (_ChildParticle.empty() == false)
		{
			auto sharedSelf = shared_from_this();
			for (auto& child : _ChildParticle)
			{
				result = result | child->Update(sharedSelf, time);
			}
		}
		if (result & UpdateResult::NeedRender)
		{
			return UpdateResult::NeedRender;
		}
		else if (result & UpdateResult::Nothing)
		{
			return  UpdateResult::Nothing;
		}
		else
		{
			return UpdateResult::Destroyable;
		}
	}

	void Sprite::Draw(DrawingContext& context)
	{
		if (!_Hidden)
		{
			context.Draw(_Image);
		}
	}

	void Sprite::DrawChild(DrawingContext& context)
	{
		for (auto& child : _ChildParticle)
		{
			child->Draw(context);
		}
	}

	void Sprite::AddComponent(std::shared_ptr<ParticleComponent>&& particle)
	{
		_ChildParticle.emplace_back(particle);
	}

	void Sprite::UpdateRotation(float absTime, float progress, float& angle, Vector2& center)
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

	void Sprite::UpdateScaling(float absTime, float progress, Vector2& scal)
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
	
	void Sprite::UpdateTranslate(float absTime, float progress, Vector2& trans)
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

	void Sprite::UpdateColor(float absTime, float progress)
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

	void Sprite::UpdateTexture(float absTime, float progress)
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

	void Sprite::UpdataInherite()
	{
		if (_Inherite == InheriteBehavior::Never)
		{
			return;
		}
		auto parent = _Parent.lock();
		if (parent == nullptr)
		{
			return;
		}
		auto& parentInfo = parent->GetInfo();
		if (_Inherite & InheriteBehavior::PositionAlways)
		{
			_Info.translate = _Info.translate + parentInfo.translate;
		}
	}
}