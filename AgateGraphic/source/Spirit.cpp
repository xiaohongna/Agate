#include "Spirit.h"
#include "DrawingContext.h"

constexpr uint32_t Spirit_Flag_Rasterize = 1;

void Spirit::SetImage(const std::shared_ptr<Image>& img)
{
    _Image = img;
}

void Spirit::SetClip(const Vector4& clip)
{
    if (clip != _Clip)
    {
        _Clip = clip;
        RemoveFlag(Spirit_Flag_Rasterize);
    }
}

void Spirit::SetColor(const Color clr)
{
    if (clr.color != _Color.color)
    {
        _Color = clr;
        RemoveFlag(Spirit_Flag_Rasterize);
    }
}

void Spirit::SetBlendMode(BlendMode blend)
{

}

void Spirit::SetTransform(const Matrix3X2 matrix)
{
    if (matrix != _Matrix)
    {
        _Matrix = matrix;
        RemoveFlag(Spirit_Flag_Rasterize);
    }
}

uint32_t Spirit::Rasterize(DrawingContext& context)
{
    if (HaveFlag(Spirit_Flag_Rasterize))
    {
        return 1;
    }
    AddFlag(Spirit_Flag_Rasterize);
    if (_Image->GetTexture() == nullptr)
    {
        auto txt = context.CreateTexture(_Image.get());
        _Image->SetTexture(txt);
    }
    _RasterData.texture = _Image->GetTexture();
    if (_Matrix.IsRotation())
    {

    }
    else
    {
        RasterizeNonoRotaion();
    }
    return 1;
}

const RasterizeData& Spirit::GetRasterizeData(uint32_t index)
{
    return _RasterData;
}

void Spirit::RasterizeNonoRotaion()
{
    auto vertex = _RasterData.vertex.Alloc<VertexXYUVColor>(4);
    auto index = _RasterData.index.Alloc<DrawIndex>(6);

    Vector2 posMin = _Bounds.pos;
    Vector2 posMax = _Bounds.pos + _Bounds.size;

    auto& img = _Image->GetImageData();
    Vector2 minUV(_Clip.x / img.width, _Clip.y / img.height);
    Vector2 maxUV(_Clip.right / img.width, _Clip.bottom / img.height);

    vertex[0].pos = _Matrix.TransformPoint(posMin);
    vertex[0].uv = minUV;
    vertex[0].col = _Color.color;

    vertex[1].pos = _Matrix.TransformPoint({ posMax.x, posMin.y });
    vertex[1].uv = { maxUV.x, minUV.y };
    vertex[1].col = _Color.color;

    vertex[2].pos = _Matrix.TransformPoint(posMax);
    vertex[2].uv = maxUV;
    vertex[3].col = _Color.color;

    vertex[3].pos = _Matrix.TransformPoint({ posMin.x, posMax.y });
    vertex[3].uv = { minUV.x, maxUV.y};
    vertex[3].col = _Color.color;

    index[0] = 0; index[1] = 1; index[2] = 2;
    index[3] = 2; index[4] = 3; index[5] = 0;
}

void Spirit::GetUV(Vector2& minUV, Vector2& maxUV)
{
    auto& img =_Image->GetImageData();
    minUV.x = _Clip.x / img.width;
    minUV.y = _Clip.y / img.height;
    maxUV.x = _Clip.right / img.width;
    maxUV.y = _Clip.bottom / img.height;
}
