#pragma once

#include <stdint.h>
#include <vector>
#include "Vertex.h"
#include "base/Color.h"
#include "base/Ref.h"
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

	template<typename T>
	struct FixedBuffer
	{
		T* buffer;
		uint32_t count;   //元素数量
		uint32_t preSize;  //单元素大小
		uint32_t size;    //buffer内存大小
		FixedBuffer() :buffer{}, count{}, size{}, preSize{ 1 }
		{

		}

		template<typename T1>
		T1* Alloc(uint32_t count)
		{
			assert(count > 0);
			this->count = count;
			preSize = sizeof(T1);
			auto sz = count * sizeof(T1);
			if (sz != size)
			{
				size = (uint32_t)sz;
				if (buffer)
					delete buffer;
				buffer = (T*)malloc(size);
			}
			return (T1*)buffer;
		}

		void Reset()
		{
			if (buffer)
			{
				delete buffer;
				buffer = nullptr;
				count = 0;
				size = 0;
			}
		}

		template<typename T1>
		T1* As() const
		{
			assert(size == count * sizeof(T1));
			return (T1*)buffer;
		}

		~FixedBuffer()
		{
			Reset();
		}
	};

	struct RasterizeData
	{
		EffectType pipline;
		BlendMode   blend;
		FixedBuffer<char> vertex;
		FixedBuffer<DrawIndex> index;
		TextureHandle	texture[MaxTextureCount];
		SamplerMode		samplers[MaxTextureCount];
		RasterizeData() :
			pipline{ EffectType::Color },
			blend{ BlendMode::Blend },
			texture{},
			samplers{}
		{

		}
	};


	struct BatchDrawData
	{
		EffectType pipline;
		unsigned char* vertexData;
		uint32_t vertexCount;
		DrawIndex* indexData;
		uint32_t indexCount;
		std::vector<DrawCommand> commands;
	};

	class AgateAPI IRendererDelegate: public Ref
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