#include "pch.h"
#include "WindowGraphicContext.h"
#include "D3DDevice.h"
namespace agate
{
	WindowGraphicContext::WindowGraphicContext(D3DDevice& device, HWND hwnd) :_Device(device), _Width{}, _Height{}
	{
		_Device.CreateSwapChain(hwnd, &_SwapChain);
		if (_SwapChain)
		{
			DXGI_SWAP_CHAIN_DESC desc{};
			_SwapChain->GetDesc(&desc);
			OnResize(desc.BufferDesc.Width, desc.BufferDesc.Height);
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
		_Device.CreateRenderTarget(_SwapChain, &_MainRenderTargetView);
	}

	void WindowGraphicContext::BeginDraw()
	{
		_Device.ResetState();
		_Device.SetRenderTarget(_MainRenderTargetView, _Width, _Height);
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
		_Device.CreateRenderTarget(_SwapChain, &_MainRenderTargetView);
	}

	void WindowGraphicContext::SetRenderTarget(TextureHandle handle)
	{
		if (handle == nullptr)
		{
			_Device.SetRenderTarget(_MainRenderTargetView, _Width, _Height);
		}
		else
		{
			auto texture = static_cast<D3DTexture*>(handle);
			_Device.SetRenderTarget(texture->renderTargetView, texture->width, texture->height);
		}
	}

	void WindowGraphicContext::Clear(Color color)
	{
		_Device.Clear(color);
	}

	void WindowGraphicContext::Draw(const BatchDrawData& data)
	{
		_Device.Draw(data);
	}

	void WindowGraphicContext::EndDraw(uint32_t sync)
	{
		HRESULT hr = _SwapChain->Present(1, 0);
	}
}