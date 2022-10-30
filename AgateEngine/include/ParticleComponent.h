#pragma once
#include <vector>
#include <list>
#include <memory>
#include "DrawingContext.h"
#include "Spirit.h"
#include "ParticlesParameter.h"

namespace agate
{
	class ParticleComponent
	{
	public:
		ParticleComponent();

		ParticleComponent(std::shared_ptr<ParticleParameter>& param);

		void SetParams(std::shared_ptr<ParticleParameter>& param)
		{
			_Params = param;
		}

		int Update(int64_t time);

		int Update(std::shared_ptr<Sprite>& sprite, int64_t time);

		void Draw(DrawingContext& context);

		void AddChild(std::shared_ptr<ParticleComponent>&& child)
		{
			_Children.emplace_back(child);
		}

		std::shared_ptr<ParticleComponent> ReferenceClone();
	protected:
		void UpdateChildren(int64_t time);
		void GenerateInstances(std::shared_ptr<Sprite>& parent, int64_t time);
		void AssignTranslate(Sprite* spirit);
		void AssignScaling(Sprite* spirit);
		void AssignRotation(Sprite* spirit);
		void AssignColor(Sprite* spirit);
		void AssignTexture(Sprite* spirit);
	protected:
		std::list<std::shared_ptr<Sprite>> _Particles;
		std::vector<Sprite*> _ShowingParticles;
		std::vector <std::shared_ptr<ParticleComponent>> _Children;
		uint32_t _ParticleCount;
		int64_t _LastParticleBeginning;
		Randomizer _Random;
		Vector2  _BasePosition;
		float  _BaseScale;

		std::shared_ptr<ParticleParameter>	_Params;
	};

}