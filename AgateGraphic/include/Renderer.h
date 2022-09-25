#pragma once

#include "Vertex.h"

struct DrawCommand
{
	BlendMode blend;
	uint32_t indexCount;
	uint32_t startIndexLocation;

	uint32_t clipLeft;
	uint32_t clipTop;
	uint32_t clipWidth;
	uint32_t clipHeight;
};

struct BatchDrawData
{
	PiplineType pipline;
	char* vertexData;
	uint32_t vertexCount;
	DrawIndex* indexData;
	uint32_t indexCount;
	std::vector<DrawCommand> commands;
};

class IRenderer
{
public:
	virtual void SetViewPort(uint32_t width, uint32_t height) = 0;

	virtual void SetRenderTarget() = 0;

	virtual void Clear(const Vector4& color) = 0;

	virtual void Draw(const BatchDrawData& data)  = 0;

	virtual void Present(uint32_t sync) = 0;

};

