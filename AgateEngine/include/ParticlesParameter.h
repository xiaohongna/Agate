#pragma once
#include <cstdint>
#include "MinMaxValue.h"
#include <bitserializer/bit_serializer.h>

namespace agate
{
	struct ParticleParameter
	{
		/// <summary>
		/// ��������
		/// </summary>
		uint32_t particleCount; 
		/// <summary>
		/// һֱ��������
		/// </summary>
		bool infinite;
		/// <summary>
		/// �������ɼ��
		/// </summary>
		MinMax<uint32_t> generateInterval;
		/// <summary>
		/// ������������
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

