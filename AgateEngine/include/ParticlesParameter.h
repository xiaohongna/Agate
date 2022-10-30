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
		/// 只在创建时继承
		/// </summary>
		PositionOnCreate = 1,
		/// <summary>
		/// 一直从父粒子获取
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
		/// 粒子数量
		/// </summary>
		uint32_t particleCount; 
		/// <summary>
		/// 一直产生粒子
		/// </summary>
		bool infinite;
		/// <summary>
		/// 初始化延时
		/// </summary>
		MinMax<uint32_t> initialDelay;
		/// <summary>
		/// 粒子生成间隔
		/// </summary>
		MinMax<uint32_t> generateInterval;
		/// <summary>
		/// 粒子生命周期
		/// </summary>
		MinMax<uint64_t> particleLife;
		/// <summary>
		/// 是否和父粒子有绑定关系
		/// </summary>
		bool bindParent;
		/// <summary>
		/// 绑定父节点时属性继承方式
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

