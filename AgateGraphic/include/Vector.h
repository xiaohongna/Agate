#pragma once
#include <assert.h>

struct Vector2
{
    float   x, y;
    Vector2() : x(0.0f), y(0.0f) { }
    Vector2(float _x, float _y) : x(_x), y(_y) { }

    float operator[] (size_t idx) const
    {
        assert(idx <= 1); return (&x)[idx];
    }

    float& operator[] (size_t idx)
    {
        assert(idx <= 1);
        return (&x)[idx];
    }

    void offset(float _x, float _y)
    {
        x += _x;
        y += _y;
    }
};

struct Vector4
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
        struct
        {
            Vector2 pos;
            Vector2 size;
        };
    };

    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }

    Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
};

static inline Vector2 operator*(const Vector2& lhs, const float rhs) { return Vector2(lhs.x * rhs, lhs.y * rhs); }
static inline float operator*(const Vector2& lhs, const Vector2& rhs) {return lhs.x* rhs.x + lhs.y * rhs.y; }
static inline Vector2 operator+(const Vector2& lhs, const Vector2& rhs) { return Vector2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline Vector2 operator-(const Vector2& lhs, const Vector2& rhs) { return Vector2(lhs.x - rhs.x, lhs.y - rhs.y); }