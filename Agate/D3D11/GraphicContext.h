#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "Renderer.h"
#include "VideoBuffer.h"
#include "Pipline.h"

#define NameSpace_Render_D11_Header  namespace agate { namespace render { namespace D11 {
#define NameSpace_Render_D11_Tail  } } }

//NameSpace_Render_D11_Header

class GraphicContext :public IRenderer
{
public:
    GraphicContext() :_Width{}, 
        _Height{}, 
        _init{false},
        _CurrentPipline{PiplineType::Color},
        _CurrentBlend{BlendMode::Blend}
    {

    }
    
    bool CreateDeviceD3D(HWND hWnd);

    //void* LoadTexture(const std::wstring& fileName);

    void SetViewPort(uint32_t width, uint32_t height) override;

    void SetRenderTarget() override;

    void Clear(const Vector4& color) override;

    void Draw(const BatchDrawData& data) override;

    void Present(uint32_t sync) override;

private:
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void CreateOther();
    void CreateBlendState();

    void SetupRenderState();
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
    std::unique_ptr<PiplineBase>    _Piplines[2];

    PiplineType _CurrentPipline;
    BlendMode   _CurrentBlend;

    uint32_t        _Width;
    uint32_t        _Height;
    bool _init;
};

//NameSpace_Render_D11_Tail