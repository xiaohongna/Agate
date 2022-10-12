#pragma once
#include <cstdint>
#include "MinMaxValue.h"

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
		/// �������ɼ��
		/// </summary>
		MinMax<uint64_t> particleLife;

	};
}

