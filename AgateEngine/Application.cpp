
#include "pch.h"
#include "include/Application.h"
#include "D2DRenderEngine.h"
namespace Agate
{

	bool Application::Initialize(HWND wnd, bool fullscreen)
	{
		m_Renderer = std::make_unique<D2DRenderEngine>(wnd);
		m_Root = m_Renderer->CreateRectangeVisual();
		return false;
	}
	void Application::Render()
	{
		m_Renderer->Render(m_Root.get());
	}
}