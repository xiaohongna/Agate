#pragma once
#include "Vector.h"
#include "MinMaxValue.h"
namespace agate
{
	class RotationParameter
	{
	};

	enum class RotationAnimationType
	{
		/// <summary>
		/// �̶�ֵ
		/// </summary>
		Fixed,
		/// <summary>
		/// ���ֵ
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
	struct ParticleRotationParameter
	{
		RotationAnimationType type;
		union Params
		{
			float fixed;
			MinMax<float> random;
			struct
			{
				MinMax<float> from;
				MinMax<float> to;
			};
			struct
			{
				MinMax<float> base;
				MinMax<float> velocity;
				MinMax<float> acceleration;
			};
			Params() :base{}, velocity{}, acceleration{}
			{

			};
		} params;
		Vector2  center;
	};

	struct RotationAnimationParameter
	{
		RotationAnimationType type;
		union 
		{
			float fixed;
			struct 
			{
				float from;
				float to;
			};
			struct 
			{
				float base;
				float velocity;
				float acceleration;
			};
		} params;
		Vector2  center;
	};
}

