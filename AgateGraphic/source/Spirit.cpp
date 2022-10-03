#include "Spirit.h"
#include "DrawingContext.h"

constexpr uint32_t Spirit_Flag_Rasterize = 1;

void Spirit::SetImage(const std::shared_ptr<Image>& img)
{
    _Image = img;
}

void Spirit::SetClip(const Vector4& clip)
{
    _Clip = clip;
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
    auto vertex = _RasterData.vertex.Alloc<VertexXYUVColor>(4);
    auto index = _RasterData.index.Alloc<DrawIndex>(6);

    Vector2 posMin = _Bounds.pos;
    Vector2 posMax = _Bounds.pos + _Bounds.size;

    vertex[0].pos = _Matrix.TransformPoint(posMin);
    vertex[0].uv = { 0.0f, 0.0f };
    vertex[0].col = 0x5FFFFFFF;

    vertex[1].pos = _Matrix.TransformPoint({ posMax.x, posMin.y });
    vertex[1].uv = { 1.0f, 0.0f };
    vertex[1].col = 0x5FFFFFFF;

    vertex[2].pos = _Matrix.TransformPoint(posMax);
    vertex[2].uv = { 1.0f, 1.0f };
    vertex[3].col = 0x5FFFFFFF;

    vertex[3].pos = _Matrix.TransformPoint({ posMin.x, posMax.y });
    vertex[3].uv = { 0.0f, 1.0f };
    vertex[3].col = 0x5FFFFFFF;

    index[0] = 0; index[1] = 1; index[2] = 2;
    index[3] = 2; index[4] = 3; index[5] = 0;

    return 1;
}

const RasterizeData& Spirit::GetRasterizeData(uint32_t index)
{
    return _RasterData;
}
