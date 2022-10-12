#pragma once
#include "Vector.h"
#include "MinMaxValue.h"
namespace agate
{
	class ScalingParameter
	{
	};

	enum class ScalingAnimationType
	{
		Fixed,
		/// <summary>
		/// �ȱȹ̶�ֵ
		/// </summary>
		UniformRandom,
		/// <summary>
		/// �ȱ�
		/// </summary>
		UniformFromTo,

		UniformPVA,
		/// <summary>
		/// �̶�ֵ
		/// </summary>
		Random,
		/// <summary>
		/// ��A��B
		/// </summary>
		FromTo,
		/// <summary>
		/// �ٶȼ��ٶ�
		/// </summary>
		PVA,
	};

	struct ParticleScalingParameter
	{
		ScalingAnimationType type;
		union Params
		{
			Vector2	fixed;
			MinMax<Vector2> random;
			struct
			{
				MinMax<Vector2> from;
				MinMax<Vector2> to;
			};
			struct
			{
				MinMax<Vector2> base;
				MinMax<Vector2> velocity;
				MinMax<Vector2> acceleration;
			};
			MinMax<float> uniformRandom;
			struct
			{
				MinMax<float> uniformFrom;
				MinMax<float> uniformTo;
			};
			struct
			{
				MinMax<float> uniformBase;
				MinMax<float> uniformVelocity;
				MinMax<float> uniformAcceleration;
			};
			Params() : base{  }, velocity{ }, acceleration{ }
			{

			}
		} params;
	};
	struct ScalingAnimationParameter
	{
		ScalingAnimationType type;
		union Params
		{
			Vector2 fixed;
			struct
			{
				Vector2 from;
				Vector2 to;
			};
			struct
			{
				Vector2 base;
				Vector2 velocity;
				Vector2 acceleration;
			};
			
			struct 
			{
				float uniformFrom;
				float uniformTo;
			};
			struct 
			{
				float uniformBase;
				float uniformVelocity;
				float uniformAcceleration;
			};

			Params(): base{1.0f, 1.0f}, velocity{0.0f, 0.0f}, acceleration{0.0f, 0.0f}
			{
				
			}
		} params;
	};
}
