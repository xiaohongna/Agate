#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <map>

#define NameSpace_Render_D11_Header  namespace agate { namespace render { namespace D11 {
#define NameSpace_Render_D11_Tail  } } }

//NameSpace_Render_D11_Header

enum class BufferType
{
    Constant,
    Vertex,
    VertexIndex
};

template <typename T>
class GPUBuffer
{
public:
    GPUBuffer(BufferType bufferType) :_Device{}, _Buffer{}, _Capacity{}, _Size{}
    {
        _Type = bufferType;
    }
    ~GPUBuffer()
    {
        if (_Buffer)
        {
            _Buffer->Release();
            _Buffer = nullptr;
        }
    }

    void Init(ID3D11Device* device, UINT32 capacity = 5000)
    {
        _Device = device;
        setCapacity(capacity);
    }

    UINT32 getCapacity()
    {
        return _Capacity;
    }

    void setCapacity(UINT32 count)
    {
        if (count != _Capacity)
        {
            if (_Buffer)
            {
                _Buffer->Release();
                _Buffer = nullptr;
            }
        }
        _Capacity = count;

        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = count * sizeof(T);
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        switch (_Type)
        {
        case BufferType::Constant:
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            break;
        case BufferType::Vertex:
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            break;
        case BufferType::VertexIndex:
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            break;
        default:
            break;
        }
        auto hr = _Device->CreateBuffer(&desc, NULL, &_Buffer);
    }

    T* Map(ID3D11DeviceContext* context)
    {
        D3D11_MAPPED_SUBRESOURCE subresource;
        if (context->Map(_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource) != S_OK)
            return nullptr;
        return (T*)subresource.pData;
    }

    void UnMap(ID3D11DeviceContext* context)
    {
        context->Unmap(_Buffer, 0);
    }

    void Bind(ID3D11DeviceContext* context, UINT StartSlot = 0)
    {
        if (_Type == BufferType::Vertex)
        {
            unsigned int stride = sizeof(T);
            unsigned int offset = 0;
            context->IASetVertexBuffers(StartSlot, 1, &_Buffer, &stride, &offset);
        }
        else if (_Type == BufferType::Constant)
        {
            context->VSSetConstantBuffers(StartSlot, 1, &_Buffer);
        }
        else
        {
            context->IASetIndexBuffer(_Buffer, sizeof(T) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
        }
    }

    void Update(ID3D11DeviceContext* context, const T* data, uint32_t dataCount)
    {
        auto dest = Map(context);
        memcpy(dest, data, dataCount * sizeof(T));
        UnMap(context);
    }

    void setCount(UINT32 size)
    {
        assert(size <= _Capacity);
        _Size = size;
    }

private:
    ID3D11Buffer* _Buffer;
    UINT32  _Capacity;
    UINT32  _Size;
    ID3D11Device* _Device;
    BufferType _Type;
};

    class GraphicContext
    {
    public:
        GraphicContext();
        ~GraphicContext();
        bool CreateDeviceD3D(HWND hWnd);
        void SetViewSize(int width, int height);
        void Render(DrawerList* list);
        void* LoadTexture(const std::wstring& fileName);
    private:
        void CleanupDeviceD3D();
        void CreateRenderTarget();
        void CleanupRenderTarget();
        void LoadVertexShader();
        void LoadPixelShader();
        void CreateOther();

        void SetupRenderState();
    private:
        IDXGISwapChain* _SwapChain;
        ID3D11Device* _Device;
        ID3D11DeviceContext* _DeviceContext;
        ID3D11RenderTargetView* _MainRenderTargetView;

        IDXGIFactory* _Factory;
        GPUBuffer<DrawVert> _VertexBuffer;
        GPUBuffer<ImDrawIdx> _VertexIndexBuffer;
        ID3D11VertexShader* _VertexShader;
        ID3D11InputLayout* _InputLayout;
        GPUBuffer<float>  _VertexConstantBuffer;
        GPUBuffer<float>  _VertexColorConstantBuffer;
        ID3D11PixelShader* _PixelShader;
        ID3D11SamplerState* _Sampler;
        ID3D11ShaderResourceView* _FontTextureView;
        ID3D11RasterizerState* _RasterizerState;
        ID3D11BlendState* _BlendState;
        ID3D11DepthStencilState* _DepthStencilState;
        std::map<std::wstring, ID3D11ShaderResourceView*> _TextureStorage;
        int                         _VertexBufferSize;
        int                         _IndexBufferSize;

        int _Width;
        int _Height;
        bool _init;
    };

//NameSpace_Render_D11_Tail