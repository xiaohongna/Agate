// AgateGraphic.cpp : 定义静态库的函数。
//

#include "framework.h"

// TODO: 这是一个库函数示例
void fnAgateGraphic()
{
}

POINT Vec2ToPoint(const ImVec2& p)
{
    POINT r{ p.x * 16, p.y * 16 };
    return r;
}

ImVec2 PointToVec2(const POINT& pt)
{
    return ImVec2(pt.x / 16.0f, pt.y / 16.0f);
}

ImVector<ImVec2> BezierTest(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4)
{
    ImVector<ImVec2> result;
    result.push_back(p1);
    POINT input[4];
    input[0] = Vec2ToPoint(p1);
    input[1] = Vec2ToPoint(p2);
    input[2] = Vec2ToPoint(p3);
    input[3] = Vec2ToPoint(p4);
    RECT clip{ input[0].x, input[0].y, input[0].x + 16, input[0].y + 16 };
    CMILBezier bezier(input, &clip);
    BOOL haveMore = true;
    while (haveMore)
    {
        auto count = bezier.Flatten(input, 4, &haveMore);
        for (int i = 0; i < count; i++)
        {
            result.push_back(PointToVec2(input[i]));
        }
    }
    return result;
}

void ImGui::CustomRender(const char* fmt)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ImVec2 pos = window->DC.CursorPos;
    ImRect bb(pos, pos + ImVec2(110.0f, 110.f));
    ItemSize(bb, 0.0f);
    if (!ItemAdd(bb, 0))
        return;
    //window->DrawList->AddBezierCubic(bb.Min, { 120,45 }, { 278, 300 }, bb.Max, 0x5F00FF00, 6.0f);
    //window->DrawList->AddRectFilled(bb.Min, bb.Max, 0xFF00FF00);
    auto list = BezierTest(bb.Min, { 120,45 }, { 278, 300 }, bb.Max);
    window->DrawList->AddPolyline(list.Data, list.size(), 0x6F00FFFF, 0, 6.0f);
}