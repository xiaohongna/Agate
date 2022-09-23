#pragma once

#include "Geometry.h"
#include "Renderer.h"

class DrawingContext
{
public:
	DrawingContext(IRenderer* delegate);

	void SetViewSize(uint32_t width, uint32_t height);

	void SetClip(Vector4 clip);
	
	void Draw(Geometry& geometry);

	void Present(uint32_t sync);
private:
	bool NeedFlush(const RasterizeData& data);
	
	void Flush();

	uint32_t GetVertextItemSize()
	{
		switch (_Pipline)
		{
		case PiplineType::Color:
			return sizeof(VertexXYColor);
		case PiplineType::Texture:
			return sizeof(VertexXYUV);
		case PiplineType::TextureColor:
			return sizeof(VertexXYUVColor);
		default:
			assert(false);
			return 0;
		}
	}

	
private:
	IRenderer*				_Renderer;
	FixedBuffer<char>		_VertextBuffer;
	uint32_t				_VertextCount;
	FixedBuffer<DrawIndex>	_IndexBuffer;
	uint32_t				_IndexCount;

	int32_t	_ViewWidth;
	int32_t	_ViewHeight;

	PiplineType  _Pipline;
	BlendMode	_BlendMode;
	Vector4		_ClipRgn;

};

