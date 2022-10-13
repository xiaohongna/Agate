#pragma once
#include <string>
#include "Renderer.h"

namespace agate
{
	struct RenderParameter
	{
		std::wstring filePath;
		BlendMode	blend; //���ģʽ
		bool antialiasing; //�Ƿ����ñ�Ե�����
		Vector2 size;
		RenderParameter() :
			blend{BlendMode::Blend},
			antialiasing{false}
		{

		}
	};
}