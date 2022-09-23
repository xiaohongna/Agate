#pragma once

#include "Vertex.h"

class IRenderer
{
public:
	virtual void SetViewPort(uint32_t width, uint32_t height) = 0;

	virtual void Draw(const RasterizeData& data)  = 0;

	virtual void Present(uint32_t sync) = 0;

};

