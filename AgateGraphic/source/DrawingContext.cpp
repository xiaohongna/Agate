#include "DrawingContext.h"

namespace agate
{

	constexpr  uint32_t Max_IndexBuffer_Count = 1024 * 20;

	constexpr  uint32_t Max_VertexBuffer_Count = 1024 * 10;

	DrawingContext::DrawingContext(std::unique_ptr<IRendererDelegate>&& delegate) :
		_ClipX{ 0 },
		_ClipY{ 0 },
		_CurrentBatch{},
		_ClipChanged{ true }
	{
		_Renderer = std::move(delegate);
		_VertextBuffer.Alloc<char>(Max_VertexBuffer_Count * sizeof(VertexXYUVColor));
		_IndexBuffer.Alloc<DrawIndex>(Max_IndexBuffer_Count);
		_CurrentBatch.vertexData = _VertextBuffer.buffer;
		_CurrentBatch.indexData = _IndexBuffer.buffer;
		_Renderer->GetViewSize(_ClipWidth, _ClipHeight);
	}

	void DrawingContext::SetViewSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
		{
			return;
		}
		_ClipX = 0;
		_ClipY = 0;
		_ClipWidth = width;
		_ClipHeight = height;
		_Renderer->SetViewSize(width, height);
	}

	void DrawingContext::SetClip(const Vector4& clip)
	{
		_ClipX = (uint32_t)clip.left;
		_ClipY = (uint32_t)clip.top;
		_ClipWidth = (uint32_t)(clip.right - clip.left);
		_ClipHeight = (uint32_t)(clip.bottom - clip.top);
		_ClipChanged = true;
	}

	void DrawingContext::BeginDraw()
	{
		_Renderer->BeginDraw();
	}

	void DrawingContext::SetTarget(const std::shared_ptr<ImageAsset>& image)
	{
		if (_CurrentTarget != image)
		{
			Flush();
			_CurrentTarget = image;
			_ClipX = 0;
			_ClipY = 0;
			_ClipChanged = true;
			if (image == nullptr)
			{
				_Renderer->SetRenderTarget(nullptr);
				_Renderer->GetViewSize(_ClipWidth, _ClipHeight);
			}
			else
			{
				_ClipWidth = image->GetWidth();
				_ClipHeight = image->GetHeight();
				_Renderer->SetRenderTarget(image->GetTexture());
			}
		}
	}

	void DrawingContext::Clean(Color clearColor)
	{
		_Renderer->Clear(clearColor);
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

	void DrawingContext::EndDraw(uint32_t sync)
	{
		assert(_CurrentTarget == nullptr);
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
			for (uint32_t i = 0; i < MaxTextureCount; i++)
			{
				cmd.texture[i] = data.texture[i];
				cmd.samplers[i] = data.samplers[i];
			}
			_CurrentBatch.commands.emplace_back(cmd);
		}
		else
		{
			auto& back = _CurrentBatch.commands.back();
			if (CanMergeCommand(back, data))
			{
				back.indexCount += data.index.count;
			}
			else
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
				for (uint32_t i = 0; i < MaxTextureCount; i++)
				{
					cmd.texture[i] = data.texture[i];
					cmd.samplers[i] = data.samplers[i];
				}
				_ClipChanged = false;
				_CurrentBatch.commands.emplace_back(cmd);
			}
		}
	}
	bool DrawingContext::CanMergeCommand(const DrawCommand& cmd, const RasterizeData& data) const
	{
		if (_ClipChanged)
		{
			return false;
		}
		if (cmd.blend != data.blend)
		{
			return false;
		}
		for (uint32_t i = 0; i < MaxTextureCount; i++)
		{
			if (cmd.texture[i] != data.texture[i] || cmd.samplers[i] != data.samplers[i])
			{
				return false;
			}
		}
		return true;
	}
}