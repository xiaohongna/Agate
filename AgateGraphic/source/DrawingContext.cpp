#include "DrawingContext.h"

constexpr  uint32_t Max_IndexBuffer_Count = 1024 * 20;

constexpr  uint32_t Max_VertexBuffer_Count = 1024 * 10;

DrawingContext::DrawingContext():_Pipline{PiplineType::Color}, 
_BlendMode{BlendMode::Blend},
_IndexCount{},
_VertextCount{},
_ViewWidth{1024},
_ViewHeight{768}

{
	_VertextBuffer.Alloc<char>(Max_VertexBuffer_Count * sizeof(VertexXYUVColor));
	_IndexBuffer.Alloc<DrawIndex>(Max_IndexBuffer_Count);
}

void DrawingContext::SetViewSize(uint32_t width, uint32_t height)
{

}

void DrawingContext::SetClip(Vector4 clip)
{
}

void DrawingContext::Draw(Geometry& geometry)
{
	geometry.Freeze();
	auto count = geometry.Rasterize();
	for (uint32_t i = 0; i < count; i++)
	{
		auto& data = geometry.GetRasterizeData(i);
		if (NeedFlush(data))
		{
			Flush();
			_Pipline = data.pipline;
			_BlendMode = data.blend;
		}
		auto vsize = GetVertextItemSize();
		if(_VertextCount == 0)
		{
			//全新的buffer，直接copy就行
			memcpy_s(_VertextBuffer.buffer, _VertextBuffer.count, data.vertexBuffer, data.vertexCount * vsize);
			memcpy_s(_IndexBuffer.buffer, _IndexBuffer.count * sizeof(DrawIndex), data.indexBuffer, data.indexCount * sizeof(DrawIndex));
		}
		else
		{
			auto vertexPtr = _VertextBuffer.buffer + vsize * _VertextCount;
			memcpy_s(vertexPtr, _VertextBuffer.count - vsize * _VertextCount, data.vertexBuffer, data.vertexCount * vsize);
			auto indexPtr = _IndexBuffer.buffer + _IndexCount * sizeof(DrawIndex);
			memcpy_s(indexPtr, _IndexBuffer.count - _IndexCount * sizeof(DrawIndex), data.indexBuffer, data.indexCount * sizeof(DrawIndex));
			auto pIndex = (DrawIndex*)indexPtr;
			for (size_t i = 0; i < data.indexCount; i++)
			{
				pIndex[i] += _IndexCount;
			}
			_VertextCount += data.vertexCount;
			_IndexCount += data.indexCount;
		}
	}
}

bool DrawingContext::NeedFlush(const RasterizeData& data)
{
	if (_Pipline != data.pipline || _BlendMode != data.blend)
	{
		//渲染或者混合模式不行
		return true;
	}
	if (data.indexCount + _IndexCount > _IndexBuffer.count)
	{
		/// indexbuff 满了
		return true;
	}
	uint32_t vertexsize = GetVertextItemSize();
	if (data.vertexCount * _VertextCount * vertexsize > _VertextBuffer.count)
	{
		//vertex满了
		return true;
	}
	return false;
}

uint32_t DrawingContext::GetVertextItemSize()
{
	return uint32_t();
}

void DrawingContext::Flush()
{
	if (_VertextCount == 0)
	{
		return;
	}

	_VertextCount = 0;
	_IndexCount = 0;
}
