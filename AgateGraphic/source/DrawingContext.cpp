#include "DrawingContext.h"

constexpr  uint32_t Max_IndexBuffer_Count = 1024 * 20;

constexpr  uint32_t Max_VertexBuffer_Count = 1024 * 10;

DrawingContext::DrawingContext(IRenderer* delegate):
_ViewWidth{1024},
_ViewHeight{768},
_ClipX{0},
_ClipY{0},
_ClipWidth{1024},
_ClipHeight{768},
_Renderer(delegate),
_CurrentBatch{},
_ClipChanged{true}
{
	_VertextBuffer.Alloc<char>(Max_VertexBuffer_Count * sizeof(VertexXYUVColor));
	_IndexBuffer.Alloc<DrawIndex>(Max_IndexBuffer_Count);
	_CurrentBatch.vertexData = _VertextBuffer.buffer;
	_CurrentBatch.indexData = _IndexBuffer.buffer;
}

void DrawingContext::SetViewSize(uint32_t width, uint32_t height)
{
	_ViewWidth = width;
	_ViewHeight = height;
	_ClipX = 0;
	_ClipY = 0;
	_ClipWidth = width;
	_ClipHeight = height;
}

void DrawingContext::SetClip(Vector4 clip)
{
	_ClipX = clip.x;
	_ClipY = clip.y;
	_ClipWidth = clip.z - clip.x;
	_ClipHeight = clip.w - clip.y;
	_ClipChanged = true;
}

void DrawingContext::BeginDraw(bool clear, uint32_t clearColor)
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
		}
		PushCommnd(data);

		auto itemSize = _VertextBuffer.preSize;
		if(_CurrentBatch.vertexCount == 0)
		{
			//ȫ�µ�buffer��ֱ��copy����
			memcpy_s(_VertextBuffer.buffer, _VertextBuffer.size, data.vertex.buffer, data.vertex.size);
			memcpy_s(_IndexBuffer.buffer, _IndexBuffer.size, data.index.buffer, data.index.size);
		}
		else
		{
			auto vertexPtr = _VertextBuffer.buffer + itemSize * _CurrentBatch.vertexCount;
			memcpy_s(vertexPtr, _VertextBuffer.size - itemSize * _CurrentBatch.vertexCount, data.vertex.buffer, data.vertex.size);
			auto indexPtr = _IndexBuffer.buffer + _CurrentBatch.indexCount;
			memcpy_s(indexPtr, _IndexBuffer.size - _CurrentBatch.indexCount * sizeof(DrawIndex), data.index.buffer, data.index.size);
			for (size_t i = 0; i < data.index.count; i++)
			{
				*indexPtr += _CurrentBatch.indexCount;
				indexPtr++;
			}
			_CurrentBatch.vertexCount += data.vertex.count;
			_CurrentBatch.indexCount += data.index.count;
		}
	}
}

void DrawingContext::EndDraw(uint32_t sync)
{
	Flush();
	_Renderer->Present(sync);
}

bool DrawingContext::NeedFlush(const RasterizeData& data)
{
	if (_CurrentBatch.pipline != data.pipline)
	{
		//��Ⱦ���߲�ͬ
		return true;
	}
	if (data.index.count + _CurrentBatch.indexCount > _IndexBuffer.count)
	{
		/// indexbuff ����
		return true;
	}
	
	if (data.vertex.count + _CurrentBatch.vertexCount > _VertextBuffer.count)
	{
		//vertex����
		return true;
	}
	return false;
}

void DrawingContext::Flush()
{
	if (_CurrentBatch.commands.empty())
	{
		return;
	}
	_Renderer->Draw(_CurrentBatch);
	_CurrentBatch.indexCount = 0;
	_CurrentBatch.vertexCount = 0;
	_CurrentBatch.commands.clear();
}

void DrawingContext::PushCommnd(const RasterizeData& data)
{
	if (_CurrentBatch.commands.empty()) 
	{
		DrawCommand cmd 
		{ 
			data.blend, 
			data.index.count,
			_ClipX,
			_ClipY,
			_ClipWidth,
			_ClipHeight
		};
		_ClipChanged = false;
		_CurrentBatch.commands.emplace_back(cmd);
	}
	else
	{
		auto& back = _CurrentBatch.commands.back();
		if (back.blend != data.blend || _ClipChanged)
		{
			DrawCommand cmd{};
			cmd.blend = data.blend;
			if (_ClipChanged)
			{
				cmd.clipLeft = _ClipX;
				cmd.clipTop = _ClipY;
				cmd.clipWidth = _ClipWidth;
				cmd.clipHeight = _ClipHeight;
			}
			cmd.indexCount = data.index.count;
			cmd.startIndexLocation = _CurrentBatch.indexCount - 1;
			_ClipChanged = false;
		}
		else
		{
			back.indexCount += data.index.count;
		}
	}
	
}
