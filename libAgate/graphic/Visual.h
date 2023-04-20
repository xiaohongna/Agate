#pragma once
#include "Ref.h"
#include "Vector.h"
#include "Color.h"
#include "Matrix.h"

namespace agate
{
    enum class LineType
    {
        Line,
        Bezier
    };

    class Visual :
        public Ref
    {
    public:
        Visual() :Ref()
        {

        }

        void setGeometry(const std::vector<Vector2>& pts, const std::vector<LineType>& lts,
            bool closed)
        {
            _points = pts;
            _lineTypes = lts;
            _closed = closed;
        }

        void setStrokeWidth(float width)
        {
            _strokeWidth = width;
        }

        void setFillColor(Color& clr)
        {
            _fillColor = clr;
        }

        void setStrokeColor(Color& clr)
        {
            _strokeColor = clr;
        }

        void setTransform(const Matrix3X2& transition)
        {
            _transition = transition;
        }
    private:
        void flatten(std::vector<Vector2>& flattenLines);

        void flattenBezier(Vector2* pPoints, std::vector<Vector2>& flattenLines);

        void rasterizeStroke(uint32_t color, std::vector<Vector2>& flattenLines);

        void rasterizeFill(uint32_t color, std::vector<Vector2>& flattenLines);
    private:
        std::vector<Vector2> _points;
        std::vector<LineType> _lineTypes;
        bool _closed { false };
        Matrix3X2 _transition;
        Color _strokeColor;
        Color _fillColor;
        float _strokeWidth{ 1.0f };

        std::vector<Vector2> _flattenLines;
    };
}

