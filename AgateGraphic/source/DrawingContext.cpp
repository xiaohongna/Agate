#include "DrawingContext.h"

namespace agate
{

	constexpr  uint32_t Max_IndexBuffer_Count = 1024 * 20;

	constexpr  uint32_t Max_VertexBuffer_Count = 1024 * 10;

	DrawingContext::DrawingContext(IRenderer* delegate) :
		_ViewWidth{ 1024 },
		_ViewHeight{ 768 },
		_ClipX{ 0 },
		_ClipY{ 0 },
		_ClipWidth{ 1024 },
		_ClipHeight{ 768 },
		_Renderer(delegate),
		_CurrentBatch{},
		_ClipChanged{ true }
	{
		_VertextBuffer.Alloc<char>(Max_VertexBuffer_Count * sizeof(VertexXYUVColor));
		_IndexBuffer.Alloc<DrawIndex>(Max_IndexBuffer_Count);
		_CurrentBatch.vertexData = _VertextBuffer.buffer;
		_CurrentBatch.indexData = _IndexBuffer.buffer;
	}

	void DrawingContext::SetViewSize(uint32_t width, uint32_t height)
	{
		_ClipX = 0;
		_ClipY = 0;
		_ClipWidth = width;
		_ClipHeight = height;
		_Renderer->SetViewPort(width, height);
	}

	void DrawingContext::SetClip(const Vector4& clip)
	{
		_ClipX = (uint32_t)clip.left;
		_ClipY = (uint32_t)clip.top;
		_ClipWidth = (uint32_t)(clip.right - clip.left);
		_ClipHeight = (uint32_t)(clip.bottom - clip.top);
		_ClipChanged = true;
	}

	void DrawingContext::BeginDraw(bool clear, uint32_t clearColor)
	{
		_Renderer->BeginDraw();
		_Renderer->SetRenderTarget();
		if (clear)
		{
			Vector4 clr(1.0f, 1.0f, 1.0f, 1.0f);
			_Renderer->Clear(clr);
		}
	}

	void DrawingContext::Draw(Drawable& drawing)
	{
		auto count = drawing.Rasterize(*this);
		for (uint32_t i = 0; i < count; i++)
		{
			auto& data = drawing.GetRasterizeData(i);
			Draw(data);
		}
	}

	TextureHandle DrawingContext::CreateTexture(Texture* img)
	{
		auto& data = img->GetImageData();
		return _Renderer->CreateTexture(data);
	}

	void DrawingContext::ReleaseTexture(Texture* img)
	{

	}

	void DrawingContext::EndDraw(uint32_t sync)
	{
		Flush();
		_Renderer->EndDraw(sync);
	}

	void DrawingContext::Draw(const RasterizeData& data)
	{
		if (NeedFlush(data))
		{
			Flush();
		}
		PushCommnd(data);

		auto itemSize = data.vertex.preSize;
		if (_CurrentBatch.vertexCount == 0)
		{
			//全新的buffer，直接copy就行
			memcpy_s(_VertextBuffer.buffer, _VertextBuffer.size, data.vertex.buffer, data.vertex.size);
			memcpy_s(_IndexBuffer.buffer, _IndexBuffer.size, data.index.buffer, data.index.size);
			_CurrentBatch.vertexCount = data.vertex.count;
			_CurrentBatch.indexCount = data.index.count;
		}
		else
		{
			auto vertexPtr = _VertextBuffer.buffer + itemSize * _CurrentBatch.vertexCount;
			memcpy_s(vertexPtr, _VertextBuffer.size - itemSize * _CurrentBatch.vertexCount, data.vertex.buffer, data.vertex.size);
			auto indexPtr = _IndexBuffer.buffer + _CurrentBatch.indexCount;
			memcpy_s(indexPtr, _IndexBuffer.size - _CurrentBatch.indexCount * sizeof(DrawIndex), data.index.buffer, data.index.size);
			for (size_t i = 0; i < data.index.count; i++)
			{
				*indexPtr += _CurrentBatch.vertexCount;
				indexPtr++;
			}
			_CurrentBatch.vertexCount += data.vertex.count;
			_CurrentBatch.indexCount += data.index.count;
		}
	}

	bool DrawingContext::NeedFlush(const RasterizeData& data)
	{
		if (_CurrentBatch.pipline != data.pipline)
		{
			//渲染管线不同
			return true;
		}
		if (data.index.count + _CurrentBatch.indexCount > _IndexBuffer.count)
		{
			/// indexbuff 满了
			return true;
		}

		if (data.vertex.count + _CurrentBatch.vertexCount > _VertextBuffer.count)
		{
			//vertex满了
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
			_CurrentBatch.pipline = data.pipline;
			DrawCommand cmd{};
			cmd.blend = data.blend;
			cmd.indexCount = data.index.count;
			if (_ClipChanged)
			{
				cmd.clipX = _ClipX;
				cmd.clipY = _ClipY;
				cmd.clipWidth = _ClipWidth;
				cmd.clipHeight = _ClipHeight;
				_ClipChanged = false;
			}
			cmd.texture = data.texture;
			_CurrentBatch.commands.emplace_back(cmd);
		}
		else
		{
			auto& back = _CurrentBatch.commands.back();
			if (back.blend != data.blend || _ClipChanged || back.texture != data.texture)
			{
				DrawCommand cmd{};
				cmd.blend = data.blend;
				if (_ClipChanged)
				{
					cmd.clipX = _ClipX;
					cmd.clipY = _ClipY;
					cmd.clipWidth = _ClipWidth;
					cmd.clipHeight = _ClipHeight;
				}
				cmd.indexCount = data.index.count;
				cmd.startIndexLocation = _CurrentBatch.indexCount;
				cmd.texture = data.texture;
				_ClipChanged = false;
				_CurrentBatch.commands.emplace_back(cmd);
			}
			else
			{
				back.indexCount += data.index.count;
			}
		}

	}
}