#pragma once
#include <cstdint>
#include "MinMaxValue.h"
#include "RenderParameter.h"
#include "RotationParameter.h"
#include "ScalingParameter.h"
#include "TextureParameter.h"
#include "TranslateParameter.h"
#include "ColorParameter.h"
namespace agate
{
	enum class InheriteBehavior : uint32_t
	{
		Never = 0,
		/// <summary>
		/// ֻ�ڴ���ʱ�̳�
		/// </summary>
		PositionOnCreate = 1,
		/// <summary>
		/// һֱ�Ӹ����ӻ�ȡ
		/// </summary>
		PositionAlways = 1 << 1,

		ScalingOnCreate = 1 << 2,

		ScalingAlways = 1 << 3,

		RotationOnCreate = 1 << 4,

		RotationAlways = 1 << 5,
	};

	static bool operator& (InheriteBehavior left, InheriteBehavior right)
	{
		return ((uint32_t)left & (uint32_t)right) != 0;
	}

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
		/// ��ʼ����ʱ
		/// </summary>
		MinMax<uint32_t> initialDelay;
		/// <summary>
		/// �������ɼ��
		/// </summary>
		MinMax<uint32_t> generateInterval;
		/// <summary>
		/// ������������
		/// </summary>
		MinMax<uint64_t> particleLife;
		/// <summary>
		/// �Ƿ�͸������а󶨹�ϵ
		/// </summary>
		bool bindParent;
		/// <summary>
		/// �󶨸��ڵ�ʱ���Լ̳з�ʽ
		/// </summary>
		InheriteBehavior  inherite;

		ParticleTranslateParameter translate;
		ParticleScalingParameter  scaling;
		ParticleRotationParameter rotation;
		ParticlColorParameter	color;
		ParticleTextureParameter  texture;
		RenderParameter		render;
	};
}

