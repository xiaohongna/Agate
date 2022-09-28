#pragma once

#include "Geometry.h"
#include "Renderer.h"

class DrawingContext
{
public:
	DrawingContext(IRenderer* delegate);

	void SetViewSize(uint32_t width, uint32_t height);

	void SetClip(const Vector4& clip);
	
	void BeginDraw(bool clear, uint32_t clearColor);

	void Draw(Geometry& geometry);

	void EndDraw(uint32_t sync);
private:
	bool NeedFlush(const RasterizeData& data);
	
	void Flush();

	void PushCommnd(const RasterizeData& data);

private:
	IRenderer*				_Renderer;
	FixedBuffer<unsigned char>		_VertextBuffer;
	FixedBuffer<DrawIndex>	_IndexBuffer;

	BatchDrawData		    _CurrentBatch;

	uint32_t	_ViewWidth;
	uint32_t	_ViewHeight;
	uint32_t	_ClipX;
	uint32_t	_ClipY;
	uint32_t	_ClipWidth;
	uint32_t	_ClipHeight;
	bool		_ClipChanged;
};

