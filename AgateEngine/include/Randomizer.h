#pragma once
#include <cstdint>
#include <stdlib.h>
namespace agate
{
	class Randomizer
	{
	public:
		Randomizer()
		{
			srand(85);
			_Seed = rand();
		}

		void SetSeed(int32_t seed)
		{
			_Seed = seed;
		}

		int32_t GetRandInt()
		{
			constexpr int64_t a = 1103515245;
			constexpr int64_t c = 3124592;
			constexpr int64_t m = 2147483647;

			_Seed = (_Seed * a + c) & m;
			return _Seed % 0x7FFF;
		}
		/// <summary>
		/// 0~1 Ëæ»úÊı
		/// </summary>
		/// <returns></returns>
		float GetRand()
		{
			auto ret = GetRandInt();
			return (float)ret / (float)(0x7FFF - 1);
		}

		float GetRand(float min_, float max_)
		{
			auto dis = max_ - min_;
			if (dis < 0.0001 && dis > -0.0001)
			{
				return max_;
			}
			return GetRand() * dis + min_;
		}
	private:
		int32_t _Seed;
	};
}

