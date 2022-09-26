#include "pch.h"
#include "Pipline.h"
#include <d3dcompiler.h>


void PiplineBase::Active(ID3D11DeviceContext* context)
{
    _VertexBuffer.Bind(context);
    _IndexBuffer.Bind(context);
    context->IASetInputLayout(_InputLayout);
    context->VSSetShader(_VertexShader, NULL, 0);
    context->PSSetShader(_PixelShader, NULL, 0);
}

void PiplineBase::UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t size)
{
    _VertexBuffer.Update(context, data, size);
}

void PiplineBase::UpdateIndex(ID3D11DeviceContext* context, DrawIndex* data, uint32_t size)
{
    _IndexBuffer.Update(context, data, size);
}

bool PiplineBase::LoadVertexShader(ID3D11Device* device, const std::wstring& csofile)
{
    /*
    wchar_t pathBuffer[512];
    GetModuleFileName(0, pathBuffer, 512);
    std::wstring path(pathBuffer);
    auto pos = path.rfind(LR"(\)");
    path = path.substr(0, pos);
    path = path + LR"(\Color_VS.cso)";
    */
    CComPtr<ID3DBlob> vertexShaderBlob;
    if (FAILED(D3DReadFileToBlob(csofile.c_str(), &vertexShaderBlob)))
    {
        return false;
    }
    if (FAILED(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &_VertexShader)))
    {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC local_layout[] =
    {

        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(VertexXYColor, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, (UINT)offsetof(VertexXYColor, uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",   0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)offsetof(VertexXYColor, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    auto hr = device->CreateInputLayout(local_layout, 2, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &_InputLayout);
    return SUCCEEDED(hr);
}

bool PiplineBase::LoadPixelShader(ID3D11Device* device, const std::wstring& csofile)
{/*
    wchar_t pathBuffer[512];
    GetModuleFileName(0, pathBuffer, 512);
    std::wstring path(pathBuffer);
    auto pos = path.rfind(LR"(\)");
    path = path.substr(0, pos);
    path = path + LR"(\Color_PS.cso)";
    */
    CComPtr<ID3DBlob> pixelShaderBlob;
    if (FAILED(D3DReadFileToBlob(csofile.c_str(), &pixelShaderBlob)))
    {
        return false;
    }
    auto rt = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &_PixelShader);
    return SUCCEEDED(rt);
}
