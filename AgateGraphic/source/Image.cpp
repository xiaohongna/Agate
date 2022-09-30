#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

bool Image::Create(const std::wstring& fileName)
{
    FILE* f = nullptr;
    if (0 != _wfopen_s(&f, fileName.c_str(), L"rb"))
    {
        return false;
    }
    int channel = 0;
    int w{}, h{};
    _Bites = stbi_load_from_file(f, &w, &h, &channel, 4);
    fclose(f);
    _Width = w;
    _Height = h;
    return _Bites != nullptr;
}
