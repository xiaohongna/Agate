#pragma once
#include <cstdint>
#include "MinMaxValue.h"
#include <bitserializer/bit_serializer.h>

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
		/// 粒子生命周期
		/// </summary>
		MinMax<uint64_t> particleLife;

		template <class TArchive>
		void Serialize(TArchive& archive)
		{
			archive << BitSerializer::MakeAutoKeyValue("ParticleCount", particleCount);
			archive << BitSerializer::MakeAutoKeyValue("Infinite", infinite);
			archive << BitSerializer::MakeAutoKeyValue("GenerateInterval", generateInterval);
			archive << BitSerializer::MakeAutoKeyValue("ParticleLife", particleLife);
		}
	};
}

