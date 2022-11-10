#pragma once
#include "Brush.h"
#include "Drawable.h"
#include "Renderer.h"
#include "AssetManager.h"

namespace agate
{
	class DrawingContext
	{
	public:
		DrawingContext(std::unique_ptr<IRendererDelegate>&& delegate);

		void SetViewSize(uint32_t width, uint32_t height);

		void SetClip(const Vector4& clip);

		void BeginDraw();
		/// <summary>
		///  设置渲染目标，nullptr时渲染到窗口
		/// </summary>
		/// <param name="image"></param>
		void SetTarget(const std::shared_ptr<ImageAsset>& image);

		void Clean(Color clearColor);

		void Draw(Drawable& drawing);

		void EndDraw(uint32_t sync);
	private:
		void Draw(const RasterizeData& data);

		bool NeedFlush(const RasterizeData& data);

		void Flush();

		void PushCommnd(const RasterizeData& data);

	private:
		std::unique_ptr<IRendererDelegate> _Renderer;
		FixedBuffer<unsigned char>		_VertextBuffer;
		FixedBuffer<DrawIndex>	_IndexBuffer;

		BatchDrawData		    _CurrentBatch;
		std::shared_ptr<ImageAsset>  _CurrentTarget;
		uint32_t	_ClipX;
		uint32_t	_ClipY;
		uint32_t	_ClipWidth;
		uint32_t	_ClipHeight;
		bool		_ClipChanged;
	};

}