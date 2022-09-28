#pragma once

#include "Vertex.h"

struct DrawCommand
{
	BlendMode blend;
	uint32_t indexCount;
	uint32_t startIndexLocation;

	uint32_t clipX;
	uint32_t clipY;
	uint32_t clipWidth;
	uint32_t clipHeight;
};

struct BatchDrawData
{
	PipelineType pipline;
	unsigned char* vertexData;
	uint32_t vertexCount;
	DrawIndex* indexData;
	uint32_t indexCount;
	std::vector<DrawCommand> commands;
};

struct RendererConfig
{

};

class IRenderer
{
public:
	virtual void BeginDraw() = 0;

	virtual void SetRenderTarget() = 0;

	virtual void SetViewPort(uint32_t width, uint32_t height) = 0;

	virtual void Clear(const Vector4& color) = 0;

	virtual void Draw(const BatchDrawData& data)  = 0;

	virtual void EndDraw(uint32_t sync) = 0;

};

