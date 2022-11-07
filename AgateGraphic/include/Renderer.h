#pragma once
#include "Vertex.h"
#include "AssetManager.h"
#include "Brush.h"
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
		/// Alpha ���
		/// </summary>
		Blend,
		/// <summary>
		/// ��͸��
		/// </summary>
		Opacity,
		/// <summary>
		/// ����
		/// </summary>
		Additive,
		/// <summary>
		/// ���
		/// </summary>
		Subtract,
		/// <summary>
		/// ���
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

	class IRenderer
	{
	public:
		virtual void BeginDraw() = 0;

		virtual void SetRenderTarget() = 0;

		virtual void Clear(Color color) = 0;

		virtual void Draw(const BatchDrawData& data) = 0;

		virtual void EndDraw(uint32_t sync) = 0;

	};

}