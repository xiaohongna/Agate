#pragma once
#include <string>
#include "Vector.h"
#include "Renderer.h"
namespace agate
{
	enum class TextureAnimationType
	{
		/// <summary>
		/// 固定值
		/// </summary>
		Fixed,
		/// <summary>
		/// 垂直滚动效果
		/// </summary>
		VerticalScroll,
		/// <summary>
		/// 水平滚动
		/// </summary>
		HorizontalScroll,
		/// <summary>
		/// 帧序列动画
		/// </summary>
		FramebyFrame,
		/// <summary>
		/// 随机区域
		/// </summary>
		FrameRandom,
	};

	struct TextureAnimationParameter
	{
		TextureAnimationType type;
		Vector4 UVFrame;
		union Parame
		{
			struct 
			{
				uint32_t scallSpeed;  //每秒滚动的距离
			};
			struct 
			{
				float stepX;
				float stepY;
				uint32_t lineFrameCount;
				uint32_t frameCount;
				float frameRate;   //帧率
			};
		} params;
	};

	typedef  TextureAnimationParameter ParticleTextureParameter;
}