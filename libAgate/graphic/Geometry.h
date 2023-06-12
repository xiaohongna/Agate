#pragma once
#include "base/Ref.h"
#include "base/Vector.h"
#include "base/Matrix.h"
#include "graphic/Visual.h"
#include "Canvas.h"

namespace agate
{
    class AgateAPI Geometry :
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
            _closed = true;
        }

        void setTransform(const Matrix3X2& transition)
        {
            _transition = transition;
        }

        void paint(Canvas* canvas);


        static Geometry* initAsRectangle(const Vector4& rect);

        static Geometry* initAsRoundedRectangle(const Vector4& rect, float rRadiusX, float rRadiusY);

        static Geometry* initAsEllipse(float rCenterX, float rCenterY, float rRadiusX, float rRadiusY);

    private:
        void reserve(int32_t pointCount, int32_t typeCount);
    private:
        std::vector<Vector2> _points;
        std::vector<LineType> _pointTypes;
        Matrix3X2 _transition;
        bool _closed{ false };
        Color _strokeColor;
        Color _fillColor;
        float _strokeWidth{ 1.0f };

        Share_Ptr<Visual>  _visual;
    };
}

