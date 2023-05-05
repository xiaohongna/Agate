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
		uint64_t maxVideoMemory;  //����ʲ��Դ�ռ��
		bool holdMemory;
	};

	class AgateAPI IAssetManagerDelegate
	{
	public:
		virtual const AssetManagerConfig& GetConfig() = 0;

		virtual TextureHandle CreateRenderTarget(uint32_t width, uint32_t height) = 0;
		/// <summary>
		/// ����������Դ����ʽΪR8G8B8A8_UNORM
		/// </summary>
		/// <param name="width">����</param>
		/// <param name="height">�߶�</param>
		/// <param name="sysMem">�ڴ�</param>
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
		/// ��Ч��������Դ����D3D�豸��ʧʱ����
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