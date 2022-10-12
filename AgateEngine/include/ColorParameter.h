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

