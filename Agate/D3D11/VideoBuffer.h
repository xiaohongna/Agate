#pragma once

#include <atlbase.h>
#include <stdint.h>
#include <d3d11.h>

namespace agate
{

    enum class VideoBufferType
    {
        Vertex,
        VertexIndex,
        Constant,
    };

    template <typename T, VideoBufferType bufferType = VideoBufferType::Vertex>
    class VideoBuffer
    {
    public:
        VideoBuffer() :_Device{}, _Capacity{}, _Size{}
        {
        }

        void Init(ID3D11Device* device, UINT32 capacity = 5000)
        {
            _Device = device;
            setCapacity(capacity);
        }

        uint32_t getCapacity()
        {
            return _Capacity;
        }

        void setCapacity(UINT32 count)
        {
            if (count != _Capacity)
            {
                _Buffer = nullptr;
            }
            _Capacity = count;

            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.ByteWidth = count * sizeof(T);
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            switch (bufferType)
            {
            case VideoBufferType::Constant:
                desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                break;
            case VideoBufferType::Vertex:
                desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                break;
            case VideoBufferType::VertexIndex:
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
            if constexpr (bufferType == VideoBufferType::Vertex)
            {
                unsigned int stride = sizeof(T);
                unsigned int offset = 0;
                context->IASetVertexBuffers(StartSlot, 1, &_Buffer.p, &stride, &offset);
            }
            else if constexpr (bufferType == VideoBufferType::Constant)
            {
                context->VSSetConstantBuffers(StartSlot, 1, &_Buffer.p);
            }
            else
            {
                context->IASetIndexBuffer(_Buffer.p, sizeof(T) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
            }
        }

        void Update(ID3D11DeviceContext* context, const T* data, uint32_t dataCount)
        {
            uint32_t dataSize = dataCount * sizeof(T);
            if (dataSize > _Capacity)
            {
                setCapacity(dataSize + sizeof(T) * 1024);
            }
            auto dest = Map(context);
            memcpy(dest, data, dataSize);
            UnMap(context);
        }

        void setCount(UINT32 size)
        {
            assert(size <= _Capacity);
            _Size = size;
        }

    private:
        CComPtr<ID3D11Buffer> _Buffer;
        uint32_t  _Capacity;
        uint32_t  _Size;
        ID3D11Device* _Device;
    };
}