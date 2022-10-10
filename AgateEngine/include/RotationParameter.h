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
		/// �̶�ֵ
		/// </summary>
		Fixed,
		/// <summary>
		/// ��A��B
		/// </summary>
		FromTo,
		/// <summary>
		/// �ٶȼ��ٶ�
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

