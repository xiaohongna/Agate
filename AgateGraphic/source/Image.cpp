#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

std::map<std::wstring, std::weak_ptr<Image>> Image::g_ImageStorage;

std::shared_ptr<Image> Image::CreateFromFile(const std::wstring& fileName)
{
    auto found = g_ImageStorage.find(fileName);
    if (found != g_ImageStorage.end())
    {
        auto result = found->second.lock();
        if (result)
        {
            return result;
        }
    }
    
    FILE* f = nullptr;
    if (0 == _wfopen_s(&f, fileName.c_str(), L"rb"))
    {
        int channel = 0;
        int w{}, h{};
        auto bits = stbi_load_from_file(f, &w, &h, &channel, 4);
        auto c = fclose(f);
        if (bits != nullptr)
        {
            auto result = std::make_shared<Image>();
            result->_Data.bites = bits;
            result->_Data.width = w;
            result->_Data.height = h;
            result->_Data.pitch = 4 * w;
            g_ImageStorage[fileName] = result;
            return result;
        }
    }
    return std::shared_ptr<Image>();
}
