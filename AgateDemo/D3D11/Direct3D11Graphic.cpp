#include "Direct3D11Graphic.h"
#include "WindowGraphicContext.h"
#include "D3DDevice.h"

namespace agate
{
	static D3DDevice g_Device;

	DrawingContext* CreateDrawContext(HWND hwnd)
	{
		auto windowRender = std::make_unique<WindowGraphicContext>(g_Device, hwnd);
		AssetManager::SharedInstance().Initialize(&g_Device);
		DrawingContext* context = new DrawingContext(std::move(windowRender));
		return context;
	}

	void ClearResource()
	{
		AssetManager::SharedInstance().Invalidate();
		g_Device.Dispose();
	}
}