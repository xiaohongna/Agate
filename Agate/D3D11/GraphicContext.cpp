
#include "pch.h"
#include "GraphicContext.h"
#include <d3dcompiler.h>
#define STB_IMAGE_IMPLEMENTATION
//#include "stb/stb_image.h"
#include <DirectXMath.h>
#include <filesystem>

#define Safe_Release(obj) if(obj) { obj->Release(); obj = nullptr;}

struct VERTEX_CONSTANT_BUFFER
{
    float   mvp[4][4];
};

bool GraphicContext::CreateDeviceD3D(HWND hWnd)
{
    if (_init)
    {
        return false;
    }
    RECT rt{};
    GetClientRect(hWnd, &rt);
    _Width = rt.right - rt.left;
    _Height = rt.bottom - rt.top;

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = _Width;
    sd.BufferDesc.Height = _Height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &_SwapChain, &_Device, &featureLevel, &_DeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    CreateOther();
    CreateBlendState();
    _init = true;
    return true;
}

void GraphicContext::SetViewPort(uint32_t width, uint32_t height)
{
    if (!_init)
    {
        return;
    }
    if (width == _Width && height == _Height)
    {
        return;
    }
    _Width = width;
    _Height = height;
    auto constant =  _VertexConstantBuffer.Map(_DeviceContext);

    float L = 0;
    float R = (float)width;
    float T = 0;
    float B = (float)height;
    float mvp[4][4] =
    {
        { 2.0f / (R - L),       0.0f,                0.0f,       0.0f },
        { 0.0f,                 2.0f / (T - B),      0.0f,       0.0f },
        { 0.0f,                 0.0f,                0.5f,       0.0f },
        {(R + L) / (L - R),     (T + B) / (B - T),   0.5f,       1.0f },
    };
    auto matrix = DirectX::XMMatrixScaling(2 / (R - L), - 2 / (B - T), 0) * DirectX::XMMatrixTranslation(-1, 1, 0.7f);
    memcpy(constant, &(matrix.r[0]), sizeof(matrix));

    _VertexConstantBuffer.UnMap(_DeviceContext);
    _MainRenderTargetView.Release();
    _SwapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    CreateRenderTarget();

}

void GraphicContext::SetRenderTarget()
{
    _DeviceContext->OMSetRenderTargets(1, &_MainRenderTargetView, NULL);
}

void GraphicContext::Clear(const Vector4& color)
{
    _DeviceContext->ClearRenderTargetView(_MainRenderTargetView, (float*)&color);
}

void GraphicContext::Draw(const BatchDrawData& data)
{
    if (!_init)
    {
        return;
    }

    //_DeviceContext->PSSetShaderResources(0, list->GetTextureCount(), (ID3D11ShaderResourceView**)(list->GetAllTexture()));
   // _VertexBuffer.Update(_DeviceContext, (VertexXYColor*)data.vertexData, data.vertexCount);
    //_VertexIndexBuffer.Update(_DeviceContext, data.indexData, data.indexCount);
    //_VertexColorConstantBuffer.Update(_DeviceContext, list->GetColor(), list->GetColorCount());
    
    SetupRenderState();
    for (auto& cmd : data.commands)
    {
        _DeviceContext->DrawIndexed(cmd.indexCount, cmd.startIndexLocation, 0);
    }
    
}
/*
void* GraphicContext::LoadTexture(const std::wstring& fileName)
{
    auto found = _TextureStorage.find(fileName);

    if (found != _TextureStorage.end())
    {
        return found->second;
    }
    FILE* f = nullptr;
    if (0 != _wfopen_s(&f, fileName.c_str(), L"rb"))
    {
        return nullptr;
    }
    int channel = 0;
    int w{}, h{};
    auto bits = stbi_load_from_file(f, &w, &h, &channel, 4);
    if (bits == nullptr)
    {
        return nullptr;
    }

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture{};
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = bits;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    auto hr = _Device->CreateTexture2D(&desc, &subResource, &pTexture);
    assert(SUCCEEDED(hr));
    ID3D11ShaderResourceView* resourceView{};
    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = _Device->CreateShaderResourceView(pTexture, &srvDesc, &resourceView);
    assert(SUCCEEDED(hr));
    pTexture->Release();
    delete bits;
    _TextureStorage[fileName] = resourceView;
    return resourceView;
}
*/


void GraphicContext::Present(uint32_t sync)
{
    HRESULT hr = _SwapChain->Present(1, 0);
}

void GraphicContext::CleanupDeviceD3D()
{
}

void GraphicContext::CreateRenderTarget()
{
    CComPtr<ID3D11Texture2D> backBuffer;
    if(SUCCEEDED(_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
    {
        _Device->CreateRenderTargetView(backBuffer, NULL, &_MainRenderTargetView);
    }
}

void GraphicContext::CleanupRenderTarget()
{

}

void GraphicContext::CreateOther()
{
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.MinLOD = 0.f;
        desc.MaxLOD = 0.f;
       // _Device->CreateSamplerState(&desc, &_Sampler);
    }
    
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.ScissorEnable = true;
        desc.DepthClipEnable = true;
        _Device->CreateRasterizerState(&desc, &_RasterizerState);
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        desc.StencilEnable = false;
        desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace = desc.FrontFace;
        _Device->CreateDepthStencilState(&desc, &_DepthStencilState);
    }
    _VertexConstantBuffer.Init(_Device, 16);
}

void GraphicContext::CreateBlendState()
{
    D3D11_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    _Device->CreateBlendState(&desc, &_BlendStates[0]);  //Blend

    desc.RenderTarget[0].BlendEnable = false;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    _Device->CreateBlendState(&desc, &_BlendStates[1]);  //Blend

    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    _Device->CreateBlendState(&desc, &_BlendStates[2]);  //Additive

    desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
    _Device->CreateBlendState(&desc, &_BlendStates[3]);  //Additive

    desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    _Device->CreateBlendState(&desc, &_BlendStates[4]);  //Multiply
}

void GraphicContext::SetupRenderState()
{
    D3D11_VIEWPORT vp{};
    vp.Width = (float)_Width;
    vp.Height = (float)_Height;
    vp.MaxDepth = 1.0f;
    _DeviceContext->RSSetViewports(1, &vp);
    //×°Åä

    _DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _VertexConstantBuffer.Bind(_DeviceContext, 0);
    _DeviceContext->GSSetShader(NULL, NULL, 0);
    _DeviceContext->HSSetShader(NULL, NULL, 0); 
    _DeviceContext->DSSetShader(NULL, NULL, 0); 
    _DeviceContext->CSSetShader(NULL, NULL, 0);

    //ºÏ³É½×¶Î
    const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
    _DeviceContext->OMSetBlendState(_BlendStates[1], blend_factor, 0xffffffff);
    _DeviceContext->OMSetDepthStencilState(_DepthStencilState, 0);
    _DeviceContext->RSSetState(_RasterizerState);

    const D3D11_RECT r = { (LONG)0, (LONG)0, (LONG)_Width, (LONG)_Height };
    _DeviceContext->RSSetScissorRects(1, &r);
}
