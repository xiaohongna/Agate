#pragma once
#include <string>
#include "Renderer.h"

namespace agate
{
	struct RenderParameter
	{
		std::wstring filePath;
		BlendMode	blend; //混合模式
		bool antialiasing; //是否启用边缘抗锯齿
		Vector2 size;
		RenderParameter() :
			blend{BlendMode::Blend},
			antialiasing{false}
		{

		}
	};
}