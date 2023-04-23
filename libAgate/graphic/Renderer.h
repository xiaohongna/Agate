#pragma once

#include <stdint.h>
#include <vector>
#include "Vertex.h"
#include "Color.h"
namespace agate
{
	typedef void* TextureHandle;

	constexpr uint32_t MaxTextureCount = 2;

	enum class EffectType
	{
		Color,
		TextureColor,
		DistortTextureColor,
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

	enum class SamplerMode
	{
		PointClamp,
		PointWarp,
		LinearClamp,
		LinearWarp, 
		AnisotropicClamp,
		AnisotropicWarp, 
		SamplerModeCount
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
		TextureHandle texture[MaxTextureCount];
		SamplerMode samplers[MaxTextureCount];
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

	class IRendererDelegate
	{
	public:
		virtual void GetViewSize(uint32_t& width, uint32_t& height) = 0;

		virtual void SetViewSize(uint32_t& width, uint32_t& height) = 0;

		virtual void BeginDraw() = 0;

		virtual void SetRenderTarget(TextureHandle handle) = 0;

		virtual void Clear(Color color) = 0;

		virtual void Draw(const BatchDrawData& data) = 0;

		virtual void EndDraw(uint32_t sync) = 0;

	};

}