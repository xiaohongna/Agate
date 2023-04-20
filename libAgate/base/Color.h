#pragma once
#include <cstdint>

namespace agate
{
	union Color
	{
		struct
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;
		};
		
		uint32_t color;

		Color()
		{
			color = 0;
		}
		/// <summary>
		/// ABGR
		/// </summary>
		/// <param name="uintCol">32Î»ÑÕÉ«</param>
		Color(uint32_t uintCol)
		{
			color = uintCol;
		}

		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		{
			alpha = a;
			red = r;
			green = g;
			blue = b;
		}

	};

}
