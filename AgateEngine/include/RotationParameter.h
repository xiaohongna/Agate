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
		/// 固定值
		/// </summary>
		Fixed,
		/// <summary>
		/// 随机值
		/// </summary>
		Random,
		/// <summary>
		/// 从A到B
		/// </summary>
		FromTo,
		/// <summary>
		/// 速度加速度
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

