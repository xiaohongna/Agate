#pragma once
#include <assert.h>

namespace agate
{
    struct AgateAPI Vector2
    {
        float   x, y;
        
        Vector2() : x(0.0f), y(0.0f) { }
        
        Vector2(float _x, float _y) : x(_x), y(_y) { }

        void offset(float _x, float _y)
        {
            x += _x;
            y += _y;
        }

        float operator[] (size_t idx) const
        {
            assert(idx <= 1); return (&x)[idx];
        }

        float& operator[] (size_t idx)
        {
            assert(idx <= 1);
            return (&x)[idx];
        }

        Vector2 operator*(const float scale) const
        {
            return Vector2(x * scale, y * scale);
        }

        Vector2 operator*(const Vector2& v2) const
        {
            return Vector2(x * v2.x, y * v2.y);
        }

        Vector2 operator+(const Vector2& v2) const
        {
            return Vector2(x + v2.x, y + v2.y);
        }

        Vector2 operator-(const Vector2& v2) const
        {
            return Vector2(x - v2.x, y - v2.y);
        }

    };

    struct AgateAPI Vector4
    {
        union
        {
            struct
            {
                float x, y, z, w;
            };
            struct
            {
                float left, top, right, bottom;
            };
        };

        Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }

        Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }

        float Width() const
        {
            return right - left;
        }
        float Height() const
        {
            return bottom - top;
        }
        bool operator != (const Vector4 v4) const
        {
            return x != v4.x || y != v4.y || z != v4.z || w != v4.w;
        }

        bool operator == (const Vector4 v4) const
        {
            return x == v4.x && y == v4.y && z == v4.z && w == v4.w;
        }
    };

    struct Rect
    {
        Vector2 pos;
        Vector2 size;

        Rect() :pos{}, size{}
        {

        }

        Rect(float left, float top, float width, float height):
            pos(left, top), size(width, height)
        {

        }

        float left() const
        {
            return pos.x;
        }

        float top() const
        {
            return pos.y;
        }

        float right() const
        {
            return pos.x + size.x;
        }
        float bottom() const
        {
            return pos.y + size.y;
        }
    };
}
