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
    class GraphicContext :public IRenderer
    {
    public:
        GraphicContext() :_Width{},
            _Height{},
            _init{ false },
            _CurrentPipline{},
            _CurrentBlend{ BlendMode::Blend }
        {

        }

        bool CreateDeviceD3D(HWND hWnd);

        //void* LoadTexture(const std::wstring& fileName);

        void BeginDraw() override;

        void SetViewPort(uint32_t width, uint32_t height) override;

        void SetRenderTarget() override;

        void Clear(Color color) override;

        void Draw(const BatchDrawData& data) override;

        void EndDraw(uint32_t sync) override;

        TextureHandle CreateTexture(const BitmapData& data) override;

        void ReleaseTexture(TextureHandle texture) override;

    private:
        void CleanupDeviceD3D();
        void CreateRenderTarget();
        void CleanupRenderTarget();
        void CreateOther();
        void CreateBlendState();
        void CreateSamplerState();
        void SetupRenderState();
        void SetBlend(BlendMode blend);
    private:
        CComPtr<IDXGIFactory>           _Factory;
        CComPtr<IDXGISwapChain>         _SwapChain;
        CComPtr<ID3D11Device>           _Device;
        CComPtr<ID3D11DeviceContext>    _DeviceContext;
        CComPtr<ID3D11RenderTargetView> _MainRenderTargetView;

        VideoBuffer<float, VideoBufferType::Constant>  _VertexConstantBuffer;

        CComPtr<ID3D11RasterizerState>  _RasterizerState;
        CComPtr<ID3D11DepthStencilState>  _DepthStencilState;

        CComPtr<ID3D11BlendState>       _BlendStates[5];
        CComPtr<ID3D11SamplerState>     _Sampler;
        std::unique_ptr<PipelineBase>    _Piplines[2];

        PipelineBase* _CurrentPipline;
        BlendMode   _CurrentBlend;

        uint32_t        _Width;
        uint32_t        _Height;
        bool _init;
    };
}