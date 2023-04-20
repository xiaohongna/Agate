#include "pch.h"
#include "Visual.h"
#include "bezier.h"

namespace agate
{
	void Visual::flatten(std::vector<Vector2>& flattenLines)
	{
        Vector2* tranPpt = (Vector2*)_malloca(_points.size() * sizeof(Vector2));
        auto ppt = _points.data();
        _transition.transformPoint(tranPpt, ppt, _points.size());
        ppt = tranPpt;
        flattenLines.push_back(*ppt);
        for (auto ptt : _lineTypes)
        {
            if (ptt == LineType::Line)
            {
                ppt++;
                flattenLines.push_back(*ppt);
            }
            else
            {
                flattenBezier(ppt, flattenLines);
                ppt += 3;
            }
            _freea(tranPpt);
        }
	}
    void Visual::flattenBezier(Vector2* pPoints, std::vector<Vector2>& flattenLines)
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
                flattenLines.emplace_back(points[i].x / 16.0f, points[i].y / 16.0f);
            }
        }
    }
    void Visual::rasterizeStroke(uint32_t color, std::vector<Vector2>& flattenLines)
    {
        constexpr float FringeScale = 1.0f;
        auto points_count = flattenLines.size();
        if (points_count < 2)
            return;
        _strokeData.index.Reset();
        _StrokeData.vertex.Reset();
        const bool closed = _Flags & Geometry_Flag_Closed;
        const int count = closed ? points_count : points_count - 1;
        const bool thick_line = (_StrokeWidth > FringeScale);

        // Anti-aliased stroke
        const float AA_SIZE = FringeScale;
        const uint32_t col_trans = col & 0xFFFFFF; // ~IM_COL32_A_MASK;
        //宽度至少一个像素
        auto thickness = max(_StrokeWidth, 1.0f);

        const int integer_thickness = (int)thickness;
        const float fractional_thickness = thickness - integer_thickness;

        const int idx_count = thick_line ? count * 18 : count * 12;
        const int vtx_count = thick_line ? points_count * 4 : points_count * 3;

        auto idxWritePtr = _StrokeData.index.Alloc<DrawIndex>(idx_count);
        auto vtxWritePtr = _StrokeData.vertex.Alloc<VertexXYColor>(vtx_count);

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
        _freea(temp_normals);
        _StrokeData.pipline = PipelineType::Color;
        _StrokeData.blend = BlendMode::Blend;
        AddFlag(Geometry_Flag_Stroke_RS);
    }
    void Visual::rasterizeFill(uint32_t color, std::vector<Vector2>& flattenLines)
    {
    }
}