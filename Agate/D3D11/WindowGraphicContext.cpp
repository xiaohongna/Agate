#include "pch.h"
#include "WindowGraphicContext.h"
#include "D3DDevice.h"
namespace agate
{
	WindowGraphicContext::WindowGraphicContext(HWND hwnd)
	{
		D3DDevice::ShareInstance().CreateSwapChain(hwnd, &_SwapChain);
		if (_SwapChain)
		{
			DXGI_SWAP_CHAIN_DESC desc{};
			_SwapChain->GetDesc(&desc);
			_Width = desc.BufferDesc.Width;
			_Height = desc.BufferDesc.Height;

		}
	}
	void WindowGraphicContext::OnResize(uint32_t width, uint32_t height)
	{
		if (width == _Width && height == _Height)
		{
			return;
		}
		_Width = width;
		_Height = height;
		_MainRenderTargetView.Release();
		_SwapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		D3DDevice::ShareInstance().CreateRenderTarget(_SwapChain, &_MainRenderTargetView);
	}

	void WindowGraphicContext::BeginDraw()
	{

	}

	void WindowGraphicContext::SetRenderTarget()
	{
	}
	void WindowGraphicContext::Clear(Color color)
	{
	}
	void WindowGraphicContext::Draw(const BatchDrawData& data)
	{
	}
	void WindowGraphicContext::EndDraw(uint32_t sync)
	{
	}
}