#pragma once
#include "Renderer.h"

namespace agate
{
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
				size = sz;
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
		PipelineType pipline;
		BlendMode   blend;
		FixedBuffer<char> vertex;
		FixedBuffer<DrawIndex> index;
		Texture2D	texture;
		RasterizeData() :pipline{ PipelineType::Color },
			blend{ BlendMode::Blend },
			texture{}
		{

		}
	};

	class DrawingContext;

	class Drawable
	{
	public:
		virtual uint32_t Rasterize(DrawingContext& context) = 0;

		virtual const RasterizeData& GetRasterizeData(uint32_t index) = 0;
	};

}