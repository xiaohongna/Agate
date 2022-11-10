#include "AssetManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace agate
{
    TextureHandle ImageAsset::GetTexture()
    {
        if (_Texture)
        {
            return _Texture;
        }
        if (_SysMemory == nullptr)
        {
            assert(false);
            return _Texture;
        }
        auto& mgr = AssetManager::SharedInstance();
        _Texture = mgr._Delegate->CreateTexture(_Width, _Height, _SysMemory);
        if (_Texture != nullptr && mgr._Delegate->GetConfig().holdMemory == false)
        {
            delete _SysMemory;
            _SysMemory = nullptr;
        }
        return _Texture;
    }

    void AssetManager::Invalidate()
    {
        for (auto& asset : _ImageStorage)
        {
            auto image = asset.second.lock();
            if (image)
            {
                _Delegate->ReleaseTexture(image->_Texture);
                image->_Texture = nullptr;
            }
        }
    }

    std::shared_ptr<ImageAsset> AssetManager::CreateImage(std::wstring filePath)
    {
        auto found = _ImageStorage.find(filePath);
        if (found != _ImageStorage.end())
        {
            auto result = found->second.lock();
            if (result)
            {
                return result;
            }
        }

        FILE* f = nullptr;
        if (0 == _wfopen_s(&f, filePath.c_str(), L"rb"))
        {
            int channel = 0;
            int w{}, h{};
            auto bits = stbi_load_from_file(f, &w, &h, &channel, 4);
            auto rt = fclose(f);
            if (bits != nullptr)
            {
                auto image = std::make_shared<ImageAsset>();
                image->_Width = static_cast<uint32_t>(w);
                image->_Height = static_cast<uint32_t>(h);
                image->_SysMemory = bits;
                _ImageStorage[filePath] = image;
                return image;
            }
        }
        return std::shared_ptr<ImageAsset>();

    }

    std::shared_ptr<ImageAsset> AssetManager::CreateImage(uint32_t width, uint32_t height)
    {
        auto image = std::make_shared<ImageAsset>();
        image->_Width = width;
        image->_Height = height;
        return image;
    }

}