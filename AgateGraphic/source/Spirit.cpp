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
    RasterizeRotaionEx();
    return 1;
}

const RasterizeData& Spirit::GetRasterizeData(uint32_t index)
{
    return _RasterData;
}

void Spirit::RasterizeNonoRotaion(Vector2* rect)
{
    auto vertex = _RasterData.vertex.Alloc<VertexXYUVColor>(4);
    auto index = _RasterData.index.Alloc<DrawIndex>(6);

    auto& img = _Image->GetImageData();
    Vector2 minUV(_Clip.x / img.width, _Clip.y / img.height);
    Vector2 maxUV(_Clip.right / img.width, _Clip.bottom / img.height);

    vertex[0].pos = rect[0];
    vertex[0].uv = minUV;
    vertex[0].col = _Color.color;

    vertex[1].pos = rect[1];
    vertex[1].uv = { maxUV.x, minUV.y };
    vertex[1].col = _Color.color;

    vertex[2].pos = rect[2];
    vertex[2].uv = maxUV;
    vertex[3].col = _Color.color;

    vertex[3].pos = rect[3];
    vertex[3].uv = { minUV.x, maxUV.y};
    vertex[3].col = _Color.color;

    index[0] = 0; index[1] = 1; index[2] = 2;
    index[3] = 2; index[4] = 3; index[5] = 0;
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

inline bool IsAntiAliasing(Vector2* rect)
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

void Spirit::RasterizeRotaionEx()
{
    constexpr float AA_SIZE = 1.2f;

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
    if (IsAntiAliasing(rect) == false)
    {
        RasterizeNonoRotaion(rect);
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

    auto& img = _Image->GetImageData();
    Vector2 minUV(_Clip.x / img.width, _Clip.y / img.height);
    Vector2 maxUV(_Clip.right / img.width, _Clip.bottom / img.height);
    const uint32_t col_trans = _Color.color  & 0xFFFFFF;
    auto vertex = _RasterData.vertex.Alloc<VertexXYUVColor>(12);
    auto index = _RasterData.index.Alloc<DrawIndex>(30);
    for (int i = 0, indexbase = 0; i < 4; indexbase +=3, i++)
    {
        Vector2 offset{ edge_normals[i].x * AA_SIZE, edge_normals[i].y * AA_SIZE };
        vertex[0].pos = rect[i];  vertex[0].col = _Color.color;
        vertex[1].pos = rect[i] + offset; vertex[1].col = col_trans;
        vertex[2].pos = rect[i + 1] + offset; vertex[2].col = col_trans;
        index[0] = indexbase; index[1] = 1 + indexbase; index[2] =  2 + indexbase;
        index[3] = 2 + indexbase; index[4] =  3 + indexbase; index[5] = indexbase;
        index += 6;
        vertex += 3;
    }
    index = _RasterData.index.As<DrawIndex>();
    vertex = _RasterData.vertex.As<VertexXYUVColor>();
    index[22] = 0;
    index[23] = 9;
    index[24] = 0; index[25] = 3; index[26] = 6;
    index[27] = 6; index[28] = 9; index[29] = 0;

    vertex[0].uv = vertex[1].uv = vertex[11].uv = minUV;
    vertex[2].uv = vertex[3].uv = vertex[4].uv = Vector2(maxUV.x, minUV.y);
    vertex[5].uv = vertex[6].uv = vertex[7].uv = maxUV;
    vertex[8].uv = vertex[9].uv = vertex[10].uv = Vector2(minUV.x, maxUV.y);
}

void Spirit::GetUV(Vector2& minUV, Vector2& maxUV)
{
    auto& img =_Image->GetImageData();
    minUV.x = _Clip.x / img.width;
    minUV.y = _Clip.y / img.height;
    maxUV.x = _Clip.right / img.width;
    maxUV.y = _Clip.bottom / img.height;
}
