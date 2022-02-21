#pragma once

#include <d2d1_1.h>
#include <d2d1_2.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <D3D11.h>
#include <DXGI1_2.h>
#include <Wincodec.h>
#include <dwrite.h>
#include <map>
#include <dxgi1_3.h>
#include "RenderEngine.h"
#include "visual.h"
#include "ShapeVisual.h"
#include <atlbase.h>

namespace Agate
{
	class D2DRenderEngine
	{
	public:
		D2DRenderEngine(HWND wnd);
		void Render(RectangeVisual* root);
		~D2DRenderEngine();

		std::shared_ptr<RectangeVisual> CreateRectangeVisual();
	private:
		void CreateSwapChain();
		HRESULT CreateD3D11Device(D3D_DRIVER_TYPE driverType, UINT flags);
		bool Resize(int width, int height);
	public:
		HWND					m_window;
		int32_t					m_width;
		int32_t					m_height;

		CComPtr<ID2D1Factory>			m_pD2DFactory;
		CComPtr<ID2D1Factory1>			m_pD2DFactory1;

		CComPtr<ID2D1RenderTarget>		m_pRenderTarget;
		CComPtr<ID2D1DeviceContext>		m_pDeviceContext;

		ID3D11Device*			m_pD3DDevice11;
		CComPtr<ID3D11DeviceContext>	m_pD3DDeviceContext11;
		CComPtr<ID2D1Device>			m_pD2DDevice;
		CComPtr<IDXGISwapChain1>  m_SwapChain;
	};
}

