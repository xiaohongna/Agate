#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "Drawable.h"
#include "Renderer.h"
#include "VideoBuffer.h"
#include "Pipeline.h"
#include "Brush.h"

namespace agate
{
	class WindowGraphicContext: private IRenderer
	{
    public:
        WindowGraphicContext(HWND hwnd);

        void OnResize(uint32_t width, uint32_t height);
#pragma region IRenderer
    private:
        void BeginDraw() override;

        void SetRenderTarget() override;

        void Clear(Color color) override;

        void Draw(const BatchDrawData& data) override;

        void EndDraw(uint32_t sync) override;
#pragma endregion
    private:
        CComPtr<IDXGISwapChain>         _SwapChain;
        CComPtr<ID3D11RenderTargetView> _MainRenderTargetView;
        uint32_t        _Width;
        uint32_t        _Height;
	};
}

