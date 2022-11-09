#include "pch.h"
#include "D3DDevice.h"

namespace agate
{
	HRESULT D3DDevice::CreateSwapChain(HWND hwnd, IDXGISwapChain** swapChain)
	{
        if (_Device == NULL)
        {
            RECT rt{};
            GetClientRect(hwnd, &rt);
            DXGI_SWAP_CHAIN_DESC sd{};
            sd.BufferCount = 2;
            sd.BufferDesc.Width = rt.right - rt.left;
            sd.BufferDesc.Height = rt.bottom - rt.top;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hwnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            UINT createDeviceFlags = 0;
            createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
            D3D_FEATURE_LEVEL featureLevel;
            const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
            auto hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, swapChain, &_Device, &featureLevel, &_DeviceContext);
            if (SUCCEEDED(hr))
            {

            }
        }
        else
        {
            return E_FAIL;
        }
	}

    HRESULT D3DDevice::CreateRenderTarget(IDXGISwapChain* swapChain, ID3D11RenderTargetView** rootView)
    {
        CComPtr<ID3D11Texture2D> backBuffer;
        if (SUCCEEDED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
        {
            _Device->CreateRenderTargetView(backBuffer, NULL, rootView);
        }
    }

    void D3DDevice::ResetState()
    {
        _DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _VertexConstantBuffer.Bind(_DeviceContext, 0);
        _DeviceContext->GSSetShader(NULL, NULL, 0);
        _DeviceContext->HSSetShader(NULL, NULL, 0);
        _DeviceContext->DSSetShader(NULL, NULL, 0);
        _DeviceContext->CSSetShader(NULL, NULL, 0);
        _DeviceContext->OMSetDepthStencilState(_DepthStencilState, 0);
        _DeviceContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&_Sampler.p);
        _DeviceContext->RSSetState(_RasterizerState);
        SetBlend(BlendMode::Blend);
    }

    void D3DDevice::SetBlend(BlendMode blend)
    {
        constexpr float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
        _DeviceContext->OMSetBlendState(_BlendStates[(int)blend], blend_factor, 0xffffffff);
        _CurrentBlend = blend;
    }

    void D3DDevice::SetRenderTarget(ID3D11RenderTargetView* targetView, uint32_t width, uint32_t height)
    {
        D3D11_VIEWPORT vp{};
        vp.Width = (float)width;
        vp.Height = (float)height;
        vp.MaxDepth = 1.0f;
        _DeviceContext->RSSetViewports(1, &vp);
        const D3D11_RECT r = { (LONG)0, (LONG)0, (LONG)width, (LONG)height};
        _DeviceContext->RSSetScissorRects(1, &r);
        _DeviceContext->OMSetRenderTargets(1, &targetView, nullptr);
    }
	
    const AssetManagerConfig& D3DDevice::GetConfig()
	{
        static AssetManagerConfig g_Config{ 1024 * 1024 * 300, false }; 
        return  g_Config;
	}
	TextureHandle D3DDevice::CreateRenderTarget()
	{
        return nullptr;
	}
	TextureHandle D3DDevice::CreateTexture(uint32_t width, uint32_t height, void* sysMem)
	{
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        CComPtr<ID3D11Texture2D> pTexture;
        D3D11_SUBRESOURCE_DATA subResource { sysMem, desc.Width * 4, 0 };
        auto hr = _Device->CreateTexture2D(&desc, &subResource, &pTexture);
        assert(SUCCEEDED(hr));

        ID3D11ShaderResourceView* resourceView;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        hr = _Device->CreateShaderResourceView(pTexture, &srvDesc, &resourceView);
        assert(SUCCEEDED(hr));
        return resourceView;
	}

	void D3DDevice::ReleaseTexture(TextureHandle handle)
	{
        if (handle)
        {
            auto resourceView = reinterpret_cast<ID3D11ShaderResourceView*>(handle);
            resourceView->Release();
        }
	}
    void D3DDevice::CreateOther()
    {
        {
            D3D11_RASTERIZER_DESC desc{};
            desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME;//
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

        auto colorPPL = std::make_unique<ColorPipline>();
        if (colorPPL->Load(_Device))
        {
            _Piplines[0] = std::move(colorPPL);
        }
        else
        {
            assert(false);
        }

        auto texturePPL = std::make_unique<TextureColorPipline>();
        if (texturePPL->Load(_Device))
        {
            _Piplines[1] = std::move(texturePPL);
        }
        else
        {
            assert(false);
        }
    }
    void D3DDevice::CreateBlendState()
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
    void D3DDevice::CreateSamplerState()
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MipLODBias = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = 0.f;
        desc.MaxLOD = 0.f;
        _Device->CreateSamplerState(&desc, &_Sampler);
    }
    void D3DDevice::SetupRenderState()
    {
    }
}
