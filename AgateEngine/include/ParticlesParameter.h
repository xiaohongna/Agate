#pragma once
#include <cstdint>
#include "MinMaxValue.h"

namespace agate
{
	struct ParticleParameter
	{
		/// <summary>
		/// 粒子数量
		/// </summary>
		uint32_t particleCount; 
		/// <summary>
		/// 一直产生粒子
		/// </summary>
		bool infinite;
		/// <summary>
		/// 粒子生成间隔
		/// </summary>
		MinMax<uint32_t> generateInterval;
		/// <summary>
		/// 粒子生成间隔
		/// </summary>
		MinMax<uint64_t> particleLife;

	};
}

