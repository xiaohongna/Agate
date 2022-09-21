#pragma once
#include <assert.h>
#include <stdint.h>
#include <vector>
#include <array>
#include "GraphicMath.h"


#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000


#ifndef IM_DRAWLIST_TEX_LINES_WIDTH_MAX
#define IM_DRAWLIST_TEX_LINES_WIDTH_MAX     (63)
#endif



struct DrawShareData
{
    // [Internal] Lookup tables
    ImVec2          ArcFastVtx[IM_DRAWLIST_ARCFAST_TABLE_SIZE]; // Sample points on the quarter of the circle.
    float           ArcFastRadiusCutoff;                        // Cutoff radius after which arc drawing will fallback to slower PathArcTo()
    int             CircleSegmentCounts[64];    // Precomputed segment count for given radius before we calculate it dynamically (to avoid calculation overhead)
    uint32_t        MaxTexture;             //每次绘制的最大texture数量
    DrawShareData();

    static DrawShareData& ShareData();

};

class PathSink
{
public:

    void ApplyTransition(const Matrix3X2& matrix);

    void AddLine(const ImVec2& pos);

    void AddRect(const ImVec2& rect_min, const ImVec2& rect_max);

    void AddRoundRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding = 0.0f);

    void AddEllipse(const ImVec2& center, float radiusX, float radiusY);

    void AddArc(const ImVec2& center, float radius, float a_min, float a_max, int num_segments = 0);

    void AddBezierCubicCurve(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments = 0); 

    void AddBezierQuadraticCurve(const ImVec2& p2, const ImVec2& p3, int num_segments = 0);  

    friend class DrawerList;
private:
    enum class PathCmdType
    {
        Line,
        Rect,
        RoundRect,
        Ellipse,
        Arc,
        CubicCurve,
        QuadraticCurve,
    };

    union PathData
    {
        struct
        {
            ImVec2 pos;
        } Line;
        struct
        {
            ImVec2 rect_min;
            ImVec2 rect_max;
        } Rect;
        struct
        {
            ImVec2 rect_min;
            ImVec2 rect_max;
            float rounding;
        } RoundRect;
        struct
        {
            ImVec2 center;
            float radiusX;
            float radiusY;
        } Ellipse;
        struct
        {
            ImVec2 center;
            float radius;
            float a_min; 
            float a_max;
            int num_segments;
        } Arc;
        struct
        {
            ImVec2 p2;
            ImVec2 p3;
            ImVec2 p4;
            int num_segments;
        } CubicCurve;
        struct
        {
            ImVec2 p2;  
            ImVec2 p3;
            int num_segments;
        } QuadraticCurve;
        PathData()
        {
            memset(&CubicCurve, 0, sizeof(CubicCurve));
        }
    };

    struct PathCmd
    {
        PathCmdType type;
        PathData data;
    };

    void AddLine(const ImVec2& pos, const Matrix3X2& matrix)
    {
        _Data.emplace_back(matrix.TransformPoint(pos));
    }

    void AddRect(const ImVec2& rect_min, const ImVec2& rect_max, const Matrix3X2& matrix);

    void AddRoundRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding, const Matrix3X2& matrix);

    void AddEllipse(ImVec2 center, float rx, float ry, const Matrix3X2& matrix);

    void AddArc(ImVec2 center, float radius, float a_min, float a_max, int num_segments, const Matrix3X2& matrix);

    void AddArcFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12);

    void AddBezierCubicCurve(ImVec2 p2, ImVec2 p3, ImVec2 p4, int num_segments, const Matrix3X2& matrix);

    void AddBezierQuadraticCurve(ImVec2 p2, ImVec2 p3, int num_segments, const Matrix3X2& matrix);

    void AddArcToFastEx(const ImVec2& center, float radius, int a_min_sample, int a_max_sample, int a_step);

    void AddArcToN(const ImVec2& center, float radius, float a_min, float a_max, int num_segments);

    int CalcCircleAutoSegmentCount(float radius) const;

    void PathBezierCubicCurveToCasteljau(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level);

    void PathBezierQuadraticCurveToCasteljau(float x1, float y1, float x2, float y2, float x3, float y3, float tess_tol, int level);

    void BuildPath();
private:
    std::vector<ImVec2> _Data;
    std::vector<PathCmd> _PathCmd;
};

enum class BrushType
{
    SolidColor,    //纯色
    Texture,  //纹理
    SolidColorTexture,  //颜色+纹理
};

class Brush
{
public:
    Brush(uint32_t col) :_Color{col}, _Texture{}, _Type(BrushType::SolidColor)
    {
        
    }
    Brush(void* texture) :_Color{0xFFFFFFFF}, _Texture{texture}, _Type(BrushType::Texture)
    {

    }
    Brush(uint32_t col, void* texture) :_Color{ col }, _Texture{ texture }, _Type(BrushType::SolidColorTexture)
    {

    }
    friend class  DrawerList;
private:
    BrushType _Type;
    uint32_t _Color;
    void* _Texture;
};

class DrawerList
{
public:
    DrawerList();
    void Reset()
    {
        _VertexCount = 0;
        _VertexIndexCount = 0;
        _Texture.clear();
        _Matrix = Matrix3X2::Identity();
        _Colors.resize(1);
        _FloatColors.resize(4);
    }
    
    void SetTransform(const Matrix3X2& matrix);

    void AddLineEx(const ImVec2& p1, const ImVec2& p2, uint32_t col, float thickness = 1.0f);

    void AddLine(const ImVec2& p1, const ImVec2& p2, uint32_t col, float thickness = 1.0f);

    void AddRect(const ImVec2& p_min, const ImVec2& p_max, uint32_t col, float thickness = 1.0f);

    void AddRoundRect(const ImVec2& p_min, const ImVec2& p_max, uint32_t col, float rounding, float thickness = 1.0f);

    void AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, const Brush& brush);

    void AddRoundRectFilled(const ImVec2& p_min, const ImVec2& p_max, uint32_t col, float rounding);
    
    void AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, uint32_t col, float thickness = 1.0f);

    void AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, uint32_t col);

    void AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, uint32_t col, uint32_t flag);

    void AddPathStroke(PathSink& path,  uint32_t col, bool close, float thickness = 1.0f);
    
    void AddPathFilled(PathSink& path, uint32_t col);

    void AddPathFilled(const PathSink& path, uint32_t col, uint32_t flag);

    void AddPolyline(const ImVec2* points, int points_count, uint32_t col, bool closed, float thickness);

    void AddImage(const ImVec2& p_min, const ImVec2& p_max, void* textID);

    DrawVert* GetVertex()
    {
        return _Vertex.data();
    }

    uint32_t GetVertexCount()
    {
        return _VertexCount;
    }

    ImDrawIdx* GetVertexIndex()
    {
        return _VertexIndex.data();
    }

    uint32_t GetVertexIndexCount()
    {
        return _VertexIndexCount;
    }

    uint32_t GetTextureCount()
    {
        return _Texture.size();
    }
    
    float* GetColor()
    {
        return _FloatColors.data();
    }

    uint32_t GetColorCount()
    {
        return _FloatColors.size();
    }

    void* GetAllTexture()
    {
        return _Texture.data();
    }
private:

    struct DrawData
    {
        DrawVert* vtx;     //顶点
        ImDrawIdx* vtxIndex;  //顶点index
        uint32_t vtxIndexBase;
    };

    DrawData BeginDraw()
    {
        DrawData data{ _Vertex.data() + _VertexCount, _VertexIndex.data() + _VertexIndexCount, _VertexCount};
        return data;
    }

    void EndDraw(int idx_count, int vtx_count)
    {
        _VertexCount += vtx_count;
        _VertexIndexCount += idx_count;
    }
    
    uint32_t AddTexture(void* texture)
    {
        for (size_t i = 0; i < _Texture.size(); i++)
        {
            if (_Texture[i] == texture)
            {
                return i;
            }
        }
        _Texture.emplace_back(texture);
        return _Texture.size() - 1;
    }

    uint32_t AddColor(uint32_t col)
    {
        for (size_t i = 0; i < _Colors.size(); i++)
        {
            if (_Colors[i] == col)
            {
                return i;
            }
        }
        _Colors.push_back(col);
        _FloatColors.push_back((uint8_t)((col & 0x00ff0000) >> 16) / 255.f);
        _FloatColors.push_back((uint8_t)((col & 0x0000ff00) >> 8) / 255.f);
        _FloatColors.push_back((uint8_t)(col & 0x000000ff) / 255.f);
        _FloatColors.push_back((uint8_t)((col & 0xff000000) >> 24) / 255.f);
        return _Colors.size() - 1;
    }
private:
    Matrix3X2 _Matrix;
    std::vector<DrawVert>  _Vertex;
    std::vector<ImDrawIdx>  _VertexIndex;
    std::vector<void*>  _Texture;
    std::vector<uint32_t> _Colors;
    std::vector<float> _FloatColors;
    ImVec4 clipRect;
    uint32_t _VertexCount;
    uint32_t _VertexIndexCount;
    float _FringeScale;
};

/*
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }

// - Misc maths helpers
static inline ImVec2 ImMin(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImClamp(const ImVec2& v, const ImVec2& mn, ImVec2 mx) { return ImVec2((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, float t) { return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, const ImVec2& t) { return ImVec2(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t) { return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline float  ImSaturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline float  ImLengthSqr(const ImVec2& lhs) { return (lhs.x * lhs.x) + (lhs.y * lhs.y); }
static inline float  ImLengthSqr(const ImVec4& lhs) { return (lhs.x * lhs.x) + (lhs.y * lhs.y) + (lhs.z * lhs.z) + (lhs.w * lhs.w); }
static inline float  ImInvLength(const ImVec2& lhs, float fail_value) { float d = (lhs.x * lhs.x) + (lhs.y * lhs.y); if (d > 0.0f) return ImRsqrt(d); return fail_value; }
static inline float  ImFloor(float f) { return (float)(int)(f); }
static inline float  ImFloorSigned(float f) { return (float)((f >= 0 || (float)(int)f == f) ? (int)f : (int)f - 1); } // Decent replacement for floorf()
static inline ImVec2 ImFloor(const ImVec2& v) { return ImVec2((float)(int)(v.x), (float)(int)(v.y)); }
static inline ImVec2 ImFloorSigned(const ImVec2& v) { return ImVec2(ImFloorSigned(v.x), ImFloorSigned(v.y)); }
static inline int    ImModPositive(int a, int b) { return (a + b) % b; }
static inline float  ImDot(const ImVec2& a, const ImVec2& b) { return a.x * b.x + a.y * b.y; }
static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a) { return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }
static inline float  ImLinearSweep(float current, float target, float speed) { if (current < target) return ImMin(current + speed, target); if (current > target) return ImMax(current - speed, target); return current; }
static inline ImVec2 ImMul(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline bool   ImIsFloatAboveGuaranteedIntegerPrecision(float f) { return f <= -16777216 || f >= 16777216; }
*/