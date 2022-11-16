#include "pch.h"
#include "D3DDevice.h"
#include <DirectXMath.h>
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
            assert(SUCCEEDED(hr));
            if (SUCCEEDED(hr))
            {
                hr = CreateOther();
                assert(SUCCEEDED(hr));
                hr = CreateBlendState();
                assert(SUCCEEDED(hr));
                hr = CreateSamplerState();
                assert(SUCCEEDED(hr));
            }
            return hr;
        }
        else
        {
            return E_FAIL;
        }
	}

    HRESULT D3DDevice::CreateRenderTarget(IDXGISwapChain* swapChain, ID3D11RenderTargetView** rootView)
    {
        CComPtr<ID3D11Texture2D> backBuffer;
        HRESULT hr = E_FAIL;
        if (SUCCEEDED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
        {
           hr = _Device->CreateRenderTargetView(backBuffer, NULL, rootView);
        }
        assert(SUCCEEDED(hr));
        return hr;
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
        if (_CurRenderTarget == targetView)
        {
            return;
        }
        _CurRenderTarget = targetView;  //不用增加引用
        auto matrix = DirectX::XMMatrixScaling(2.f / width, -2.f / height, 0) * DirectX::XMMatrixTranslation(-1, 1, 0.7f);
        _VertexConstantBuffer.Update(_DeviceContext, (float*) & (matrix.r[0]), 16);
        D3D11_VIEWPORT vp{};
        vp.Width = (float)width;
        vp.Height = (float)height;
        vp.MaxDepth = 1.0f;
        _DeviceContext->RSSetViewports(1, &vp);
        const D3D11_RECT r = { (LONG)0, (LONG)0, (LONG)width, (LONG)height};
        _DeviceContext->RSSetScissorRects(1, &r);
        _DeviceContext->OMSetRenderTargets(1, &targetView, nullptr);
    }

    void D3DDevice::Clear(Color color)
    {
        float fColor[4]{ color.red / 255.0f, color.green / 255.0f, color.blue / 255.0f, color.alpha / 255.0f };
        _DeviceContext->ClearRenderTargetView(_CurRenderTarget, fColor);
    }

    void D3DDevice::Draw(const BatchDrawData& data)
    {
        if (_CurrentPipline == nullptr || _CurrentPipline->GetType() != data.pipline)
        {
            _CurrentPipline = _Piplines[(int)data.pipline].get();
            _CurrentPipline->Active(_DeviceContext);
        }
        _CurrentPipline->UpdateVertex(_DeviceContext, data.vertexData, data.vertexCount);
        _CurrentPipline->UpdateIndex(_DeviceContext, data.indexData, data.indexCount);
        for (auto const& cmd : data.commands)
        {
            if (cmd.blend != _CurrentBlend)
            {
                SetBlend(cmd.blend);
            }
            ID3D11ShaderResourceView* textures[MaxTextureCount]{};
            ID3D11SamplerState* samplers[MaxTextureCount]{};
            uint32_t textureCount = 0;
            for (uint32_t i = 0; i < MaxTextureCount; i++)
            {
                if (cmd.texture[i] != nullptr)
                {
                    textures[i] = static_cast<D3DTexture*>(cmd.texture[i])->resourceView;
                    samplers[i] = _Samplers[(UINT32)cmd.samplers[i]];
                    textureCount++;
                }
            }
            _DeviceContext->PSSetShaderResources(0, textureCount, textures);
            _DeviceContext->PSSetSamplers(0, textureCount, samplers);
            if (cmd.clipHeight != 0 && cmd.clipWidth != 0)
            {
                const D3D11_RECT r = { cmd.clipX, cmd.clipY, cmd.clipX + cmd.clipWidth, cmd.clipY + cmd.clipHeight };
                _DeviceContext->RSSetScissorRects(1, &r);
            }
            _DeviceContext->DrawIndexed(cmd.indexCount, cmd.startIndexLocation, 0);
        }
    }

    void D3DDevice::Dispose()
    {
        
        CComPtr<ID3D11Debug> debug;
        _Device->QueryInterface(&debug);
        _Factory.Release();
        _Device.Release();
        _DeviceContext.Release();
        _VertexConstantBuffer.Dispose();
        _RasterizerState.Release();
        _DepthStencilState.Release();
        for (size_t i = 0; i < 5; i++)
        {
            _BlendStates[i].Release();
        }
        for (int i = 0; i < MaxTextureCount; i++)
        {
            _Samplers[i].Release();
        }
        _Piplines[0]->Dispose();
        _Piplines[1]->Dispose();
        if (debug)
        {
            debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        }
    }
	
    const AssetManagerConfig& D3DDevice::GetConfig()
	{
        static AssetManagerConfig g_Config{ 1024 * 1024 * 300, false }; 
        return  g_Config;
	}

    TextureHandle D3DDevice::CreateRenderTarget(uint32_t width, uint32_t height)
    {
        auto result = new D3DTexture();
        result->width = width;
        result->height = height;
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        desc.CPUAccessFlags = 0;

        //D3D11_SUBRESOURCE_DATA subResource{ nullptr, desc.Width * 4, 0 };
        auto hr = _Device->CreateTexture2D(&desc, nullptr, &result->resource);
        assert(SUCCEEDED(hr));
        hr = _Device->CreateRenderTargetView(result->resource, NULL, &result->renderTargetView);
        assert(SUCCEEDED(hr));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        hr = _Device->CreateShaderResourceView(result->resource, &srvDesc, &result->resourceView);
        assert(SUCCEEDED(hr));
        return result;
    }

	TextureHandle D3DDevice::CreateTexture(uint32_t width, uint32_t height, void* sysMem)
	{
        auto result = new D3DTexture();
        result->width = width;
        result->height = height;
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

        D3D11_SUBRESOURCE_DATA subResource { sysMem, desc.Width * 4, 0 };
        auto hr = _Device->CreateTexture2D(&desc, &subResource, &result->resource);
        assert(SUCCEEDED(hr));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        hr = _Device->CreateShaderResourceView(result->resource, &srvDesc, &result->resourceView);
        assert(SUCCEEDED(hr));
        return result;
	}

	void D3DDevice::ReleaseTexture(TextureHandle handle)
	{
        if (handle)
        {
            auto texture = reinterpret_cast<D3DTexture*>(handle);
            delete texture;
        }
	}
    HRESULT D3DDevice::CreateOther()
    {
        HRESULT hr = E_FAIL;
        {
            D3D11_RASTERIZER_DESC desc{};
            desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME;//
            desc.CullMode = D3D11_CULL_NONE;
            desc.ScissorEnable = true;
            desc.DepthClipEnable = true;
            hr = _Device->CreateRasterizerState(&desc, &_RasterizerState);
        }
        assert(SUCCEEDED(hr));
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
            hr = _Device->CreateDepthStencilState(&desc, &_DepthStencilState);
        }
        assert(SUCCEEDED(hr));
        hr = _VertexConstantBuffer.Init(_Device, 16);
        assert(SUCCEEDED(hr));
        auto colorPPL = std::make_unique<ColorPipline>();
        if (colorPPL->Load(_Device))
        {
            _Piplines[0] = std::move(colorPPL);
        }
        else
        {
            hr = E_FAIL;
            assert(false);
        }

        auto texturePPL = std::make_unique<TextureColorPipline>();
        if (texturePPL->Load(_Device))
        {
            _Piplines[1] = std::move(texturePPL);
        }
        else
        {
            hr = E_FAIL;
            assert(false);
        }
        return hr;
    }
    HRESULT D3DDevice::CreateBlendState()
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
        auto hr = _Device->CreateBlendState(&desc, &_BlendStates[0]);  //Blend
        assert(SUCCEEDED(hr));
        desc.RenderTarget[0].BlendEnable = false;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        hr = _Device->CreateBlendState(&desc, &_BlendStates[1]);  //Blend
        assert(SUCCEEDED(hr));
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        hr = _Device->CreateBlendState(&desc, &_BlendStates[2]);  //Additive
        assert(SUCCEEDED(hr));
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
        hr = _Device->CreateBlendState(&desc, &_BlendStates[3]);  //Additive
        assert(SUCCEEDED(hr));
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        hr =_Device->CreateBlendState(&desc, &_BlendStates[4]);  //Multiply
        assert(SUCCEEDED(hr));
        return hr;
    }
    HRESULT D3DDevice::CreateSamplerState()
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.MipLODBias = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = 0.f;
        desc.MaxLOD = 0.f;
        //SamplerMode::PointClamp
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        auto hr = _Device->CreateSamplerState(&desc, &_Samplers[0]);
        assert(SUCCEEDED(hr));
        //SamplerMode::PointWarp
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        hr = _Device->CreateSamplerState(&desc, &_Samplers[1]);
        assert(SUCCEEDED(hr));

        //SamplerMode::LinearClamp
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        hr = _Device->CreateSamplerState(&desc, &_Samplers[2]);
        assert(SUCCEEDED(hr));

        //SamplerMode::LinearWarp
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        hr = _Device->CreateSamplerState(&desc, &_Samplers[3]);
        assert(SUCCEEDED(hr));

        //SamplerMode::AnisotropicClamp
        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        hr = _Device->CreateSamplerState(&desc, &_Samplers[4]);
        assert(SUCCEEDED(hr));

        //SamplerMode::AnisotropicWarp
        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        hr = _Device->CreateSamplerState(&desc, &_Samplers[5]);
        assert(SUCCEEDED(hr));
        return hr;
    }
}
