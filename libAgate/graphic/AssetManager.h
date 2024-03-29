#pragma once
#include <cassert>
#include <stdint.h>
#include <string>
#include <memory>
#include <map>
#include "base/Ref.h"

namespace agate
{
	typedef void* TextureHandle;

	struct AgateAPI AssetManagerConfig
	{
		uint64_t maxVideoMemory;  //最大资产显存占用
		bool holdMemory;
	};

	class AgateAPI IAssetManagerDelegate
	{
	public:
		virtual const AssetManagerConfig& GetConfig() = 0;

		virtual TextureHandle CreateRenderTarget(uint32_t width, uint32_t height) = 0;
		/// <summary>
		/// 创建纹理资源，格式为R8G8B8A8_UNORM
		/// </summary>
		/// <param name="width">宽度</param>
		/// <param name="height">高度</param>
		/// <param name="sysMem">内存</param>
		/// <returns></returns>
		virtual TextureHandle CreateTexture(uint32_t width, uint32_t height, void* sysMem) = 0;

		virtual void ReleaseTexture(TextureHandle handle) = 0;
	};

	class AssetManager;

	class AgateAPI ImageAsset: public Ref
	{
	public:
		ImageAsset() :
			_Width{},
			_Height{},
			_SysMemory{},
			_Texture{}
		{

		};
		
		uint32_t GetWidth()
		{
			return _Width;
		}

		uint32_t GetHeight()
		{
			return _Height;
		}

		TextureHandle GetTexture();

		~ImageAsset() override
		{
			if (_SysMemory)
			{
				delete _SysMemory;
				_SysMemory = nullptr;
			}
			//AssetManager::SharedInstance;
		}
		
		friend AssetManager;
	private:
		uint32_t _Width;
		uint32_t _Height;
		void* _SysMemory;
		TextureHandle	_Texture;
	};

	class AgateAPI AssetManager
	{
	public:
		static AssetManager& SharedInstance()
		{
			static  AssetManager g_Manager;
			return g_Manager;
		};

		bool Initialize(IAssetManagerDelegate* delegate)
		{
			assert(_Delegate == nullptr);
			_Delegate = delegate;
			return true;
		}
		/// <summary>
		/// 无效化所有资源，在D3D设备丢失时调用
		/// </summary>
		void Invalidate();

		Share_Ptr<ImageAsset> CreateImage(std::wstring filePath);

		Share_Ptr<ImageAsset> CreateImage(uint32_t width, uint32_t height);

		friend ImageAsset;
	private:
		AssetManager() : _Delegate{}
		{
			
		}
	private:

		IAssetManagerDelegate* _Delegate;

		std::map<std::wstring, Weak_Ptr<ImageAsset>> _ImageStorage;
	};
}
