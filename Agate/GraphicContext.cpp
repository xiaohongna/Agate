#include "GraphicContext.h"
#include <d3dcompiler.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <DirectXMath.h>

#define Safe_Release(obj) if(obj) { obj->Release(); obj = nullptr;}

struct VERTEX_CONSTANT_BUFFER
{
    float   mvp[4][4];
};

GraphicContext::GraphicContext() :_SwapChain{}, _Device{}, _DeviceContext{}, _MainRenderTargetView{},
_Factory{},
_VertexShader{},
_InputLayout{},
_VertexConstantBuffer{BufferType::Constant},
_VertexColorConstantBuffer{BufferType::Constant},
_PixelShader{},
_Sampler{},
_FontTextureView{},
_RasterizerState{},
_BlendState{},
_DepthStencilState{},
_VertexBufferSize{},
_IndexBufferSize{},
_Width{},
_Height{},
_init{false},
_VertexBuffer { BufferType::Vertex},
_VertexIndexBuffer {BufferType::VertexIndex}
{

}

GraphicContext::~GraphicContext()
{
    Safe_Release(_SwapChain);
    Safe_Release(_Device);
    Safe_Release(_DeviceContext);
    Safe_Release(_MainRenderTargetView);
    Safe_Release(_Factory);
    Safe_Release(_VertexShader);
    Safe_Release(_InputLayout);
    Safe_Release(_PixelShader);
    Safe_Release(_Sampler);
    Safe_Release(_RasterizerState);
    Safe_Release(_BlendState);
    Safe_Release(_DepthStencilState);
    for (auto& item : _TextureStorage)
    {
        item.second->Release();
    }
}

bool GraphicContext::CreateDeviceD3D(HWND hWnd)
{
    if (_init)
    {
        return false;
    }
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
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
    LoadVertexShader();
    LoadPixelShader();
    CreateOther();
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
    float R = width;
    float T = 0;
    float B = height;
    float mvp[4][4] =
    {
        { 2.0f / (R - L),       0.0f,                0.0f,       0.0f },
        { 0.0f,                 2.0f / (T - B),      0.0f,       0.0f },
        { 0.0f,                 0.0f,                0.5f,       0.0f },
        {(R + L) / (L - R),     (T + B) / (B - T),   0.5f,       1.0f },
    };
    auto matrix = DirectX::XMMatrixScaling(2 / (R - L), - 2 / (B - T), 0) * DirectX::XMMatrixTranslation(-1, 1, 0.7);
    memcpy(constant, &(matrix.r[0]), sizeof(matrix));

    _VertexConstantBuffer.UnMap(_DeviceContext);
    if (_MainRenderTargetView)
    {
        _MainRenderTargetView->Release();
        _MainRenderTargetView = nullptr;
    }
    _SwapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    CreateRenderTarget();
}

void GraphicContext::Draw(const BatchDrawData& data)
{
    if (!_init)
    {
        return;
    }
    
    const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    _DeviceContext->OMSetRenderTargets(1, &_MainRenderTargetView, NULL);
    _DeviceContext->ClearRenderTargetView(_MainRenderTargetView, clear_color_with_alpha);
    _DeviceContext->PSSetShaderResources(0, list->GetTextureCount(), (ID3D11ShaderResourceView**)(list->GetAllTexture()));
    _VertexBuffer.Update(_DeviceContext, list->GetVertex(), list->GetVertexCount());
    _VertexIndexBuffer.Update(_DeviceContext, list->GetVertexIndex(), list->GetVertexIndexCount());
    _VertexColorConstantBuffer.Update(_DeviceContext, list->GetColor(), list->GetColorCount());
    
    SetupRenderState();
    _DeviceContext->DrawIndexed(list->GetVertexIndexCount(), 0, 0);
    
}

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


void GraphicContext::Present(uint32_t sync)
{
    HRESULT hr = _SwapChain->Present(1, 0);
}

void GraphicContext::CleanupDeviceD3D()
{
}

void GraphicContext::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer{};
    _SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer)
    {
        _Device->CreateRenderTargetView(pBackBuffer, NULL, &_MainRenderTargetView);
        pBackBuffer->Release();
    }
}

void GraphicContext::CleanupRenderTarget()
{
}

void GraphicContext::LoadVertexShader()
{
    ID3DBlob* vertexShaderBlob{};
    if (FAILED(D3DReadFileToBlob(L"E:\\Agate\\Agate\\x64\\Debug\\VertexShader.cso", &vertexShaderBlob)))
    {
        return;
    }
    if (FAILED(_Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &_VertexShader)))
    {
        vertexShaderBlob->Release();
        return;
    }
    
    D3D11_INPUT_ELEMENT_DESC local_layout[] =
    {
        
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(DrawVert, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, (UINT)offsetof(DrawVert, uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "INDEX",   0, DXGI_FORMAT_R32_UINT, 0, (UINT)offsetof(DrawVert, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    if (_Device->CreateInputLayout(local_layout, 3, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &_InputLayout) != S_OK)
    {
        vertexShaderBlob->Release();
        return;
    }
    vertexShaderBlob->Release();
    _VertexConstantBuffer.Init(_Device, 16);
    _VertexColorConstantBuffer.Init(_Device, 1000);
}

void GraphicContext::LoadPixelShader()
{
    ID3DBlob* pixelShaderBlob{};
    if (FAILED(D3DReadFileToBlob(L"E:\\Agate\\Agate\\x64\\Debug\\Color_PS.cso", &pixelShaderBlob)))
    {
        return;
    }
    if (_Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &_PixelShader) != S_OK)
    {
        pixelShaderBlob->Release();
    }
    pixelShaderBlob->Release();
    
}

void GraphicContext::CreateOther()
{
    //_Device->CreateBuffer()
    _VertexBuffer.Init(_Device);
    _VertexIndexBuffer.Init(_Device);
   
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
        _Device->CreateSamplerState(&desc, &_Sampler);
    }

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
        _Device->CreateBlendState(&desc, &_BlendState);
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
}

void GraphicContext::SetupRenderState()
{
    D3D11_VIEWPORT vp{};
    vp.Width = _Width;
    vp.Height = _Height;
    vp.MaxDepth = 1.0f;
    _DeviceContext->RSSetViewports(1, &vp);
    //×°Åä
    _DeviceContext->IASetInputLayout(_InputLayout);
    _VertexBuffer.Bind(_DeviceContext);
    _VertexIndexBuffer.Bind(_DeviceContext);
    _DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    _DeviceContext->VSSetShader(_VertexShader, NULL, 0);
    _VertexConstantBuffer.Bind(_DeviceContext, 0);
    _VertexColorConstantBuffer.Bind(_DeviceContext, 1);
    _DeviceContext->PSSetShader(_PixelShader, NULL, 0);
    _DeviceContext->PSSetSamplers(0, 1, &_Sampler);
    _DeviceContext->GSSetShader(NULL, NULL, 0);
    _DeviceContext->HSSetShader(NULL, NULL, 0); 
    _DeviceContext->DSSetShader(NULL, NULL, 0); 
    _DeviceContext->CSSetShader(NULL, NULL, 0);

    //ºÏ³É½×¶Î
    const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
    _DeviceContext->OMSetBlendState(_BlendState, blend_factor, 0xffffffff);
    _DeviceContext->OMSetDepthStencilState(_DepthStencilState, 0);
    _DeviceContext->RSSetState(_RasterizerState);

    const D3D11_RECT r = { (LONG)0, (LONG)0, (LONG)_Width, (LONG)_Height };
    _DeviceContext->RSSetScissorRects(1, &r);
}
