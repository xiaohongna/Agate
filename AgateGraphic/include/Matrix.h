#pragma once

#include <stdint.h>
#include <cmath>
#include "Vector.h"

constexpr float PI = 3.14159265358979323846f;

class Matrix3X2
{
public:
    static Matrix3X2 Identity()
    {
        Matrix3X2 identity{};
        identity._11 = 1.0f;
        identity._22 = 1.0f;
        return identity;
    }

    static Matrix3X2 Translation(float x, float y)
    {
        Matrix3X2 identity{};
        identity._11 = 1.0f;
        identity._22 = 1.0f;
        identity._31 = x;
        identity._32 = y;
        return identity;
    }

    static Matrix3X2 Scale(float value)
    {
        Matrix3X2 scale{};
        scale._11 = value;
        scale._22 = value;
        return scale;
    }

    static Matrix3X2 Scale(float x, float y)
    {
        Matrix3X2 scale{};
        scale._11 = x;
        scale._22 = y;
        return scale;
    }

    static Matrix3X2 Scale(float value, const Vector2& center)
    {
        Matrix3X2 scale;
        scale._11 = value; scale._12 = 0.0;
        scale._21 = 0.0; scale._22 = value;
        scale._31 = center.x - value * center.x;
        scale._32 = center.y - value * center.y;

        return scale;
    }

    static Matrix3X2 Rotation(float angle)
    {
        float cs = cosf(angle), sn = sinf(angle);
        Matrix3X2 rotation;
        rotation._11 = cs; rotation._12 = sn;
        rotation._21 = -sn; rotation._22 = cs;
        rotation._31 = rotation._32 = 0.0f;
        return rotation;
    }

    static Matrix3X2 Rotation(float angle, const Vector2& center)
    {
        float cs = cosf(angle), sn = sinf(angle);
        Matrix3X2 rotation;
        rotation._11 = cs; rotation._12 = sn;
        rotation._21 = -sn; rotation._22 = cs;
        rotation._31 = center.x - (center.x * cs - center.y * sn);
        rotation._32 = center.y - (center.x * sn + center.y * cs);
        return rotation;
    }

    void Set(float m11, float m12, float m21, float m22, float m31, float m32)
    {
        _11 = m11; _12 = m12;
        _21 = m21; _22 = m22;
        _31 = m31; _32 = m32;
    }

    Vector2 TransformPoint(const Vector2& point) const
    {
        Vector2 result =
        {
            point.x * _11 + point.y * _21 + _31,
            point.x * _12 + point.y * _22 + _32
        };
        return result;
    }

    void TransformPoint(Vector2* point, const int count) const
    {
        for (int i = 0; i < count; i++)
        {
            auto x = point->x * _11 + point->y * _21 + _31;
            auto y = point->x * _12 + point->y * _22 + _32;
            point->x = x;
            point->y = y;
            point++;
        }
    }

    void TransformPoint(Vector2& dest, const Vector2& source) const
    {
        dest.x = source.x * _11 + source.y * _21 + _31;
        dest.y = source.x * _12 + source.y * _22 + _32;
    }

    float GetScale() const
    {
        return sqrtf(_11 * _11 + _12 * _12);
    }

    float GetRotation() const
    {
        if (_11 == 0.0f)
        {
            return _12 > 0 ? PI / 2 : PI * 1.5f;
        }
        auto angle = atanf(_12 / _11);
        if (_11 < 0)
        {
            angle += PI;
        }
        return angle;
    }

    bool IsIdentity() const
    {
        return     _11 == 1.f && _12 == 0.f
            && _21 == 0.f && _22 == 1.f
            && _31 == 0.f && _32 == 0.f;
    }

    bool IsInvertible() const
    {
        double det = (double)t[0] * t[3] - (double)t[2] * t[1];
        if (det > -1e-6 && det < 1e-6)
        {
            return false;
        }
        return true;
    }

    Matrix3X2 Invert() const
    {
        double invdet, det = (double)t[0] * t[3] - (double)t[2] * t[1];
        if (det > -1e-6 && det < 1e-6) {
            return Identity();
        }
        Matrix3X2 invert;
        invdet = 1.0 / det;
        invert.t[0] = (float)(t[3] * invdet);
        invert.t[2] = (float)(-t[2] * invdet);
        invert.t[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
        invert.t[1] = (float)(-t[1] * invdet);
        invert.t[3] = (float)(t[0] * invdet);
        invert.t[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
        return invert;
    }

    Matrix3X2 operator*(const Matrix3X2& matrix) const
    {
        Matrix3X2 result;
        result.SetProduct(*this, matrix);
        return result;
    }

    /// <summary>
    /// ÊÇ·ñÐý×ª
    /// </summary>
    /// <returns></returns>
    bool IsRotation() const
    {
        return abs(_11 + _21 - _12 - _22) > 0.00001f;
    }
private:
    void SetProduct(const Matrix3X2& a, const Matrix3X2& b)
    {
        _11 = a._11 * b._11 + a._12 * b._21;
        _12 = a._11 * b._12 + a._12 * b._22;
        _21 = a._21 * b._11 + a._22 * b._21;
        _22 = a._21 * b._12 + a._22 * b._22;
        _31 = a._31 * b._11 + a._32 * b._21 + b._31;
        _32 = a._31 * b._12 + a._32 * b._22 + b._32;
    }
public:
    union
    {
        struct
        {
            float _11, _12;
            float _21, _22;
            float _31, _32;
        };

        float m[3][2];

        float t[6];
    };

};

