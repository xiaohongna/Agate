#include "pch.h"
#include "Geometry.h"

namespace agate
{
    void ArcToBezier(
        float xStart,     // X coordinate of the last point
        float yStart,     // Y coordinate of the last point
        float xRadius,    // The ellipse's X radius
        float yRadius,    // The ellipse's Y radius
        float rRotation,  // Rotation angle of the ellipse's x axis
        bool fLargeArc,  // Choose the larger of the 2 possible arcs if TRUE
        bool fSweepUp,   // Sweep the arc while increasing the angle if TRUE
        float xEnd,       // X coordinate of the last point
        float yEnd,       // Y coordinate of the last point
        Vector2* pPt,       // An array of size 12 receiving the Bezier points
        int& cPieces);   // The number of output Bezier curves
	void Geometry::startAt(float x, float y)
	{
		if (!_points.empty())
		{
			assert(false);
		}
		_points.emplace_back(x, y);
	}

	void Geometry::lineTo(float x, float y)
	{
		_points.emplace_back(x, y);
		_pointTypes.emplace_back(LineType::Line);
	}

	void Geometry::lineTo(const Vector2* pts, uint32_t count)
	{
		reserve(count, count);
		for (size_t i = 0; i < count; i++)
		{
			_points.emplace_back(pts[i]);
			_pointTypes.emplace_back(LineType::Line);
		}
	}

	void Geometry::bezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2& endPt)
	{
		reserve(3, 1);
		_points.emplace_back(ctr1);
		_points.emplace_back(ctr2);
		_points.emplace_back(endPt);
		_pointTypes.emplace_back(LineType::Bezier);
	}

	void Geometry::bezierTo(const Vector2* pts, uint32_t count)
	{
		assert(count % 3 == 0);
		auto bCount = count / 3;
		reserve(count, bCount);
		for (uint32_t i = 0; i < bCount; i++)
		{
			_points.emplace_back(pts[0]);
			_points.emplace_back(pts[1]);
			_points.emplace_back(pts[2]);
			_pointTypes.emplace_back(LineType::Bezier);
			pts += 3;
		}
	}
	void Geometry::quadBezierTo(const Vector2& ctrl, const Vector2& endPt)
	{
		constexpr float fact = 2.0f / 3.0f;
		if (_points.empty())
		{
			assert(false);
			return;
		}
		auto& back = _points.back();
		Vector2 bCtrl1{ back.x + fact * (ctrl.x - back.x), back.y + fact * (ctrl.y - back.y) };
		Vector2 bCtrl2{ endPt.x + fact * (ctrl.x - endPt.x), endPt.y + fact * (ctrl.y - endPt.y) };
		bezierTo(bCtrl1, bCtrl2, endPt);
	}

	void Geometry::arcTo(float xRadius, float yRadius, float rRotation, bool fLargeArc, bool fSweepUp, float xEnd, float yEnd)
	{
		Vector2 pt[12];
		int cPieces;

		if (xRadius < 0 || yRadius < 0)
		{
			assert(false);
			return;
		}

		if (_points.empty())
		{
			assert(false);
			return;
		}

		// Get the approximation of the arc with Bezier curves
		;
		ArcToBezier(_points.back().x,
			_points.back().y,
			xRadius,
			yRadius,
			rRotation,
			fLargeArc,
			fSweepUp,
			xEnd,
			yEnd,
			pt,
			cPieces);

		if (0 == cPieces)
		{
			// We have a zero radius, add a straight line segment instead of an arc
			lineTo(xEnd, yEnd);
		}
		else if (cPieces > 0)
		{
			if (cPieces > 1)
			{
				bezierTo(&pt[3], 3 * (cPieces - 1));
			}
			bezierTo(pt, 3);
		}
	}

    void Geometry::paint(Canvas* canvas)
    {
        if (_visual == nullptr)
        {
            _visual = new Visual();
            _visual->setGeometry(_points, _pointTypes, _closed);
        }
        canvas->paint(_visual);
    }

    Geometry* Geometry::initAsRectangle(const Vector4& rect)
    {
        Geometry* geometry = new Geometry();
        geometry->reserve(4, 3);
        geometry->_points[0].x = rect.left;
        geometry->_points[0].y = rect.top;
        geometry->_points[1].x = rect.right;
        geometry->_points[1].y = rect.top;
        geometry->_points[2].x = rect.right;
        geometry->_points[2].y = rect.bottom;
        geometry->_points[3].x = rect.left;
        geometry->_points[3].y = rect.bottom;
        geometry->_pointTypes[0] = geometry->_pointTypes[1] = geometry->_pointTypes[2] = LineType::Line;
        geometry->close();
        return geometry;
    }

    Geometry* Geometry::initAsRoundedRectangle(const Vector4& rect, float rRadiusX, float rRadiusY)
    {
        return nullptr;
    }

    Geometry* Geometry::initAsEllipse(float rCenterX, float rCenterY, float rRadiusX, float rRadiusY)
    {
        return nullptr;
    }

    void Geometry::reserve(int32_t pointCount, int32_t typeCount)
	{
		_points.reserve(_points.size() + pointCount);
		_pointTypes.reserve(_pointTypes.size() + typeCount);
	}
#pragma region ¸¨Öúº¯Êý
    bool AcceptRadius(float rHalfChord2,
        // (1/2 chord length)squared
        float rFuzz2,
        // Squared fuzz
        float& rRadius)
        // The radius to accept (or not)
    {
        // Ignore NaNs
        assert(!(rHalfChord2 < rFuzz2));

        //
        // If the above assert is not true, we have no guarantee that the radius is
        // not 0, and we need to divide by the radius.
        //

        //
        // Accept NaN here, because otherwise we risk forgetting we encountered
        // one.
        //
        bool fAccept = !(rRadius * rRadius <= rHalfChord2 * rFuzz2);
        if (fAccept)
        {
            if (rRadius < 0)
            {
                rRadius = -rRadius;
            }
        }
        return fAccept;
    }

    inline float Determinant(const Vector2& a, const Vector2& b)
    {
        return (a.x * b.y - a.y * b.x);
    }

    void GetArcAngle(
        const Vector2& ptStart,
        // Start point
        const Vector2& ptEnd,
        // End point
        bool fLargeArc,
        // Choose the larger of the 2 possible arcs if TRUE
        bool fSweepUp,
        // Sweep the arc while increasing the angle if TRUE
        float& rCosArcAngle,
        // Cosine of a the sweep angle of one arc piece
        float& rSinArcAngle,
        // Sine of a the sweep angle of one arc piece
        int& cPieces)      // Out: The number of pieces
    {
        constexpr float TWO_PI = 6.2831853071795865f;    // 2PI
        float rAngle;

        // The points are on the unit circle, so:
        rCosArcAngle = ptStart.x * ptEnd.x + ptStart.y * ptEnd.y;
        rSinArcAngle = Determinant(ptStart, ptEnd);

        if (rCosArcAngle >= 0)
        {
            if (fLargeArc)
            {
                // The angle is between 270 and 360 degrees, so
                cPieces = 4;
            }
            else
            {
                // The angle is between 0 and 90 degrees, so
                cPieces = 1;
                return;
            }
        }
        else
        {
            if (fLargeArc)
            {
                // The angle is between 180 and 270 degrees, so
                cPieces = 3;
            }
            else
            {
                // The angle is between 90 and 180 degrees, so
                cPieces = 2;
            }
        }

        //
        // We have to chop the arc into the computed number of pieces.  For
        // cPieces=2 and 4 we could have uses the half-angle trig formulas, but for
        // cPieces=3 it requires solving a cubic equation; the performance
        // difference is not worth the extra code, so we'll get the angle, divide
        // it, and get its sine and cosine.
        //

        assert(cPieces > 0);
        rAngle = atan2f(rSinArcAngle, rCosArcAngle);
        if (fSweepUp)
        {
            if (rAngle < 0)
            {
                rAngle += static_cast<float>(TWO_PI);
            }
        }
        else
        {
            if (rAngle > 0)
            {
                rAngle -= static_cast<float>(TWO_PI);
            }
        }
        rAngle /= cPieces;
        rCosArcAngle = cosf(rAngle);
        rSinArcAngle = sinf(rAngle);
    }

    constexpr float FUZZ = 1.e-6f;           // Relative 0
    constexpr float PI_OVER_180 = 0.0174532925199432957692f;  // PI/180
    constexpr float FOUR_THIRDS = 1.33333333333333333f; // = 4/3
    constexpr float ARC_AS_BEZIER = 0.5522847498307933984f; // =(\/2 - 1)*4/3

    float GetBezierDistance(  // Return the distance as a fraction of the radius
        float rDot,    // In: The dot product of the two radius vectors
        float rRadius = 1.0f) // In: The radius of the arc's circle (optional=1)
    {
        float rRadSquared = rRadius * rRadius;  // Squared radius
        float rNumerator;
        float rDenominatorSquared, rDenominator;

        // Ignore NaNs
        assert(!(rDot < -rRadSquared * .1));  // angle < 90 degrees
        assert(!(rDot > rRadSquared * 1.1));  // as dot product of 2 radius vectors

        float rDist = 0.0f;   // Acceptable fallback value

        float rA = 0.5f * (rRadSquared + rDot);
        if (rA < 0.0f)
        {
            // Shouldn't happen but dist=0 will work
            goto exit;
        }

        rDenominatorSquared = rRadSquared - rA;

        if (rDenominatorSquared <= 0)
            // 0 angle, we shouldn't be rounding the corner, but dist=0 is OK
            goto exit;

        rDenominator = sqrt(rDenominatorSquared);
        rNumerator = FOUR_THIRDS * (rRadius - sqrt(rA));

        if (rNumerator <= rDenominator * FUZZ)
        {
            //
            // Dist is very close to 0, so we'll snap it to 0 and save ourselves a
            // divide.
            //
            rDist = 0.0;
        }
        else
        {
            rDist = rNumerator / rDenominator;
        }

    exit:
        return rDist;
    }


    void ArcToBezier(
        float xStart,     // X coordinate of the last point
        float yStart,     // Y coordinate of the last point
        float xRadius,    // The ellipse's X radius
        float yRadius,    // The ellipse's Y radius
        float rRotation,  // Rotation angle of the ellipse's x axis
        bool fLargeArc,  // Choose the larger of the 2 possible arcs if TRUE
        bool fSweepUp,   // Sweep the arc while increasing the angle if TRUE
        float xEnd,       // X coordinate of the last point
        float yEnd,       // Y coordinate of the last point
        Vector2* pPt,       // An array of size 12 receiving the Bezier points
        int& cPieces)   // The number of output Bezier curves
    {
        float x, y, rHalfChord2, rCos, rSin, rCosArcAngle, rSinArcAngle,
            xCenter, yCenter, rBezDist;
        Vector2 ptStart, ptEnd, vecToBez1, vecToBez2;
        Matrix3X2 matToEllipse;

        float rFuzz2 = float(FUZZ * FUZZ);
        bool fZeroCenter = false;
        int i, j;

        assert(pPt);
        cPieces = -1;

        // In the following, the line segment between between the arc's start and 
        // end points is referred to as "the chord".

        // Transform 1: Shift the origin to the chord's midpoint
        x = 0.5f * (xEnd - xStart);
        y = 0.5f * (yEnd - yStart);

        rHalfChord2 = x * x + y * y;     // (half chord length)^2

        // Degenerate case: single point
        if (rHalfChord2 < rFuzz2)
        {
            // The chord degenerates to a point, the arc will be ignored
            goto Cleanup;
        }

        // Degenerate case: straight line
        if (!AcceptRadius(rHalfChord2, rFuzz2, xRadius) ||
            !AcceptRadius(rHalfChord2, rFuzz2, yRadius))
        {
            // We have a zero radius, add a straight line segment instead of an arc
            cPieces = 0;
            goto Cleanup;
        }

        // Transform 2: Rotate to the ellipse's coordinate system
        if (fabs(rRotation) < FUZZ)
        {
            // 
            // rRotation will almost always be 0 and Sin/Cos are expensive
            // functions. Let's not call them if we don't have to.
            //
            rCos = 1.0f;
            rSin = 0.0f;
        }
        else
        {
            rRotation = -rRotation * PI_OVER_180;

            rCos = cosf(rRotation);
            rSin = sinf(rRotation);

            float rTemp = x * rCos - y * rSin;
            y = x * rSin + y * rCos;
            x = rTemp;
        }

        // Transform 3: Scale so that the ellipse will become a unit circle
        x /= xRadius;
        y /= yRadius;

        // We get to the center of that circle along a vector perpendicular to the chord   
        // from the origin, which is the chord's midpoint. By Pythagoras, the length of that
        // vector is sqrt(1 - (half chord)^2).

        rHalfChord2 = x * x + y * y;   // now in the circle coordinates   
        if (rHalfChord2 > 1.0f)
        {
            // The chord is longer than the circle's diameter; we scale the radii uniformly so 
            // that the chord will be a diameter. The center will then be the chord's midpoint,
            // which is now the origin.
            float rTemp = sqrtf(rHalfChord2);

            xRadius *= rTemp;
            yRadius *= rTemp;
            xCenter = yCenter = 0;
            fZeroCenter = true;

            // Adjust the unit-circle coordinates x and y
            x /= rTemp;
            y /= rTemp;
        }
        else
        {
            // The length of (-y,x) or (x,-y) is sqrt(rHalfChord2), and we want a vector
            // of length sqrt(1 - rHalfChord2), so we'll multiply it by:
            float rTemp = sqrtf((1.0f - rHalfChord2) / rHalfChord2);
            if (fLargeArc != fSweepUp)
                // Going to the center from the origin=chord-midpoint
            {
                // in the direction of (-y, x)
                xCenter = -rTemp * y;
                yCenter = rTemp * x;
            }
            else
            {
                // in the direction of (y, -x)
                xCenter = rTemp * y;
                yCenter = -rTemp * x;
            }
        }

        // Transformation 4: shift the origin to the center of the circle, which then becomes
        // the unit circle. Since the chord's midpoint is the origin, the start point is (-x, -y)
        // and the endpoint is (x, y).
        ptStart = Vector2(-x - xCenter, -y - yCenter);
        ptEnd = Vector2(x - xCenter, y - yCenter);

        // Set up the matrix that will take us back to our coordinate system.  This matrix is
        // the inverse of the combination of transformation 1 thru 4.
        matToEllipse.Set(rCos * xRadius, -rSin * xRadius,
            rSin * yRadius, rCos * yRadius,
            0.5f * (xEnd + xStart), 0.5f * (yEnd + yStart));
        if (!fZeroCenter)
        {
            // Prepend the translation that will take the origin to the circle's center
            matToEllipse._31 += (matToEllipse._11 * xCenter + matToEllipse._21 * yCenter);
            matToEllipse._32 += (matToEllipse._12 * xCenter + matToEllipse._22 * yCenter);
        }

        // Get the sine & cosine of the angle that will generate the arc pieces
        GetArcAngle(ptStart, ptEnd, fLargeArc, fSweepUp, rCosArcAngle, rSinArcAngle, cPieces);

        // Get the vector to the first Bezier control point
        rBezDist = GetBezierDistance(rCosArcAngle);
        if (!fSweepUp)
        {
            rBezDist = -rBezDist;
        }
        vecToBez1 = Vector2(-rBezDist * ptStart.y, rBezDist * ptStart.x);

        // Add the arc pieces, except for the last
        j = 0;
        for (i = 1; i < cPieces; i++)
        {
            // Get the arc piece's endpoint
            Vector2 ptPieceEnd(ptStart.x * rCosArcAngle - ptStart.y * rSinArcAngle,
                ptStart.x * rSinArcAngle + ptStart.y * rCosArcAngle);
            vecToBez2 = Vector2(-rBezDist * ptPieceEnd.y, rBezDist * ptPieceEnd.x);

            matToEllipse.TransformPoint(pPt[j++], ptStart + vecToBez1);
            matToEllipse.TransformPoint(pPt[j++], ptPieceEnd - vecToBez2);
            matToEllipse.TransformPoint(pPt[j++], ptPieceEnd);

            // Move on to the next arc
            ptStart = ptPieceEnd;
            vecToBez1 = vecToBez2;
        }

        // Last arc - we know the endpoint
        vecToBez2 = Vector2(-rBezDist * ptEnd.y, rBezDist * ptEnd.x);

        matToEllipse.TransformPoint(pPt[j++], ptStart + vecToBez1);
        matToEllipse.TransformPoint(pPt[j++], ptEnd - vecToBez2);

        assert(j < 12);
        pPt[j] = Vector2(xEnd, yEnd);

    Cleanup:;
    }
#pragma endregion
}
