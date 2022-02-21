#pragma once

#include <windows.h>
#include "D2DRenderEngine.h"
#include "MessageDeclare.h"
#include <memory>

namespace Agate
{

	class Application
	{
	public:
		bool Initialize(HWND wnd, bool fullscreen);
		void Render();
		//void RouteMouseEvent(const tagMouseArg&& arg);
	private:
		std::unique_ptr<D2DRenderEngine>  m_Renderer;
		std::shared_ptr<RectangeVisual> m_Root;

	};
}



