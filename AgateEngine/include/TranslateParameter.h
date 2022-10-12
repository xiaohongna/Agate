#pragma once
#include "Vector.h"
#include "MinMaxValue.h"
namespace agate
{
	class TranslateParameter
	{
	};

	enum class TranslateAnimationType
	{
		/// <summary>
		/// �̶�ֵ
		/// </summary>
		Fixed,
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
		/// <summary>
		/// ָ�������ٶȼ��ٶ�
		/// </summary>
		DirectionPVA,
	};

	struct ParticleTranslateParameter
	{
		TranslateAnimationType type;

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
			struct
			{
				MinMax<Vector2> base;
				MinMax<float> direction;
				MinMax<float> dir_velocity;
				MinMax<float> dir_acceleration;
			};
			Params() : base{}, velocity{}, acceleration{}
			{

			}
		} params;
	};

	struct TranslateAnimationParameter
	{
		TranslateAnimationType type;
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
			Params() : base{ 0.0f, 0.0f }, velocity{ 0.0f, 0.0f }, acceleration{ 0.0f, 0.0f }
			{

			}
		} params;
	};
}