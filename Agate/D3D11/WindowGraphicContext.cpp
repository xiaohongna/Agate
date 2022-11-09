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
		auto& device = D3DDevice::ShareInstance();
		device.ResetState();
		device.SetRenderTarget(_MainRenderTargetView, _Width, _Height);
	}

	void WindowGraphicContext::GetViewSize(uint32_t& width, uint32_t& height)
	{
		width = _Width;
		height = _Height;
	}

	void WindowGraphicContext::SetViewSize(uint32_t& width, uint32_t& height)
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

	void WindowGraphicContext::SetRenderTarget(TextureHandle handle)
	{
		if (handle == nullptr)
		{
			D3DDevice::ShareInstance().SetRenderTarget(_MainRenderTargetView, _Width, _Height);
		}
		else
		{
			assert(false);
		}
	}

	void WindowGraphicContext::Clear(Color color)
	{
		D3DDevice::ShareInstance().Clear(color);
	}

	void WindowGraphicContext::Draw(const BatchDrawData& data)
	{

	}

	void WindowGraphicContext::EndDraw(uint32_t sync)
	{
		HRESULT hr = _SwapChain->Present(1, 0);
	}
}