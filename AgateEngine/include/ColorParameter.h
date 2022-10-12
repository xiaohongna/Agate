#pragma once
#include "Brush.h"
#include "MinMaxValue.h"

namespace agate
{
	class ColorParameter
	{
	};

	enum class ColorAnimationType
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
	};

	struct ParticlColorParameter
	{
		ColorAnimationType type;
		union Params
		{
			Color fixed;
			MinMax<Color> random;
			struct
			{
				MinMax<Color> from;
				MinMax<Color> to;
			};
			Params() : fixed{ 0xFFFFFFFF }
			{

			}
		} params;
	};

	struct ColorAnimationParameter
	{
		ColorAnimationType type;
		union Params
		{
			Color fixed;
			struct
			{
				Color from;
				Color to;
			};
			Params() :from{ 0xFFFFFFFF }, to{0xFFFFFFFF}
			{

			}
		} params;
	};
}

