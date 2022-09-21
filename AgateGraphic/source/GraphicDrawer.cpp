#include "GraphicDrawer.h"
#include "GraphicMath.h"


const float  CircleSegmentMaxError = 0.3f;      // Number of circle segments to use per pixel of radius for AddCircle() etc
const float  CurveTessellationTol = 1.25f;      // Tessellation tolerance when using PathBezierCurveTo()

DrawerList::DrawerList() :_VertexCount{}, 
_VertexIndexCount{}
{
	_Vertex.reserve(5000);
	_VertexIndex.reserve(5000);
    _FringeScale = 1.0f;
    _Colors.reserve(250);
    _FloatColors.reserve(1000);
    _Colors.push_back(0xFFFFFF);
    _FloatColors.resize(4, 1.0f);
    _Matrix = Matrix3X2::Identity();
}

void DrawerList::SetTransform(const Matrix3X2& matrix)
{
    _Matrix = matrix;
}

void DrawerList::AddLineEx(const ImVec2& p1, const ImVec2& p2, uint32_t col, float thickness)
{;
    float cos = p2.x - p1.x;
    float sin = p2.y - p1.y;
    IM_NORMALIZE2F_OVER_ZERO(cos, sin);
    sin = -sin;
    auto drawData = BeginDraw();
    ImVec2 normalThicknessOut(thickness / 2.0f * sin, thickness / 2.0f * cos);
    thickness -= 1.0f;
    ImVec2 normalThickness(thickness / 2.0f * sin, thickness / 2.0f * cos);
    const uint32_t col_trans = AddColor(col & ~IM_COL32_A_MASK);
    col = AddColor(col);

    drawData.vtx[0].pos = p1 + normalThicknessOut;
    drawData.vtx[0].col = col_trans;

    drawData.vtx[1].pos = p2 + normalThicknessOut;
    drawData.vtx[1].col = col_trans;

    drawData.vtx[2].pos = p1 + normalThickness;
    drawData.vtx[2].col = col;

    drawData.vtx[3].pos = p2  + normalThickness;
    drawData.vtx[3].col = col;

    drawData.vtx[4].pos = p1 - normalThickness;
    drawData.vtx[4].col = col;

    drawData.vtx[5].pos = p2 - normalThickness;
    drawData.vtx[5].col = col;

    drawData.vtx[6].pos = p1 - normalThicknessOut;
    drawData.vtx[6].col = col_trans;

    drawData.vtx[7].pos = p2 - normalThicknessOut;
    drawData.vtx[7].col = col_trans;

    auto base = drawData.vtxIndexBase;
    drawData.vtxIndex[0] = base + 0; drawData.vtxIndex[1] = base + 1; drawData.vtxIndex[2] = base + 2;
    drawData.vtxIndex[3] = base + 1; drawData.vtxIndex[4] = base + 2; drawData.vtxIndex[5] = base + 3;
    drawData.vtxIndex[6] = base + 2; drawData.vtxIndex[7] = base + 3; drawData.vtxIndex[8] = base + 4;
    drawData.vtxIndex[9] = base + 3; drawData.vtxIndex[10] = base + 4; drawData.vtxIndex[11] = base + 5;
    drawData.vtxIndex[12] = base + 4; drawData.vtxIndex[13] = base + 5; drawData.vtxIndex[14] = base + 6;
    drawData.vtxIndex[15] = base + 5; drawData.vtxIndex[16] = base + 6; drawData.vtxIndex[17] = base + 7;
    EndDraw(18, 8);
}

void DrawerList::AddLine(const ImVec2& p1, const ImVec2& p2, uint32_t col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathSink sink;
    sink.AddLine(p1);
    sink.AddLine(p2);
    AddPathStroke(sink, col, false, thickness);
}

void DrawerList::AddRect(const ImVec2& p_min, const ImVec2& p_max, uint32_t col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathSink sink;
    sink.AddRect(p_min, p_max);
    AddPathStroke(sink, col, true, thickness);
}

void DrawerList::AddRoundRect(const ImVec2& p_min, const ImVec2& p_max, uint32_t col, float rounding, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathSink sink;
    sink.AddRoundRect(p_min, p_max, rounding);
    AddPathStroke(sink, col, true, thickness);
}

void DrawerList::AddRectFilled(const ImVec2& a, const ImVec2& c, const Brush& brush)
{
    if ((brush._Color & IM_COL32_A_MASK) == 0)
        return;
    PathSink sink;
    sink.AddRect(a, c);
    AddPathFilled(sink, brush._Color);

    /*
    DrawData data = BeginDraw();
    uint32_t textId = 0;
    if (brush._Texture)
    {
        textId = AddTexture(brush._Texture);
    }
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv, tex(textId, 0);
    ImDrawIdx idx = data.vtxIndexBase;
    data.vtxIndex[0] = idx; data.vtxIndex[1] = (idx + 1); data.vtxIndex[2] = (idx + 2);
    data.vtxIndex[3] = idx; data.vtxIndex[4] = (idx + 2); data.vtxIndex[5] = (idx + 3);

    data.vtx[0].pos = a; data.vtx[0].uv = { 0,0 }; data.vtx[0].col = brush._Color; data.vtx[0].tex = tex;
    data.vtx[1].pos = b; data.vtx[1].uv = {1.0, 0}; data.vtx[1].col = brush._Color; data.vtx[1].tex = tex;
    data.vtx[2].pos = c; data.vtx[2].uv = {1.0, 1.0}; data.vtx[2].col = brush._Color; data.vtx[2].tex = tex;
    data.vtx[3].pos = d; data.vtx[3].uv = {0, 1.0}; data.vtx[3].col = brush._Color; data.vtx[3].tex = tex;

    EndDraw(6, 4);
    */
}

void DrawerList::AddRoundRectFilled(const ImVec2& p_min, const ImVec2& p_max, uint32_t col, float rounding)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathSink sink;
    sink.AddRoundRect(p_min, p_max, rounding);
    AddPathFilled(sink, col);
}

void DrawerList::AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, uint32_t col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathSink sink;
    sink.AddLine(p1);
    sink.AddLine(p2);
    sink.AddLine(p3);
    AddPathStroke(sink, col, true, thickness);
}

void DrawerList::AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, uint32_t col)
{
    PathSink sink;
    sink._Data.emplace_back(p1);
    sink._Data.emplace_back(p2);
    sink._Data.emplace_back(p3);
    AddPathFilled(sink, col);
}

void DrawerList::AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, uint32_t col, uint32_t flag)
{
    PathSink sink;
    sink._Data.emplace_back(p1);
    sink._Data.emplace_back(p2);
    sink._Data.emplace_back(p3);
    AddPathFilled(sink, col, flag);
}

void DrawerList::AddPathStroke(PathSink& path, uint32_t col, bool close, float thickness)
{
    path.ApplyTransition(_Matrix);
    thickness *= _Matrix.GetScale();
    AddPolyline(path._Data.data(), path._Data.size(), col, close, thickness);
}

void DrawerList::AddPathFilled(const PathSink& path, uint32_t col, uint32_t flag)
{
    if (path._Data.size() < 3)
        return;

    const ImVec2 uv;// = _Data->TexUvWhitePixel;
    auto  points_count = path._Data.size();

    // Anti-aliased Fill
    const float AA_SIZE = _FringeScale;
    const uint32_t col_trans = AddColor(col & ~IM_COL32_A_MASK);
    col = AddColor(col);

    auto drawData = BeginDraw();
    auto _IdxWritePtr = drawData.vtxIndex;
    auto _VtxWritePtr = drawData.vtx;
    // Add indexes for fill
    unsigned int vtx_inner_idx = drawData.vtxIndexBase;
    unsigned int vtx_outer_idx = drawData.vtxIndexBase + 1;
    for (int i = 2; i < points_count; i++)
    {
        _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
        _IdxWritePtr += 3;
    }

    // Compute normals
    ImVec2* temp_normals = (ImVec2*)_malloca(points_count * sizeof(ImVec2));
    uint32_t mask = 1;
    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
    {
        if ((flag & mask))
        {
            const ImVec2& p0 = path._Data[i0];
            const ImVec2& p1 = path._Data[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }
        else
        {
            temp_normals[i0].x = 0.0f;
            temp_normals[i0].y = 0.0f;
        }
        mask = mask << 1;
    }

    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
    {
        // Average normals
        const ImVec2& n0 = temp_normals[i0];
        const ImVec2& n1 = temp_normals[i1];
        float dm_x = (n0.x + n1.x) * 0.5f;
        float dm_y = (n0.y + n1.y) * 0.5f;
        IM_FIXNORMAL2F(dm_x, dm_y);
        dm_x *= AA_SIZE * 0.5f;
        dm_y *= AA_SIZE * 0.5f;

        // Add vertices
        _VtxWritePtr[0].pos.x = (path._Data[i1].x - dm_x); _VtxWritePtr[0].pos.y = (path._Data[i1].y - dm_y); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
        _VtxWritePtr[1].pos.x = (path._Data[i1].x + dm_x); _VtxWritePtr[1].pos.y = (path._Data[i1].y + dm_y); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
        _VtxWritePtr += 2;

        // Add indexes for fringes
        _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
        _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
        _IdxWritePtr += 6;
    }
    const int idx_count = (points_count - 2) * 3 + points_count * 6;
    const int vtx_count = (points_count * 2);
    EndDraw(idx_count, vtx_count);
}

void DrawerList::AddPathFilled(PathSink& path, uint32_t col)
{
    path.ApplyTransition(_Matrix);
    if (path._Data.size() < 3)
        return;

    const ImVec2 uv;// = _Data->TexUvWhitePixel;
    auto  points_count = path._Data.size();

        // Anti-aliased Fill
    const float AA_SIZE = _FringeScale;
    const uint32_t col_trans = AddColor(col &~IM_COL32_A_MASK);
    col = AddColor(col);

    auto drawData = BeginDraw();
    auto _IdxWritePtr = drawData.vtxIndex;
    auto _VtxWritePtr = drawData.vtx;
    // Add indexes for fill
    unsigned int vtx_inner_idx = drawData.vtxIndexBase;
    unsigned int vtx_outer_idx = drawData.vtxIndexBase + 1;
    for (int i = 2; i < points_count; i++)
    {
        _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
        _IdxWritePtr += 3;
    }

    // Compute normals
    ImVec2* temp_normals = (ImVec2*)_malloca(points_count * sizeof(ImVec2)); 
    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
    {
        const ImVec2& p0 = path._Data[i0];
        const ImVec2& p1 = path._Data[i1];
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        IM_NORMALIZE2F_OVER_ZERO(dx, dy);
        temp_normals[i0].x = dy;
        temp_normals[i0].y = -dx;
    }

    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
    {
        // Average normals
        const ImVec2& n0 = temp_normals[i0];
        const ImVec2& n1 = temp_normals[i1];
        float dm_x = (n0.x + n1.x) * 0.5f;
        float dm_y = (n0.y + n1.y) * 0.5f;
        IM_FIXNORMAL2F(dm_x, dm_y);
        dm_x *= AA_SIZE * 0.5f;
        dm_y *= AA_SIZE * 0.5f;

        // Add vertices
        _VtxWritePtr[0].pos.x = (path._Data[i1].x - dm_x); _VtxWritePtr[0].pos.y = (path._Data[i1].y - dm_y); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
        _VtxWritePtr[1].pos.x = (path._Data[i1].x + dm_x); _VtxWritePtr[1].pos.y = (path._Data[i1].y + dm_y); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
        _VtxWritePtr += 2;

        // Add indexes for fringes
        _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
        _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
        _IdxWritePtr += 6;
    }
    const int idx_count = (points_count - 2) * 3 + points_count * 6;
    const int vtx_count = (points_count * 2);
    EndDraw(idx_count, vtx_count);

}

void DrawerList::AddPolyline(const ImVec2* points, int points_count, uint32_t col, bool closed, float thickness)
{
    if (points_count < 2)
        return;
    const ImVec2 opaque_uv;// = _Data->TexUvWhitePixel;
    const int count = closed ? points_count : points_count - 1; // The number of line segments we need to draw
    const bool thick_line = (thickness > _FringeScale);

    // Anti-aliased stroke
    const float AA_SIZE = _FringeScale;
    const uint32_t col_trans = AddColor(col &~IM_COL32_A_MASK);
    col = AddColor(col);
    //宽度至少一个像素
    thickness = std::max(thickness, 1.0f);

    const int integer_thickness = (int)thickness;
    const float fractional_thickness = thickness - integer_thickness;

    const bool use_texture = false; //(Flags & ImDrawListFlags_AntiAliasedLinesUseTex) && (integer_thickness < IM_DRAWLIST_TEX_LINES_WIDTH_MAX) && (fractional_thickness <= 0.00001f) && (AA_SIZE == 1.0f);


    const int idx_count = use_texture ? (count * 6) : (thick_line ? count * 18 : count * 12);
    const int vtx_count = use_texture ? (points_count * 2) : (thick_line ? points_count * 4 : points_count * 3);
    auto drawData = BeginDraw();
    ImVec2* temp_normals = (ImVec2*)_malloca(points_count * ((use_texture || !thick_line) ? 3 : 5) * sizeof(ImVec2));
    ImVec2* temp_points = temp_normals + points_count;

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

    if (use_texture || !thick_line)
    {
        const float half_draw_size = use_texture ? ((thickness * 0.5f) + 1) : AA_SIZE;
        if (!closed)
        {
            temp_points[0] = points[0] + temp_normals[0] * half_draw_size;
            temp_points[1] = points[0] - temp_normals[0] * half_draw_size;
            temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * half_draw_size;
            temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * half_draw_size;
        }
        unsigned int idx1 = drawData.vtxIndexBase;
        auto idxWritePtr = drawData.vtxIndex;
        auto vtxWritePtr = drawData.vtx;
        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            const unsigned int idx2 = ((i1 + 1) == points_count) ? drawData.vtxIndexBase : (idx1 + (use_texture ? 2 : 3));

            float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
            float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
            IM_FIXNORMAL2F(dm_x, dm_y);
            dm_x *= half_draw_size;
            dm_y *= half_draw_size;

            // Add temporary vertexes for the outer edges
            ImVec2* out_vtx = &temp_points[i2 * 2];
            out_vtx[0].x = points[i2].x + dm_x;
            out_vtx[0].y = points[i2].y + dm_y;
            out_vtx[1].x = points[i2].x - dm_x;
            out_vtx[1].y = points[i2].y - dm_y;
            
            if (use_texture)
            {
                // Add indices for two triangles
                idxWritePtr[0] = (ImDrawIdx)(idx2 + 0); idxWritePtr[1] = (ImDrawIdx)(idx1 + 0); idxWritePtr[2] = (ImDrawIdx)(idx1 + 1); // Right tri
                idxWritePtr[3] = (ImDrawIdx)(idx2 + 1); idxWritePtr[4] = (ImDrawIdx)(idx1 + 1); idxWritePtr[5] = (ImDrawIdx)(idx2 + 0); // Left tri
                idxWritePtr += 6;
            }
            else
            {
                // Add indexes for four triangles
                idxWritePtr[0] = (ImDrawIdx)(idx2 + 0); idxWritePtr[1] = (ImDrawIdx)(idx1 + 0); idxWritePtr[2] = (ImDrawIdx)(idx1 + 2); // Right tri 1
                idxWritePtr[3] = (ImDrawIdx)(idx1 + 2); idxWritePtr[4] = (ImDrawIdx)(idx2 + 2); idxWritePtr[5] = (ImDrawIdx)(idx2 + 0); // Right tri 2
                idxWritePtr[6] = (ImDrawIdx)(idx2 + 1); idxWritePtr[7] = (ImDrawIdx)(idx1 + 1); idxWritePtr[8] = (ImDrawIdx)(idx1 + 0); // Left tri 1
                idxWritePtr[9] = (ImDrawIdx)(idx1 + 0); idxWritePtr[10] = (ImDrawIdx)(idx2 + 0); idxWritePtr[11] = (ImDrawIdx)(idx2 + 1); // Left tri 2
                idxWritePtr += 12;
            }

            idx1 = idx2;
        }

        // Add vertexes for each point on the line
        if (use_texture)
        {
            /*
            ImVec4 tex_uvs = _Data->TexUvLines[integer_thickness];
            ImVec2 tex_uv0(tex_uvs.x, tex_uvs.y);
            ImVec2 tex_uv1(tex_uvs.z, tex_uvs.w);
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i * 2 + 0]; _VtxWritePtr[0].uv = tex_uv0; _VtxWritePtr[0].col = col; // Left-side outer edge
                _VtxWritePtr[1].pos = temp_points[i * 2 + 1]; _VtxWritePtr[1].uv = tex_uv1; _VtxWritePtr[1].col = col; // Right-side outer edge
                _VtxWritePtr += 2;
            }
            */
        }
        else
        {
            for (int i = 0; i < points_count; i++)
            {
                vtxWritePtr[0].pos = points[i];              vtxWritePtr[0].uv = opaque_uv; vtxWritePtr[0].col = col;       // Center of line
                vtxWritePtr[1].pos = temp_points[i * 2 + 0]; vtxWritePtr[1].uv = opaque_uv; vtxWritePtr[1].col = col_trans; // Left-side outer edge
                vtxWritePtr[2].pos = temp_points[i * 2 + 1]; vtxWritePtr[2].uv = opaque_uv; vtxWritePtr[2].col = col_trans; // Right-side outer edge
                vtxWritePtr += 3;
            }
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

        unsigned int idx1 = drawData.vtxIndexBase;
        auto idxWritePtr = drawData.vtxIndex;
        auto vtxWritePtr = drawData.vtx;
        for (int i1 = 0; i1 < count; i1++) // i1 is the first point of the line segment
        {
            const int i2 = (i1 + 1) == points_count ? 0 : (i1 + 1); // i2 is the second point of the line segment
            const unsigned int idx2 = (i1 + 1) == points_count ? drawData.vtxIndexBase : (idx1 + 4); // Vertex index for end of segment

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
            ImVec2* out_vtx = &temp_points[i2 * 4];
            out_vtx[0].x = points[i2].x + dm_out_x;
            out_vtx[0].y = points[i2].y + dm_out_y;
            out_vtx[1].x = points[i2].x + dm_in_x;
            out_vtx[1].y = points[i2].y + dm_in_y;
            out_vtx[2].x = points[i2].x - dm_in_x;
            out_vtx[2].y = points[i2].y - dm_in_y;
            out_vtx[3].x = points[i2].x - dm_out_x;
            out_vtx[3].y = points[i2].y - dm_out_y;


            // Add indexes
            idxWritePtr[0] = (ImDrawIdx)(idx2 + 1); idxWritePtr[1] = (ImDrawIdx)(idx1 + 1); idxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
            idxWritePtr[3] = (ImDrawIdx)(idx1 + 2); idxWritePtr[4] = (ImDrawIdx)(idx2 + 2); idxWritePtr[5] = (ImDrawIdx)(idx2 + 1);
            idxWritePtr[6] = (ImDrawIdx)(idx2 + 1); idxWritePtr[7] = (ImDrawIdx)(idx1 + 1); idxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
            idxWritePtr[9] = (ImDrawIdx)(idx1 + 0); idxWritePtr[10] = (ImDrawIdx)(idx2 + 0); idxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
            idxWritePtr[12] = (ImDrawIdx)(idx2 + 2); idxWritePtr[13] = (ImDrawIdx)(idx1 + 2); idxWritePtr[14] = (ImDrawIdx)(idx1 + 3);
            idxWritePtr[15] = (ImDrawIdx)(idx1 + 3); idxWritePtr[16] = (ImDrawIdx)(idx2 + 3); idxWritePtr[17] = (ImDrawIdx)(idx2 + 2);
            idxWritePtr += 18;

            idx1 = idx2;
        }

        // Add vertices
        for (int i = 0; i < points_count; i++)
        {
            vtxWritePtr[0].pos = temp_points[i * 4 + 0]; vtxWritePtr[0].uv = opaque_uv; vtxWritePtr[0].col = col_trans;
            vtxWritePtr[1].pos = temp_points[i * 4 + 1]; vtxWritePtr[1].uv = opaque_uv; vtxWritePtr[1].col = col;
            vtxWritePtr[2].pos = temp_points[i * 4 + 2]; vtxWritePtr[2].uv = opaque_uv; vtxWritePtr[2].col = col;
            vtxWritePtr[3].pos = temp_points[i * 4 + 3]; vtxWritePtr[3].uv = opaque_uv; vtxWritePtr[3].col = col_trans;
            vtxWritePtr += 4;
        }
    }
    EndDraw(idx_count, vtx_count);
}

void DrawerList::AddImage(const ImVec2& p_min, const ImVec2& p_max, void* textID)
{
    int id = AddTexture(textID);
}

void PathSink::ApplyTransition(const Matrix3X2& matrix)
{
    _Data.clear();
    for (auto& cmd : _PathCmd)
    {
        switch (cmd.type)
        {
        case PathCmdType::Line:
            AddLine(cmd.data.Line.pos, matrix);
            break;
        case PathCmdType::Rect:
            AddRect(cmd.data.Rect.rect_min, cmd.data.Rect.rect_max, matrix);
            break;
        case PathCmdType::RoundRect:
            AddRoundRect(cmd.data.RoundRect.rect_min, cmd.data.RoundRect.rect_max, cmd.data.RoundRect.rounding, matrix);
            break;
        case PathCmdType::Ellipse:
            AddEllipse(cmd.data.Ellipse.center, cmd.data.Ellipse.radiusX, cmd.data.Ellipse.radiusY, matrix);
            break;
        case PathCmdType::Arc:
            AddArc(cmd.data.Arc.center, cmd.data.Arc.radius, cmd.data.Arc.a_min, cmd.data.Arc.a_max, cmd.data.Arc.num_segments, matrix);
            break;
        case PathCmdType::CubicCurve:
            AddBezierCubicCurve(cmd.data.CubicCurve.p2, cmd.data.CubicCurve.p3, cmd.data.CubicCurve.p4, cmd.data.CubicCurve.num_segments, matrix);
            break;
        case PathCmdType::QuadraticCurve:
            AddBezierQuadraticCurve(cmd.data.QuadraticCurve.p2, cmd.data.QuadraticCurve.p3, cmd.data.QuadraticCurve.num_segments, matrix);
            break;
        default:
            break;
        };
    }
}

void PathSink::AddLine(const ImVec2& pos)
{
    PathData data;
    PathCmd cmd;
    cmd.type = PathCmdType::Line;
    cmd.data.Line.pos = pos;
    _PathCmd.emplace_back(cmd);
}

void PathSink::AddRect(const ImVec2& rect_min, const ImVec2& rect_max)
{
    PathCmd cmd;
    cmd.type = PathCmdType::Rect;
    cmd.data.Rect.rect_min = rect_min;
    cmd.data.Rect.rect_max = rect_max;
    _PathCmd.emplace_back(cmd);
}

void PathSink::AddRoundRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding)
{
    PathCmd cmd;
    cmd.type = PathCmdType::RoundRect;
    cmd.data.RoundRect.rect_min = rect_min;
    cmd.data.RoundRect.rect_max = rect_max;
    cmd.data.RoundRect.rounding = rounding;
    _PathCmd.emplace_back(cmd);
}

void PathSink::AddEllipse(const ImVec2& center, float radiusX, float radiusY)
{
    PathCmd cmd;
    cmd.type = PathCmdType::Ellipse;
    cmd.data.Ellipse.center = center;
    cmd.data.Ellipse.radiusX = radiusX;
    cmd.data.Ellipse.radiusY = radiusY;
    _PathCmd.emplace_back(cmd);
}


void PathSink::AddArc(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{
    PathCmd cmd;
    cmd.type = PathCmdType::Arc;
    cmd.data.Arc.center = center;
    cmd.data.Arc.radius = radius;
    cmd.data.Arc.a_min = a_min;
    cmd.data.Arc.a_max = a_max;
    cmd.data.Arc.num_segments = num_segments;
    _PathCmd.emplace_back(cmd);
}

ImVec2 ImBezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
{
    float u = 1.0f - t;
    float w1 = u * u * u;
    float w2 = 3 * u * u * t;
    float w3 = 3 * u * t * t;
    float w4 = t * t * t;
    return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y);
}

ImVec2 ImBezierQuadraticCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float t)
{
    float u = 1.0f - t;
    float w1 = u * u;
    float w2 = 2 * u * t;
    float w3 = t * t;
    return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x, w1 * p1.y + w2 * p2.y + w3 * p3.y);
}

void PathSink::AddBezierCubicCurve(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    PathCmd cmd;
    cmd.type = PathCmdType::CubicCurve;
    cmd.data.CubicCurve.p2 = p2;
    cmd.data.CubicCurve.p3 = p3;
    cmd.data.CubicCurve.p4 = p4;
    cmd.data.CubicCurve.num_segments = num_segments;
    _PathCmd.emplace_back(cmd);
}

void PathSink::AddBezierQuadraticCurve(const ImVec2& p2, const ImVec2& p3, int num_segments)
{
    PathCmd cmd;
    cmd.type = PathCmdType::QuadraticCurve;
    cmd.data.QuadraticCurve.p2 = p2;
    cmd.data.QuadraticCurve.p3 = p3;
    cmd.data.QuadraticCurve.num_segments = num_segments;
    _PathCmd.emplace_back(cmd);
}

void PathSink::AddRect(const ImVec2& rect_min, const ImVec2& rect_max, const Matrix3X2& matrix)
{
    AddLine(rect_min, matrix);
    AddLine(ImVec2(rect_max.x, rect_min.y), matrix);
    AddLine(rect_max, matrix);
    AddLine(ImVec2(rect_min.x, rect_max.y), matrix);
}

void PathSink::AddRoundRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding, const Matrix3X2& matrix)
{
    if (rounding < 0.05)
    {
        AddRect(rect_min, rect_max, matrix);
    }
    else
    {
        auto rotation = matrix.GetRotation();
        if (abs(rotation) < 0.001)
        {
            AddArcFast(ImVec2(rect_min.x + rounding, rect_min.y + rounding), rounding, 6, 9);
            AddArcFast(ImVec2(rect_max.x - rounding, rect_min.y + rounding), rounding, 9, 12);
            AddArcFast(ImVec2(rect_max.x - rounding, rect_max.y - rounding), rounding, 0, 3);
            AddArcFast(ImVec2(rect_min.x + rounding, rect_max.y - rounding), rounding, 3, 6);
        }
        else
        {
            auto halfPI = IM_PI / 2.0f;
            AddArc(ImVec2(rect_min.x + rounding, rect_min.y + rounding), rounding, IM_PI, IM_PI + halfPI, 0, matrix );
            AddArc(ImVec2(rect_max.x - rounding, rect_min.y + rounding), rounding, IM_PI + halfPI, IM_PI + IM_PI,0, matrix);
            AddArc(ImVec2(rect_max.x - rounding, rect_max.y - rounding), rounding, 0, halfPI, 0, matrix);
            AddArc(ImVec2(rect_min.x + rounding, rect_max.y - rounding), rounding, halfPI, IM_PI, 0, matrix);

        }
    }
}

void PathSink::AddEllipse(ImVec2 center, float rx, float ry, const Matrix3X2& matrix)
{
    const float Bezier90 = 0.5522847493f;   //90度的贝塞尔曲线
    AddLine({ center.x + rx, center.y }, matrix);
    AddBezierCubicCurve({ center.x + rx, center.y + ry * Bezier90 }, { center.x + rx * Bezier90, center.y + ry }, { center.x, center.y + ry }, 0, matrix);
    AddBezierCubicCurve({ center.x - rx * Bezier90, center.y + ry }, { center.x - rx, center.y + ry * Bezier90 }, { center.x - rx, center.y }, 0, matrix);
    AddBezierCubicCurve({ center.x - rx, center.y - ry * Bezier90 }, { center.x - rx * Bezier90, center.y - ry }, { center.x, center.y - ry }, 0, matrix);
    AddBezierCubicCurve({ center.x + rx * Bezier90, center.y - ry }, { center.x + rx, center.y - ry * Bezier90 }, { center.x + rx, center.y }, 0, matrix);
}

void PathSink::AddArc(ImVec2 center, float radius, float a_min, float a_max, int num_segments, const Matrix3X2& matrix)
{
    if (radius <= 0.0f)
    {
        _Data.push_back(center);
        return;
    }
    radius *= matrix.GetScale();
    center = matrix.TransformPoint(center);
    float rota = matrix.GetRotation();
    a_min += rota;
    a_max += rota;
    if (num_segments > 0)
    {
        AddArcToN(center, radius, a_min, a_max, num_segments);
        return;
    }
    // Automatic segment count
    if (radius <= DrawShareData::ShareData().ArcFastRadiusCutoff)
    {
        const bool a_is_reverse = a_max < a_min;

        // We are going to use precomputed values for mid samples.
        // Determine first and last sample in lookup table that belong to the arc.
        const float a_min_sample_f = IM_DRAWLIST_ARCFAST_SAMPLE_MAX * a_min / (IM_PI * 2.0f);
        const float a_max_sample_f = IM_DRAWLIST_ARCFAST_SAMPLE_MAX * a_max / (IM_PI * 2.0f);

        const int a_min_sample = a_is_reverse ? (int)ImFloorSigned(a_min_sample_f) : (int)ImCeil(a_min_sample_f);
        const int a_max_sample = a_is_reverse ? (int)ImCeil(a_max_sample_f) : (int)ImFloorSigned(a_max_sample_f);
        const int a_mid_samples = a_is_reverse ? ImMax(a_min_sample - a_max_sample, 0) : ImMax(a_max_sample - a_min_sample, 0);

        const float a_min_segment_angle = a_min_sample * IM_PI * 2.0f / IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        const float a_max_segment_angle = a_max_sample * IM_PI * 2.0f / IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        const bool a_emit_start = ImAbs(a_min_segment_angle - a_min) >= 1e-5f;
        const bool a_emit_end = ImAbs(a_max - a_max_segment_angle) >= 1e-5f;

        _Data.reserve(_Data.size() + (a_mid_samples + 1 + (a_emit_start ? 1 : 0) + (a_emit_end ? 1 : 0)));
        if (a_emit_start)
            _Data.push_back(ImVec2(center.x + ImCos(a_min) * radius, center.y + ImSin(a_min) * radius));
        if (a_mid_samples > 0)
            AddArcToFastEx(center, radius, a_min_sample, a_max_sample, 0);
        if (a_emit_end)
            _Data.push_back(ImVec2(center.x + ImCos(a_max) * radius, center.y + ImSin(a_max) * radius));
    }
    else
    {
        const float arc_length = ImAbs(a_max - a_min);
        const int circle_segment_count = CalcCircleAutoSegmentCount(radius);
        const int arc_segment_count = ImMax((int)ImCeil(circle_segment_count * arc_length / (IM_PI * 2.0f)), (int)(2.0f * IM_PI / arc_length));
        AddArcToN(center, radius, a_min, a_max, arc_segment_count);
    }
}

void PathSink::AddArcFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12)
{
    if (radius <= 0.0f)
    {
        _Data.push_back(center);
        return;
    }
    AddArcToFastEx(center, radius, a_min_of_12 * IM_DRAWLIST_ARCFAST_SAMPLE_MAX / 12, a_max_of_12 * IM_DRAWLIST_ARCFAST_SAMPLE_MAX / 12, 0);
}

void PathSink::AddBezierCubicCurve(ImVec2 p2, ImVec2 p3, ImVec2 p4, int num_segments, const Matrix3X2& matrix)
{
    ImVec2 p1 = _Data.back();
    p2 = matrix.TransformPoint(p2);
    p3 = matrix.TransformPoint(p3);
    p4 = matrix.TransformPoint(p4);
    if (num_segments == 0)
    {
        PathBezierCubicCurveToCasteljau(p1.x, p1.y, p2.x, p2.y, 
            p3.x, p3.y, p4.x, p4.y, CurveTessellationTol, 0); // Auto-tessellated
    }
    else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
            _Data.emplace_back(ImBezierCubicCalc(p1, p2, p3, p4, t_step * i_step));
    }
}

void PathSink::AddBezierQuadraticCurve(ImVec2 p2, ImVec2 p3, int num_segments, const Matrix3X2& matrix)
{
    ImVec2 p1 = _Data.back();
    p2 = matrix.TransformPoint(p2);
    p3 = matrix.TransformPoint(p3);
    if (num_segments == 0)
    {
        PathBezierQuadraticCurveToCasteljau(p1.x, p1.y, p2.x, p2.y, 
            p3.x, p3.y, CurveTessellationTol, 0);
    }
    else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
            _Data.push_back(ImBezierQuadraticCalc(p1, p2, p3, t_step * i_step));
    }
}

void PathSink::AddArcToFastEx(const ImVec2& center, float radius, int a_min_sample, int a_max_sample, int a_step)
{
    if (radius <= 0.0f)
    {
        _Data.push_back(center);
        return;
    }

    // Calculate arc auto segment step size
    if (a_step <= 0)
        a_step = IM_DRAWLIST_ARCFAST_SAMPLE_MAX / CalcCircleAutoSegmentCount(radius);

    // Make sure we never do steps larger than one quarter of the circle
    a_step = ImClamp(a_step, 1, IM_DRAWLIST_ARCFAST_TABLE_SIZE / 4);

    const int sample_range = ImAbs(a_max_sample - a_min_sample);
    const int a_next_step = a_step;

    int samples = sample_range + 1;
    bool extra_max_sample = false;
    if (a_step > 1)
    {
        samples = sample_range / a_step + 1;
        const int overstep = sample_range % a_step;

        if (overstep > 0)
        {
            extra_max_sample = true;
            samples++;

            // When we have overstep to avoid awkwardly looking one long line and one tiny one at the end,
            // distribute first step range evenly between them by reducing first step size.
            if (sample_range > 0)
                a_step -= (a_step - overstep) / 2;
        }
    }

    _Data.resize(_Data.size() + samples);
    ImVec2* out_ptr = _Data.data() + (_Data.size() - samples);

    int sample_index = a_min_sample;
    if (sample_index < 0 || sample_index >= IM_DRAWLIST_ARCFAST_SAMPLE_MAX)
    {
        sample_index = sample_index % IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        if (sample_index < 0)
            sample_index += IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
    }

    if (a_max_sample >= a_min_sample)
    {
        for (int a = a_min_sample; a <= a_max_sample; a += a_step, sample_index += a_step, a_step = a_next_step)
        {
            // a_step is clamped to IM_DRAWLIST_ARCFAST_SAMPLE_MAX, so we have guaranteed that it will not wrap over range twice or more
            if (sample_index >= IM_DRAWLIST_ARCFAST_SAMPLE_MAX)
                sample_index -= IM_DRAWLIST_ARCFAST_SAMPLE_MAX;

            const ImVec2 s = DrawShareData::ShareData().ArcFastVtx[sample_index];
            out_ptr->x = center.x + s.x * radius;
            out_ptr->y = center.y + s.y * radius;
            out_ptr++;
        }
    }
    else
    {
        for (int a = a_min_sample; a >= a_max_sample; a -= a_step, sample_index -= a_step, a_step = a_next_step)
        {
            // a_step is clamped to IM_DRAWLIST_ARCFAST_SAMPLE_MAX, so we have guaranteed that it will not wrap over range twice or more
            if (sample_index < 0)
                sample_index += IM_DRAWLIST_ARCFAST_SAMPLE_MAX;

            const ImVec2 s = DrawShareData::ShareData().ArcFastVtx[sample_index];
            out_ptr->x = center.x + s.x * radius;
            out_ptr->y = center.y + s.y * radius;
            out_ptr++;
        }
    }

    if (extra_max_sample)
    {
        int normalized_max_sample = a_max_sample % IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        if (normalized_max_sample < 0)
            normalized_max_sample += IM_DRAWLIST_ARCFAST_SAMPLE_MAX;

        const ImVec2 s = DrawShareData::ShareData().ArcFastVtx[normalized_max_sample];
        out_ptr->x = center.x + s.x * radius;
        out_ptr->y = center.y + s.y * radius;
        out_ptr++;
    }
}

void PathSink::AddArcToN(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{
    if (radius <= 0.0f)
    {
        _Data.push_back(center);
        return;
    }

    _Data.reserve(_Data.size() + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        _Data.emplace_back(ImVec2(center.x + ImCos(a) * radius, center.y + ImSin(a) * radius));
    }
}

int PathSink::CalcCircleAutoSegmentCount(float radius) const
{
    auto& shareData = DrawShareData::ShareData();
    const int radius_idx = (int)(radius + 0.999999f); 
    if (radius_idx < IM_ARRAYSIZE(shareData.CircleSegmentCounts))
        return shareData.CircleSegmentCounts[radius_idx]; 
    else
        return IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, CircleSegmentMaxError);
}

void PathSink::PathBezierCubicCurveToCasteljau(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = (x2 - x4) * dy - (y2 - y4) * dx;
    float d3 = (x3 - x4) * dy - (y3 - y4) * dx;
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
    {
        _Data.emplace_back(ImVec2(x4, y4));
    }
    else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
        float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;
        PathBezierCubicCurveToCasteljau(x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        PathBezierCubicCurveToCasteljau(x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}

void PathSink::PathBezierQuadraticCurveToCasteljau(float x1, float y1, float x2, float y2, float x3, float y3, float tess_tol, int level)
{
    float dx = x3 - x1, dy = y3 - y1;
    float det = (x2 - x3) * dy - (y2 - y3) * dx;
    if (det * det * 4.0f < tess_tol * (dx * dx + dy * dy))
    {
        _Data.emplace_back(ImVec2(x3, y3));
    }
    else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        PathBezierQuadraticCurveToCasteljau(x1, y1, x12, y12, x123, y123, tess_tol, level + 1);
        PathBezierQuadraticCurveToCasteljau(x123, y123, x23, y23, x3, y3, tess_tol, level + 1);
    }
}

void PathSink::BuildPath()
{
}

DrawShareData::DrawShareData()
{
    memset(this, 0, sizeof(*this));
    for (int i = 0; i < IM_ARRAYSIZE(ArcFastVtx); i++)
    {
        const float a = ((float)i * 2 * IM_PI) / (float)IM_ARRAYSIZE(ArcFastVtx);
        ArcFastVtx[i] = ImVec2(ImCos(a), ImSin(a));
    }
    ArcFastRadiusCutoff = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(IM_DRAWLIST_ARCFAST_SAMPLE_MAX, CircleSegmentMaxError);

    for (int i = 0; i < IM_ARRAYSIZE(CircleSegmentCounts); i++)
    {
        const float radius = (float)i;
        CircleSegmentCounts[i] = (int)((i > 0) ? IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, CircleSegmentMaxError) : 0);
    }
    ArcFastRadiusCutoff = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(IM_DRAWLIST_ARCFAST_SAMPLE_MAX, CircleSegmentMaxError);
    MaxTexture = 32;
}


DrawShareData& DrawShareData::ShareData()
{
    static DrawShareData data;
    return data;
}
