#include "Image.h"
#include "DrawingContext.h"

namespace agate
{
    constexpr uint32_t Spirit_Flag_Rasterize = 1;
    constexpr uint32_t Spirit_Flag_ImageChanged = 1 << 1;
    constexpr uint32_t Spirit_Flag_ColorChanged = 1 << 2;
    constexpr uint32_t Spirit_Flag_Antialiasing = 1 << 3;

    Image::Image() :
            _Matrix{},
            _Flags{ 0 },
            _Color{ 0xFFFFFFFF },
            _NormalUV{ true },
            _Clip{ 0.0f, 0.0f, 1.0f, 1.0f },
            _ImageSampler{}
    {
            _Matrix._11 = 1.0f;
            _Matrix._22 = 1.0f;
            _RasterData.pipline = PipelineType::TextureColor;
            _RasterData.samplers[0] = SamplerMode::PointClamp;
    }

    void Image::SetAntialiasing(bool enable)
    {
        if (enable)
        {
            AddFlag(Spirit_Flag_Antialiasing);
        }
        else
        {
            RemoveFlag(Spirit_Flag_Antialiasing);
        }
        RemoveFlag(Spirit_Flag_Rasterize);
    }

    void Image::SetEffect(PipelineType effect)
    {
        _RasterData.pipline = effect;
    }

    void Image::SetBounds(const Vector4& bounds)
    {
        if (_Bounds != bounds)
        {
            _Bounds = bounds;
            RemoveFlag(Spirit_Flag_Rasterize);
        }
    }

    void Image::SetTexture(uint32_t index, const std::shared_ptr<ImageAsset>& img, SamplerMode sampler)
    {
        if (index >= MaxTextureCount)
        {
            assert(false);
            return;
        }
        if (_Images[index] != img)
        {
            _Images[index] = img;
            _ImageSampler[index] = sampler;
            AddFlag(Spirit_Flag_ImageChanged);
        }
    }

    void Image::SetClip(const Vector4& clip, bool normal)
    {
        if (clip != _Clip)
        {
            _Clip = clip;
            _NormalUV = normal;
            AddFlag(Spirit_Flag_ImageChanged);
        }
    }

    void Image::SetColor(const Color clr)
    {
        if (clr.color != _Color.color)
        {
            _Color = clr;
            AddFlag(Spirit_Flag_ColorChanged);
        }
    }

    void Image::SetTransform(const Matrix3X2& matrix)
    {
        if (matrix != _Matrix)
        {
            _Matrix = matrix;
            RemoveFlag(Spirit_Flag_Rasterize);
        }
    }

    uint32_t Image::Rasterize(DrawingContext& context)
    {
        for (uint32_t i = 0; i < MaxTextureCount; i++)
        {
            if (_Images[i] != nullptr)
            {
                _RasterData.texture[i] = _Images[i]->GetTexture();
                _RasterData.samplers[i] = _ImageSampler[i];
            }
        }
        
        if (HaveFlag(Spirit_Flag_Rasterize) == false)
        {
            Rasterize();
            AddFlag(Spirit_Flag_Rasterize);
        }
        else
        {
            if (HaveFlag(Spirit_Flag_ImageChanged))
            {
                UpdateUV();
            }
            if (HaveFlag(Spirit_Flag_ColorChanged))
            {
                UpdateColor();
            }
        }
        RemoveFlag(Spirit_Flag_ColorChanged | Spirit_Flag_ImageChanged);
        return 1;
    }

    const RasterizeData& Image::GetRasterizeData(uint32_t index)
    {
        return _RasterData;
    }

    void Image::RasterizeNoAntiAliasing(Vector2* rect)
    {
        auto vertex = _RasterData.vertex.Alloc<VertexXYUVColor>(4);
        auto index = _RasterData.index.Alloc<DrawIndex>(6);

        vertex[0].pos = rect[0];
        vertex[0].col = _Color.color;
        vertex[1].pos = rect[1];
        vertex[1].col = _Color.color;
        vertex[2].pos = rect[2];
        vertex[3].col = _Color.color;
        vertex[3].pos = rect[3];
        vertex[3].col = _Color.color;

        index[0] = 0; index[1] = 1; index[2] = 2;
        index[3] = 2; index[4] = 3; index[5] = 0;
        UpdateUV();
    }

    inline void Normalize(float& vx, float& vy)
    {
        float d2 = vx * vx + vy * vy;
        if (d2 > 0.001f)
        {
            float inv_len = 1.0f / sqrtf(d2);
            vx *= inv_len;
            vy *= inv_len;
        }
        else
        {
            vx = 0.f;
            vy = 0.f;
        }
    }

    inline bool NeedAntiAliasing(Vector2* rect)
    {
        for (int i = 1; i < 5; i++)
        {
            float vx = rect[i].x - rect[i - 1].x;
            float vy = rect[i].y - rect[i - 1].y;
            if (fabsf(vx) > 0.001f && fabsf(vy) > 0.001f)
            {
                return true;
            }
        }
        return false;
    }

    void Image::Rasterize()
    {
        constexpr float AA_SIZE = 1.0f;

        Vector2 posMin = _Bounds.pos;
        Vector2 posMax = _Bounds.pos + _Bounds.size;

        Vector2 rect[5];
        rect[0] = posMin;
        rect[1].x = posMax.x;
        rect[1].y = posMin.y;
        rect[2] = posMax;
        rect[3].x = posMin.x;
        rect[3].y = posMax.y;
        _Matrix.TransformPoint(rect, 4);
        rect[4] = rect[0];
        if (!HaveFlag(Spirit_Flag_Antialiasing) || !NeedAntiAliasing(rect))
        {
            RasterizeNoAntiAliasing(rect);
            return;
        }
        Vector2 edge_normals[4];
        for (int i = 1; i < 5; i++)
        {
            float vx = rect[i].x - rect[i - 1].x;
            float vy = rect[i].y - rect[i - 1].y;
            Normalize(vx, vy);
            edge_normals[i - 1].x = vy;
            edge_normals[i - 1].y = -vx;
        }

        const uint32_t col_trans = _Color.color & 0xFFFFFF;
        auto vertex = _RasterData.vertex.Alloc<VertexXYUVColor>(12);
        auto index = _RasterData.index.Alloc<DrawIndex>(30);
        for (int i = 0, indexbase = 0; i < 4; indexbase += 3, i++)
        {
            Vector2 offset{ edge_normals[i].x * AA_SIZE, edge_normals[i].y * AA_SIZE };
            vertex[0].pos = rect[i];  vertex[0].col = _Color.color;
            vertex[1].pos = rect[i] + offset; vertex[1].col = col_trans;
            vertex[2].pos = rect[i + 1] + offset; vertex[2].col = col_trans;
            index[0] = indexbase; index[1] = 1 + indexbase; index[2] = 2 + indexbase;
            index[3] = 2 + indexbase; index[4] = 3 + indexbase; index[5] = indexbase;
            index += 6;
            vertex += 3;
        }
        index = _RasterData.index.As<DrawIndex>();
        vertex = _RasterData.vertex.As<VertexXYUVColor>();
        index[22] = 0;
        index[23] = 9;
        index[24] = 0; index[25] = 3; index[26] = 6;
        index[27] = 6; index[28] = 9; index[29] = 0;
        UpdateUV();
    }

    inline float Clamp(float value)
    {
        if (value > 1.0f)
        {
            return 1.0f;
        }
        else if (value < 0.0f)
        {
            return 0.0f;
        }
        else
        {
            return value;
        }
    }

    void Image::UpdateUV()
    {
        if (!_NormalUV)
        {
            _Clip.x = _Clip.x / _Images[0]->GetWidth();
            _Clip.y = _Clip.y / _Images[0]->GetHeight();
            _Clip.right = _Clip.right / _Images[0]->GetWidth();
            _Clip.bottom = _Clip.bottom / _Images[0]->GetHeight();
            _NormalUV = true;
        }
        Vector2 minUV(_Clip.x, _Clip.y);
        Vector2 maxUV(_Clip.right, _Clip.bottom);
        auto vertex = _RasterData.vertex.As<VertexXYUVColor>();
        if (_RasterData.vertex.count == 12)
        {
            vertex[0].uv = vertex[1].uv = vertex[11].uv = minUV;
            vertex[2].uv = vertex[3].uv = vertex[4].uv = Vector2(maxUV.x, minUV.y);
            vertex[5].uv = vertex[6].uv = vertex[7].uv = maxUV;
            vertex[8].uv = vertex[9].uv = vertex[10].uv = Vector2(minUV.x, maxUV.y);
        }
        else
        {
            vertex[0].uv = minUV;
            vertex[1].uv = { maxUV.x, minUV.y };
            vertex[2].uv = maxUV;
            vertex[3].uv = { minUV.x, maxUV.y };
        }
    }

    void Image::UpdateColor()
    {
        auto vertex = _RasterData.vertex.As<VertexXYUVColor>();
        if (_RasterData.vertex.count == 12)
        {
            const uint32_t col_trans = _Color.color & 0xFFFFFF;
            for (int i = 0; i < 4; i++)
            {
                vertex[0].col = _Color.color;
                vertex[1].col = col_trans;
                vertex[2].col = col_trans;
                vertex += 3;
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                vertex[i].col = _Color.color;
            }
        }
    }
}