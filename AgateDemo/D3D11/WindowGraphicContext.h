#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "VideoBuffer.h"
#include "Pipeline.h"
#include "D3DDevice.h"
namespace agate
{
	class WindowGraphicContext: public IRendererDelegate
	{
    public:
        WindowGraphicContext(D3DDevice& device, HWND hwnd);

        void OnResize(uint32_t width, uint32_t height);
#pragma region IRenderer
    
        void BeginDraw() override;

        void GetViewSize(uint32_t& width, uint32_t& height) override;

        void SetViewSize(uint32_t& width, uint32_t& height) override;

        void SetRenderTarget(TextureHandle handle) override;

        void Clear(Color color) override;

        void Draw(const BatchDrawData& data) override;

        void EndDraw(uint32_t sync) override;
#pragma endregion
    private:
        CComPtr<IDXGISwapChain>         _SwapChain;
        CComPtr<ID3D11RenderTargetView> _MainRenderTargetView;
        uint32_t        _Width;
        uint32_t        _Height;
        D3DDevice& _Device;
	};
}

