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

void PiplineBase::UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t count)
{
    _VertexBuffer.Update(context, data, count);
}

void PiplineBase::UpdateIndex(ID3D11DeviceContext* context, DrawIndex* data, uint32_t count)
{
    _IndexBuffer.Update(context, data, count);
}

bool PiplineBase::LoadVertexShader(ID3D11Device* device, const std::wstring& csofile)
{
    CComPtr<ID3DBlob> vertexShaderBlob;
    if (FAILED(D3DReadFileToBlob(csofile.c_str(), &vertexShaderBlob)))
    {
        return false;
    }
    if (FAILED(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &_VertexShader)))
    {
        return false;
    }
    auto hr = CreateInputLayout(device, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
    return SUCCEEDED(hr);
}

bool PiplineBase::LoadPixelShader(ID3D11Device* device, const std::wstring& csofile)
{
    CComPtr<ID3DBlob> pixelShaderBlob;
    if (FAILED(D3DReadFileToBlob(csofile.c_str(), &pixelShaderBlob)))
    {
        return false;
    }
    auto rt = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &_PixelShader);
    return SUCCEEDED(rt);
}

bool ColorPipline::Load(ID3D11Device* device)
{
    wchar_t pathBuffer[512];
    GetModuleFileName(0, pathBuffer, 512);
    std::wstring path(pathBuffer);
    auto pos = path.rfind(LR"(\)");
    path = path.substr(0, pos);
    return LoadVertexShader(device, path + LR"(\Color_VS.cso)") && LoadPixelShader(device, path + LR"(\Color_Ps.cso)");
}

HRESULT ColorPipline::CreateInputLayout(ID3D11Device* device, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength)
{
    D3D11_INPUT_ELEMENT_DESC local_layout[] =
    {

        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(VertexXYColor, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, (UINT)offsetof(VertexXYColor, uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",   0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)offsetof(VertexXYColor, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    return device->CreateInputLayout(local_layout, 2, pShaderBytecodeWithInputSignature, BytecodeLength, &_InputLayout);
}
