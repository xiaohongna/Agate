#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <map>

namespace agate
{
	typedef void* Texture2D;

	struct BitmapData
	{
		uint32_t width;
		uint32_t height;
		uint32_t pitch;
		uint8_t* bites;
	};

	class Texture
	{
	public:

		Texture() : _Data{},
			_Texture{}
		{

		}

		const BitmapData& GetImageData() const
		{
			return _Data;
		}

		Texture2D GetTexture()
		{
			return _Texture;
		}
		void SetTexture(Texture2D texture)
		{
			_Texture = texture;
		}

		static std::shared_ptr<Texture> CreateFromFile(const std::wstring& fileName);

		~Texture()
		{
			if (_Data.bites)
			{
				delete _Data.bites;
				_Data.bites = nullptr;
			}
		}
	private:
		BitmapData	_Data;
		Texture2D	_Texture;

		static std::map<std::wstring, std::weak_ptr<Texture>> g_BitmapStorage;

	};

}