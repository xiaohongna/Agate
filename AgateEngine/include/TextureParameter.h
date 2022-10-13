#pragma once
#include <string>
#include "Vector.h"
#include "Renderer.h"
namespace agate
{
	enum class TextureAnimationType
	{
		/// <summary>
		/// �̶�ֵ
		/// </summary>
		Fixed,
		/// <summary>
		/// ��ֱ����Ч��
		/// </summary>
		VerticalScroll,
		/// <summary>
		/// ˮƽ����
		/// </summary>
		HorizontalScroll,
		/// <summary>
		/// ֡���ж���
		/// </summary>
		FramebyFrame,
		/// <summary>
		/// �������
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
				uint32_t scallSpeed;  //ÿ������ľ���
			};
			struct 
			{
				float stepX;
				float stepY;
				uint32_t lineFrameCount;
				uint32_t frameCount;
				float frameRate;   //֡��
			};
		} params;
	};

	typedef  TextureAnimationParameter ParticleTextureParameter;
}