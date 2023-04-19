#pragma once
#include "Ref.h"
#include "Vector.h"
#include "Matrix.h"

namespace agate
{
    class Geometry :
        public Ref
    {
    public:

        void startAt(float x, float y);

        void lineTo(float x, float y);

        void lineTo(const Vector2* pts, uint32_t count);

        void bezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2& endPt);

        void bezierTo(const Vector2* pts, uint32_t count);
        /// <summary>
        /// 2´ÎÇúÏß
        /// </summary>
        /// <param name="ctrl"></param>
        /// <param name="endPt"></param>
        void quadBezierTo(const Vector2& ctrl, const Vector2& endPt);

        void arcTo(float xRadius, float yRadius, float rRotation, bool fLargeArc, bool fSweepUp, float xEnd, float yEnd);

        void close()
        {
            _close = true;
        }
    private:
        enum class PointType
        {
            Line,
            Bezier
        };
        void reserve(int32_t pointCount, int32_t typeCount);
    private:
        std::vector<Vector2> _points;
        std::vector<PointType> _pointTypes;
        bool _close{ false };
    };
}

