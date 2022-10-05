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
    //if (_Matrix.IsRotation())
    {
        //RasterizeRotaion();
    }
    //else
    {
        RasterizeNonoRotaion();
    }
   // return 1;
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

inline void Normalize(float& vx, float& vy)
{
    float d2 = vx * vx + vy * vy; 
    if (d2 > 0.0f) 
    { 
        float inv_len = 1.0f / sqrtf(d2);
        vx *= inv_len; 
        vy *= inv_len; 
    }
}

void Spirit::RasterizeRotaion()
{
    constexpr float AA_SIZE = 1.f;
    auto vertex = _RasterData.vertex.Alloc<VertexXYUVColor>(8);
    auto index = _RasterData.index.Alloc<DrawIndex>(30);

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
    Vector2 temp_normals[4];
    for (int i = 1; i < 5; i++)
    {
        float vx = rect[i].x - rect[i - 1].x;
        float vy = rect[i].y - rect[i - 1].y;
        Normalize(vx, vy);
        temp_normals[i - 1].x = vy;
        temp_normals[i - 1].y = - vx;
    }

    for (int i = 2; i < 4; i++)
    {
        index[0] = (DrawIndex)(0); index[1] = (DrawIndex)((i - 1) << 1); index[2] = (DrawIndex)(i << 1);
        index += 3;
    }
    const uint32_t col_trans = _Color.color & 0xFFFFFF;
    for (int i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        const Vector2& n0 = temp_normals[i0];
        const Vector2& n1 = temp_normals[i1];
        float dm_x = (n0.x + n1.x) * 0.5f;
        float dm_y = (n0.y + n1.y) * 0.5f;
        dm_x *= AA_SIZE;
        dm_y *= AA_SIZE;

        vertex[0].pos.x = (rect[i1].x); vertex[0].pos.y = (rect[i1].y); vertex[0].col = _Color.color;        // Inner
        vertex[1].pos.x = (rect[i1].x + dm_x); vertex[1].pos.y = (rect[i1].y + dm_y); vertex[1].col = col_trans;  // Outer
        vertex += 2;

        // Add indexes for fringes
        index[0] = (DrawIndex)(i1 << 1); index[1] = (DrawIndex)(i0 << 1); index[2] = (DrawIndex)(1 + (i0 << 1));
        index[3] = (DrawIndex)(1 + (i0 << 1)); index[4] = (DrawIndex)(1 + (i1 << 1)); index[5] = (DrawIndex)(i1 << 1);
        index += 6;
    }

    auto& img = _Image->GetImageData();
    Vector2 minUV(_Clip.x / img.width, _Clip.y / img.height);
    Vector2 maxUV(_Clip.right / img.width, _Clip.bottom / img.height);
    auto vertexUV = _RasterData.vertex.As<VertexXYUVColor>();
    vertexUV[6].uv = vertex[7].uv = minUV;
    vertexUV[0].uv = vertexUV[1].uv = { maxUV.x, minUV.y };
    vertexUV[2].uv = vertexUV[3].uv = maxUV;
    vertexUV[4].uv = vertexUV[5].uv = { minUV.x, maxUV.y };
    
}

void Spirit::GetUV(Vector2& minUV, Vector2& maxUV)
{
    auto& img =_Image->GetImageData();
    minUV.x = _Clip.x / img.width;
    minUV.y = _Clip.y / img.height;
    maxUV.x = _Clip.right / img.width;
    maxUV.y = _Clip.bottom / img.height;
}
