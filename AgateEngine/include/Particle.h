#pragma once
#include "Spirit.h"
#include "SpiritBase.h"
#include "ParticlesParameter.h"
#include "TranslateParameter.h"
#include "ScalingParameter.h"
#include "RotationParameter.h"
#include "ColorParameter.h"

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

		void SetTranslate(const ParticleTranslateParameter& param);

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
		
		bool Update(int64_t time) override;

		void Draw(DrawingContext& context) override;

		void SetTexture(const std::wstring& file);
	private:
		void GenerateInstances();
		void AssignTranslate(Spirit* spirit);
		void AssignScaling(Spirit* spirit);
		void AssignRotation(Spirit* spirit);
		void AssignColor(Spirit* spirit);
	protected:
		std::list<std::shared_ptr<Spirit>> _Particles;
		std::vector<Spirit*> _ShowingParticles;
		std::wstring _Texture;

		Randomizer _Random;
		ParticleParameter	_Params;
		ParticleTranslateParameter _Translate;
		ParticleScalingParameter  _Scaling;
		ParticleRotationParameter _Rotation;
		ParticlColorParameter	_Color;
	};

}