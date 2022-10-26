#pragma once
#include "Spirit.h"
#include "SpiritBase.h"
#include "ParticlesParameter.h"
#include "TranslateParameter.h"
#include "ScalingParameter.h"
#include "RotationParameter.h"
#include "ColorParameter.h"
#include "TextureParameter.h"
#include "RenderParameter.h"

#include <list>

namespace agate
{
	class Particles: 
		public SpiritBase
	{
	public:
		Particles();

		void SetParams(const ParticleParameter& param)
		{
			_Params = param;
		}

		void SetTranslate(const ParticleTranslateParameter& param)
		{
			_Translate = param;
		}

		void SetScaling(const ParticleScalingParameter& param)
		{
			_Scaling = param;
		}

		void SetRotation(const ParticleRotationParameter& param)
		{
			_Rotation = param;
		}
		
		void SetColor(const ParticlColorParameter& param)
		{
			_Color = param;
		}
		
		void SetTexture(const ParticleTextureParameter& param)
		{
			_Texture = param;
		}

		void SetRenderParams(const RenderParameter& param)
		{
			_RenderParams = param;
		}

		int Update(int64_t time) override;

		void Draw(DrawingContext& context) override;

	private:
		void GenerateInstances(int64_t time);
		void AssignTranslate(Spirit* spirit);
		void AssignScaling(Spirit* spirit);
		void AssignRotation(Spirit* spirit);
		void AssignColor(Spirit* spirit);
		void AssignTexture(Spirit* spirit);
	protected:
		std::list<std::shared_ptr<Spirit>> _Particles;
		std::vector<Spirit*> _ShowingParticles;

		uint32_t _ParticleCount;
		int64_t _LastParticleBeginning;
		Randomizer _Random;
		ParticleParameter	_Params;
		ParticleTranslateParameter _Translate;
		ParticleScalingParameter  _Scaling;
		ParticleRotationParameter _Rotation;
		ParticlColorParameter	_Color;
		ParticleTextureParameter  _Texture;
		RenderParameter		_RenderParams;
	};

}