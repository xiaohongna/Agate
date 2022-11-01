#pragma once
#include "Image.h"
#include "ParticlesParameter.h"
namespace agate
{
	enum class UpdateResult: uint32_t
	{
		/// <summary>
		/// 什么都没有，不需要渲染
		/// </summary>
		Nothing = 1,
		/// <summary>
		/// 需要渲染
		/// </summary>
		NeedRender = 2,
		/// <summary>
		/// 可以释放
		/// </summary>
		Destroyable = 4,
	};

	static UpdateResult operator | (UpdateResult left, UpdateResult right)
	{
		return  UpdateResult((uint32_t)left | (uint32_t)right);
	}

	static bool operator & (UpdateResult left, UpdateResult right)
	{
		return  ((uint32_t)left & (uint32_t)right) != 0;
	}

	struct SpriteInfo
	{
		float rotation = 0.0f;
		Vector2 rotationCenter;
		Vector2 scale; 
		Vector2 scaleCenter;
		Vector2 translate;
		Matrix3X2 GetMatrix() const
		{
			return agate::Matrix3X2::Rotation(rotation, rotationCenter) *
				agate::Matrix3X2::Scale(scale.x, scale.y, scaleCenter) * agate::Matrix3X2::Translation(translate.x, translate.y);
		}
	};

	class ParticleComponent;

	class Sprite: public std::enable_shared_from_this<Sprite>
	{
	public:
		Sprite(int64_t beginning, int64_t ending)
		{
			_Beginning = beginning;
			_Ending = ending;
			_Hidden = false;
			_RotationParam.type = RotationAnimationType::Fixed;
			_RotationParam.params.fixed = 0.0f;
			_ScalingParam.type = ScalingAnimationType::Fixed;
			_ScalingParam.params.fixed = { 1.0f, 1.0f };
			_TranslateParam.type = TranslateAnimationType::Fixed;
			_TranslateParam.params.fixed = { 0.0f, 0.0f };
			_ColorParam.type = ColorAnimationType::Fixed;
			_ColorParam.params.fixed.color = 0xFFFFFFFF;
			_TextureParam.type = TextureAnimationType::Fixed;
			_TextureParam.UVFrame = { 0.0f, 0.0f, 1.0f, 1.0f };
			_Inherite = InheriteBehavior::Never;
		}

		void SetRotation(const RotationAnimationParameter& param);

		void SetScaling(const ScalingAnimationParameter& param);

		void SetTranslate(const TranslateAnimationParameter& param);

		void SetColor(const ColorAnimationParameter& color);

		void SetTexture(const TextureAnimationParameter& texture);

		void SetRenderParams(const RenderParameter& param);

		UpdateResult Update(int64_t time);

		void Draw(DrawingContext& context);

		void DrawChild(DrawingContext& context);

		void SetBlendMode(BlendMode mode)
		{
			_Image.SetBlendMode(mode);
		}

		void SetParent(std::weak_ptr<Sprite>&& parent)
		{
			_Parent = parent;
		}

		std::shared_ptr<Sprite> GetParent()
		{
			return _Parent.lock();
		}

		void AddComponent(std::shared_ptr<ParticleComponent>&& particle);

		const SpriteInfo& GetInfo() const
		{
			return _Info;
		}

		void SetInheriteBehavior(InheriteBehavior inherite)
		{
			_Inherite = inherite;
		}
	private:
		void UpdateRotation(float absTime, float progress, float& angle, Vector2& center);

		void UpdateScaling(float absTime, float progress, Vector2& scal);

		void UpdateTranslate(float absTime, float progress, Vector2& trans);

		void UpdateColor(float absTime, float progress);

		void UpdateTexture(float absTime, float progress);

		void UpdataInherite();
	protected:		
		int64_t _Beginning;   
		int64_t _Ending;
		std::weak_ptr<Sprite>  _Parent;
		std::vector<std::shared_ptr<ParticleComponent>> _ChildParticle;
		Image  _Image;
		SpriteInfo  _Info;
		bool _Hidden;
		InheriteBehavior _Inherite;
		RotationAnimationParameter _RotationParam;
		ScalingAnimationParameter _ScalingParam;
		TranslateAnimationParameter _TranslateParam;
		ColorAnimationParameter _ColorParam;
		TextureAnimationParameter _TextureParam;
	};
}

