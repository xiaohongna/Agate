#pragma once
#include "Vertex.h"
#include "Texture.h"

namespace agate
{

	enum class PipelineType
	{
		Color,
		TextureColor,
	};

	enum class BlendMode
	{
		/// <summary>
		/// Alpha 混合
		/// </summary>
		Blend,
		/// <summary>
		/// 不透明
		/// </summary>
		Opacity,
		/// <summary>
		/// 叠加
		/// </summary>
		Additive,
		/// <summary>
		/// 相减
		/// </summary>
		Subtract,
		/// <summary>
		/// 相乘
		/// </summary>
		Multiply

	};

	enum class TextureAddressMode
	{
		Clamp,
		Mirror,
		Wrap,
	};

	struct DrawCommand
	{
		BlendMode blend;
		uint32_t indexCount;
		uint32_t startIndexLocation;
		TextureHandle texture;
		TextureAddressMode addressMode;

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

		virtual void Draw(const BatchDrawData& data) = 0;

		virtual void EndDraw(uint32_t sync) = 0;

		virtual TextureHandle CreateTexture(const BitmapData& data) = 0;

		virtual void ReleaseTexture(TextureHandle texture) = 0;

	};

}