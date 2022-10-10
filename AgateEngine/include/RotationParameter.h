#pragma once
#include "Vector.h"
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
		/// 从A到B
		/// </summary>
		FromTo,
		/// <summary>
		/// 速度加速度
		/// </summary>
		PVA,
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
		} Params;
		Vector2  pointer;
	};
}

