#include "Geometry.h"
#include "bezier.h"

#ifdef ENABLE_SSE
#include <immintrin.h>
static inline float  ImRsqrt(float x) { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
#else
static inline float  ImRsqrt(float x) { return 1.0f / sqrtf(x); }
#endif

#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = ImRsqrt(d2); VX *= inv_len; VY *= inv_len; } } (void)0
#define IM_FIXNORMAL2F_MAX_INVLEN2          100.0f // 500.0f (see #4053, #3366)
#define IM_FIXNORMAL2F(VX,VY)               { float d2 = VX*VX + VY*VY; if (d2 > 0.000001f) { float inv_len2 = 1.0f / d2; if (inv_len2 > IM_FIXNORMAL2F_MAX_INVLEN2) inv_len2 = IM_FIXNORMAL2F_MAX_INVLEN2; VX *= inv_len2; VY *= inv_len2; } } (void)0

#define IM_COL32_A_MASK     0xFF000000


constexpr uint32_t  Frozen_Flag_Mask = 1;

constexpr uint32_t  Closed_Flag_Mask = 1 << 1;


Figure::Figure() : _Closed{}
{
}

Figure::Figure(const Figure& figure)
{
	_Points.assign(figure._Points.begin(), figure._Points.end());
	_Types.assign(figure._Types.begin(), figure._Types.end());
	_Closed = figure._Closed;
}

Figure::Figure(Figure&& figure):_Points{ std::move(figure._Points)}, 
_Types{std::move(figure._Types)},
_Closed{figure._Closed}
{
}

void Figure::Reserve(int ptCount, int typeCount)
{
	_Points.reserve(ptCount);
	_Types.reserve(typeCount);
}

void Figure::StartAt(float x, float y)
{
	assert(IsEmpty());
	_Points.emplace_back(Vector2(x, y));
}

void Figure::StartAt(const Vector2& pt)
{
	StartAt(pt.x, pt.y);
}

void Figure::LineTo(const Vector2& pt)
{
	auto ppt = AddPoints(1);
	auto ptype = AddTypes(1);
	ppt->x = pt.x;
	ppt->y = pt.y;
	*ptype = PointType::Line;
}

void Figure::LineTo(const Vector2* pts, int count)
{
	auto ppt = AddPoints(count);
	auto ptype = AddTypes(count);
	for (size_t i = 0; i < count; i++)
	{
		ppt->x = pts->x;
		ppt->y = pts->y;
		*ptype = PointType::Line;
		ppt++;
		ptype++;
		pts++;
	}
}

void Figure::BezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2& endPt)
{
	auto ppt = AddPoints(3);
	auto ptype = AddTypes(1);
	ppt[0].x = ctr1.x;
	ppt[0].y = ctr1.y;
	ppt[1].x = ctr2.x;
	ppt[1].y = ctr2.y;
	ppt[2].x = endPt.x;
	ppt[2].y = endPt.y;
	*ptype = PointType::Bezier;
}

void Figure::BezierTo(const Vector2* pts, int count)
{
	assert(count % 3 == 0);
	auto bCount = count / 3;
	auto ppt = AddPoints(bCount * 3);
	auto ptype = AddTypes(bCount);
	for (int i = 0; i < bCount; i++)
	{
		ppt[0] = pts[0];
		ppt[1] = pts[1];
		ppt[2] = pts[2];
		*ptype = PointType::Bezier;
		ppt += 3;
		pts += 3;
		ptype++;
	}
	
	*ptype = PointType::Bezier;
}

void Figure::Reset()
{
	_Points.clear();
	_Types.clear();
	_Closed = false;
}

void Figure::Close()
{
	_Closed = true;
}

Vector2* Figure::AddPoints(int count)
{
	auto size = _Points.size();
	_Points.resize(size + count);
	return _Points.data() + size;
}

PointType* Figure::AddTypes(int count)
{
	auto size = _Types.size();
	_Types.resize(size + count);
	return _Types.data() + size;
}

void Geometry::AddFigure(Figure&& figure)
{
	assert((_Flags & Frozen_Flag_Mask) == 0);
	if ((_Flags & Frozen_Flag_Mask) == 0)
	{
		_Figures.emplace_back(std::move(figure));
	}
}

void Geometry::SetStrokeWidth(float width)
{
    assert((_Flags & Frozen_Flag_Mask) == 0);
	_StrokeWidth = width;
}

void Geometry::SetFillBrush(Brush& brush)
{
    assert((_Flags & Frozen_Flag_Mask) == 0);
}

void Geometry::SetStrokeBrush(Brush& brush)
{
    assert((_Flags & Frozen_Flag_Mask) == 0);
}

Vector4 Geometry::GetBounds()
{
	return Vector4();
}

void Geometry::Freeze()
{
}

void Geometry::Flatten()
{
	for (auto& figure :_Figures)
	{
        auto ppt = figure._Points.data();
		for (auto ptt : figure._Types)
		{
			if (ptt == PointType::Line)
			{
				_FlattenLines.push_back(*ppt);
				ppt++;
				_FlattenLines.push_back(*ppt);
			}
			else
			{
				FlattenBezier(ppt);
				ppt += 3;
			}
		}
	}
}

void Geometry::FlattenBezier(Vector2* pPoints)
{
	constexpr int BufferCount = 16;
	POINT points[BufferCount];
	for (int i = 0; i < 4; i++)
	{
		points[i].x = LONG(pPoints[i].x * 16);
		points[i].y = LONG(pPoints[i].y * 16);
	}
	CMILBezier bezier(points, nullptr);
	BOOL haveMore = true;
	while (haveMore)
	{
		auto count = bezier.Flatten(points, BufferCount, &haveMore);
		for (int i = 0; i < count; i++)
		{
			_FlattenLines.emplace_back(Vector2(points[i].x / 16.0f, points[i].y / 16.0f));
		}
	}
}

void Geometry::RasterizeStroke(uint32_t col)
{
    constexpr float FringeScale = 1.0f;
    auto points_count = _FlattenLines.size();
    if (points_count < 2)
        return;
    const bool closed = _Flags & Closed_Flag_Mask;
    const int count = closed ? points_count : points_count - 1; 
    const bool thick_line = (_StrokeWidth > FringeScale);

    // Anti-aliased stroke
    const float AA_SIZE = FringeScale;
    const uint32_t col_trans = col & ~IM_COL32_A_MASK;
    //宽度至少一个像素
    auto thickness = max(_StrokeWidth, 1.0f);

    const int integer_thickness = (int)thickness;
    const float fractional_thickness = thickness - integer_thickness;

    const int idx_count = thick_line ? count * 18 : count * 12;
    const int vtx_count = thick_line ? points_count * 4 : points_count * 3;

    auto idxWritePtr = _StrokeDrawIndex.Alloc<DrawIndex>(idx_count);
    auto vtxWritePtr = _StrokeVertexBuffer.Alloc<VertexXYColor>(vtx_count);

    Vector2* temp_normals = (Vector2*)_malloca(points_count * (thick_line ? 5 : 3) * sizeof(Vector2));
    Vector2* temp_points = temp_normals + points_count;
    Vector2* points = _FlattenLines.data();
    for (int i1 = 0; i1 < count; i1++)
    {
        const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
        float dx = points[i2].x - points[i1].x;
        float dy = points[i2].y - points[i1].y;
        IM_NORMALIZE2F_OVER_ZERO(dx, dy);
        temp_normals[i1].x = dy;
        temp_normals[i1].y = -dx;
    }
    if (!closed)
        temp_normals[points_count - 1] = temp_normals[points_count - 2];

    if (!thick_line)
    {
        const float half_draw_size = AA_SIZE;
        if (!closed)
        {
            temp_points[0] = points[0] + temp_normals[0] * half_draw_size;
            temp_points[1] = points[0] - temp_normals[0] * half_draw_size;
            temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * half_draw_size;
            temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * half_draw_size;
        }
		unsigned int idx1 = 0;
        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
			const unsigned int idx2 = ((i1 + 1) == points_count) ? 0 : (idx1 + 3);

            float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
            float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
            IM_FIXNORMAL2F(dm_x, dm_y);
            dm_x *= half_draw_size;
            dm_y *= half_draw_size;

            // Add temporary vertexes for the outer edges
            auto out_vtx = &temp_points[i2 * 2];
            out_vtx[0].x = points[i2].x + dm_x;
            out_vtx[0].y = points[i2].y + dm_y;
            out_vtx[1].x = points[i2].x - dm_x;
            out_vtx[1].y = points[i2].y - dm_y;

            // Add indexes for four triangles
            idxWritePtr[0] = (DrawIndex)(idx2 + 0); idxWritePtr[1] = (DrawIndex)(idx1 + 0); idxWritePtr[2] = (DrawIndex)(idx1 + 2); // Right tri 1
            idxWritePtr[3] = (DrawIndex)(idx1 + 2); idxWritePtr[4] = (DrawIndex)(idx2 + 2); idxWritePtr[5] = (DrawIndex)(idx2 + 0); // Right tri 2
            idxWritePtr[6] = (DrawIndex)(idx2 + 1); idxWritePtr[7] = (DrawIndex)(idx1 + 1); idxWritePtr[8] = (DrawIndex)(idx1 + 0); // Left tri 1
            idxWritePtr[9] = (DrawIndex)(idx1 + 0); idxWritePtr[10] = (DrawIndex)(idx2 + 0); idxWritePtr[11] = (DrawIndex)(idx2 + 1); // Left tri 2
            idxWritePtr += 12;
            idx1 = idx2;
        }

        for (int i = 0; i < points_count; i++)
        {
            vtxWritePtr[0].pos = points[i];               vtxWritePtr[0].col = col;       // Center of line
            vtxWritePtr[1].pos = temp_points[i * 2 + 0];  vtxWritePtr[1].col = col_trans; // Left-side outer edge
            vtxWritePtr[2].pos = temp_points[i * 2 + 1];  vtxWritePtr[2].col = col_trans; // Right-side outer edge
            vtxWritePtr += 3;
        }
    }
    else
    {
        const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;

        if (!closed)
        {
            const int points_last = points_count - 1;
            temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
            temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
            temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
            temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
            temp_points[points_last * 4 + 0] = points[points_last] + temp_normals[points_last] * (half_inner_thickness + AA_SIZE);
            temp_points[points_last * 4 + 1] = points[points_last] + temp_normals[points_last] * (half_inner_thickness);
            temp_points[points_last * 4 + 2] = points[points_last] - temp_normals[points_last] * (half_inner_thickness);
            temp_points[points_last * 4 + 3] = points[points_last] - temp_normals[points_last] * (half_inner_thickness + AA_SIZE);
        }
		unsigned int idx1 = 0;
        for (int i1 = 0; i1 < count; i1++) // i1 is the first point of the line segment
        {
            const int i2 = (i1 + 1) == points_count ? 0 : (i1 + 1); // i2 is the second point of the line segment
			const unsigned int idx2 = (i1 + 1) == points_count ? 0 : (idx1 + 4); // Vertex index for end of segment

            // Average normals
            float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
            float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
            IM_FIXNORMAL2F(dm_x, dm_y);

            if (dm_x >= 1)
            {
                //dm_x = 1.0f;
               // dm_y = 0.0f;
            }
            if (dm_y <= -1.0f)
            {
                // dm_y = -1.0f;
            }

            float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
            float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
            float dm_in_x = dm_x * half_inner_thickness;
            float dm_in_y = dm_y * half_inner_thickness;

            // Add temporary vertices
            auto* out_vtx = &temp_points[i2 * 4];
            out_vtx[0].x = points[i2].x + dm_out_x;
            out_vtx[0].y = points[i2].y + dm_out_y;
            out_vtx[1].x = points[i2].x + dm_in_x;
            out_vtx[1].y = points[i2].y + dm_in_y;
            out_vtx[2].x = points[i2].x - dm_in_x;
            out_vtx[2].y = points[i2].y - dm_in_y;
            out_vtx[3].x = points[i2].x - dm_out_x;
            out_vtx[3].y = points[i2].y - dm_out_y;


            // Add indexes
            idxWritePtr[0] = (DrawIndex)(idx2 + 1); idxWritePtr[1] = (DrawIndex)(idx1 + 1); idxWritePtr[2] = (DrawIndex)(idx1 + 2);
            idxWritePtr[3] = (DrawIndex)(idx1 + 2); idxWritePtr[4] = (DrawIndex)(idx2 + 2); idxWritePtr[5] = (DrawIndex)(idx2 + 1);
            idxWritePtr[6] = (DrawIndex)(idx2 + 1); idxWritePtr[7] = (DrawIndex)(idx1 + 1); idxWritePtr[8] = (DrawIndex)(idx1 + 0);
            idxWritePtr[9] = (DrawIndex)(idx1 + 0); idxWritePtr[10] = (DrawIndex)(idx2 + 0); idxWritePtr[11] = (DrawIndex)(idx2 + 1);
            idxWritePtr[12] = (DrawIndex)(idx2 + 2); idxWritePtr[13] = (DrawIndex)(idx1 + 2); idxWritePtr[14] = (DrawIndex)(idx1 + 3);
            idxWritePtr[15] = (DrawIndex)(idx1 + 3); idxWritePtr[16] = (DrawIndex)(idx2 + 3); idxWritePtr[17] = (DrawIndex)(idx2 + 2);
            idxWritePtr += 18;

            idx1 = idx2;
        }

        // Add vertices
        for (int i = 0; i < points_count; i++)
        {
            vtxWritePtr[0].pos = temp_points[i * 4 + 0]; vtxWritePtr[0].col = col_trans;
            vtxWritePtr[1].pos = temp_points[i * 4 + 1]; vtxWritePtr[1].col = col;
            vtxWritePtr[2].pos = temp_points[i * 4 + 2]; vtxWritePtr[2].col = col;
            vtxWritePtr[3].pos = temp_points[i * 4 + 3]; vtxWritePtr[3].col = col_trans;
            vtxWritePtr += 4;
        }
    }
	_RasterizeData[0].pipline = PiplineType::Color;
	_RasterizeData[0].blend = BlendMode::Blend;
	_RasterizeData[0].indexBuffer = _StrokeDrawIndex.buffer;
	_RasterizeData[0].indexCount = _StrokeDrawIndex.count;
	_RasterizeData[0].vertexBuffer = _StrokeVertexBuffer.buffer;
	_RasterizeData[0].vertexCount = _StrokeVertexBuffer.count;
}

uint32_t Geometry::Rasterize()
{
	Flatten();
	return 1;
}

const RasterizeData& Geometry::GetRasterizeData(uint32_t index)
{
	assert(index < 2);
	return _RasterizeData[index];
	// TODO: 在此处插入 return 语句
}
