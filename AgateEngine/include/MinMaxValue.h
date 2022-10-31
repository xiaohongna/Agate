#pragma once
#include "Brush.h"
#include "Vector.h"
#include "Randomizer.h"
#include <bitserializer/bit_serializer.h>

namespace agate
{
	template<typename T>
	struct MinMax
	{
		T min;
		T max;

		T Random(Randomizer& random)
		{
			return min;
		}

	};

	template<>
	struct MinMax<uint32_t>
	{
		uint32_t min;
		uint32_t max;

		uint32_t Random(Randomizer& random)
		{
			if (max == min)
			{
				return max;
			}
			return uint32_t(random.GetRand() * (max - min)) + min;;
		}
	};

	template<>
	struct MinMax<int64_t>
	{
		int64_t min;
		int64_t max;

		int64_t Random(Randomizer& random)
		{
			if (max == min)
			{
				return max;
			}
			return int64_t(random.GetRand() * (max - min)) + min;;
		}

	};

	template<>
	struct MinMax<float>
	{
		float min;
		float max;

		float Random(Randomizer& random)
		{
			return random.GetRand(min, max);
		}
		
	};

	template<>
	struct MinMax<Vector2>
	{
		Vector2 min;
		Vector2 max;

		Vector2 Random(Randomizer& random)
		{
			return { random.GetRand(min.x, max.x), random.GetRand(min.y, max.y) };
		}

	};

	template<>
	struct MinMax<Color>
	{
		Color min;
		Color max;

		Color Random(Randomizer& random)
		{
			return { 
				(uint8_t)(random.GetRand(min.red, max.red) + 0.5f), 
				(uint8_t)(random.GetRand(min.green, max.green) + 0.5f),
				(uint8_t)(random.GetRand(min.blue, max.blue) + 0.5f),
				(uint8_t)(random.GetRand(min.alpha, max.alpha) + 0.5f)
			};
		}
	};

}