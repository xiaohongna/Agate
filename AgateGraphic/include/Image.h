#pragma once
#include <stdint.h>
#include <string>

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
	Image() : _Channel{ 4 },
		_Width{0},
		_Height{0},
		_Bites {}
	{

	}

	bool Create(const std::wstring& fileName);

	//bool Create(uint32_t width, uint32_t height);

	~Image()
	{
		if (_Bites)
		{
			delete _Bites;
		}
	}
private:
	uint32_t	_Channel;
	uint32_t	_Width;
	uint32_t	_Height;
	uint8_t*	_Bites;

};

