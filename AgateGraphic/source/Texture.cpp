#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
namespace agate
{
    std::map<std::wstring, std::weak_ptr<Texture>> Texture::g_BitmapStorage;

    std::shared_ptr<Texture> Texture::CreateFromFile(const std::wstring& fileName)
    {
        auto found = g_BitmapStorage.find(fileName);
        if (found != g_BitmapStorage.end())
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
            fclose(f);
            if (bits != nullptr)
            {
                auto result = std::make_shared<Texture>();
                result->_Data.bites = bits;
                result->_Data.width = w;
                result->_Data.height = h;
                result->_Data.pitch = 4 * w;
                g_BitmapStorage[fileName] = result;
                return result;
            }
        }
        return std::shared_ptr<Texture>();
    }
}