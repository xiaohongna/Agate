#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include <map>

typedef void* Texture2D;

struct ImageData
{
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
	uint8_t* bites;
};

class Image
{
public:

	Image() : _Data{},
		_Texture{}
	{

	}
	
	const ImageData& GetImageData() const
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

	static std::shared_ptr<Image> CreateFromFile(const std::wstring& fileName);

	~Image()
	{
		if (_Data.bites)
		{
			delete _Data.bites;
			_Data.bites = nullptr;
		}
	}
private:
	ImageData	_Data;
	Texture2D	_Texture;

	static std::map<std::wstring, std::weak_ptr<Image>> g_ImageStorage;

};

