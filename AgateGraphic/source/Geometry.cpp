#include "Geometry.h"
#include "bezier.h"

#ifdef ENABLE_SSE
#include <immintrin.h>
static inline float  ImRsqrt(float x) { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
#else
static inline float  ImRsqrt(float x) { return 1.0f / sqrtf(x); }
#endif

#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = ImRsqrt(d2); VX *= inv_len; VY *= inv_len; } } (void)0
#define IM_FIXNORMAL2F_MAX_INVLEN2          100.0f 
#define IM_FIXNORMAL2F(VX,VY)               { float d2 = VX*VX + VY*VY; if (d2 > 0.000001f) { float inv_len2 = 1.0f / d2; if (inv_len2 > IM_FIXNORMAL2F_MAX_INVLEN2) inv_len2 = IM_FIXNORMAL2F_MAX_INVLEN2; VX *= inv_len2; VY *= inv_len2; } } (void)0

constexpr float FUZZ = 1.e-6;           // Relative 0
constexpr float PI_OVER_180 = 0.0174532925199432957692f;  // PI/180
constexpr float FOUR_THIRDS = 1.33333333333333333f; // = 4/3
constexpr float ARC_AS_BEZIER = 0.5522847498307933984f; // =(\/2 - 1)*4/3

constexpr uint32_t  Geometry_Flag_Frozen = 1;

constexpr uint32_t  Geometry_Flag_Closed = 1 << 1;

constexpr uint32_t  Geometry_Flag_Flatten = 1 << 2;

constexpr uint32_t  Geometry_Flag_Stroke = 1 << 3;

constexpr uint32_t  Geometry_Flag_Fill = 1 << 4;

constexpr uint32_t  Geometry_Flag_Stroke_RS = 1 << 5;

constexpr uint32_t  Geometry_Flag_Fill_RS = 1 << 6;

Figure::Figure() : _Closed{}
{
}

Figure::Figure(const Figure& figure)
{
	_Points.assign(figure._Points.begin(), figure._Points.end());
	_Types.assign(figure._Types.begin(), figure._Types.end());
	_Closed = figure._Closed;
}

Figure::Figure(Figure&& figure):_Points{ std::move(figure._Points)}, 
_Types{std::move(figure._Types)},
_Closed{figure._Closed}
{
}

void Figure::Reserve(int ptCount, int typeCount)
{
	_Points.reserve(ptCount);
	_Types.reserve(typeCount);
}

void Figure::StartAt(float x, float y)
{
	assert(IsEmpty());
	_Points.emplace_back(Vector2(x, y));
}

void Figure::StartAt(const Vector2& pt)
{
	StartAt(pt.x, pt.y);
}

void Figure::LineTo(float x, float y)
{
    assert(!IsEmpty());
    auto ppt = AddPoints(1);
    auto ptype = AddTypes(1);
    ppt->x = x;
    ppt->y = y;
    *ptype = PointType::Line;
}

void Figure::LineTo(const Vector2* pts, int count)
{
	auto ppt = AddPoints(count);
	auto ptype = AddTypes(count);
	for (size_t i = 0; i < count; i++)
	{
		ppt->x = pts->x;
		ppt->y = pts->y;
		*ptype = PointType::Line;
		ppt++;
		ptype++;
		pts++;
	}
}

void Figure::BezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2& endPt)
{
	auto ppt = AddPoints(3);
	auto ptype = AddTypes(1);
	ppt[0].x = ctr1.x;
	ppt[0].y = ctr1.y;
	ppt[1].x = ctr2.x;
	ppt[1].y = ctr2.y;
	ppt[2].x = endPt.x;
	ppt[2].y = endPt.y;
	*ptype = PointType::Bezier;
}

void Figure::BezierTo(const Vector2* pts, int count)
{
	assert(count % 3 == 0);
	auto bCount = count / 3;
	auto ppt = AddPoints(bCount * 3);
	auto ptype = AddTypes(bCount);
	for (int i = 0; i < bCount; i++)
	{
		ppt[0] = pts[0];
		ppt[1] = pts[1];
		ppt[2] = pts[2];
		*ptype = PointType::Bezier;
		ppt += 3;
		pts += 3;
		ptype++;
	}
	
	*ptype = PointType::Bezier;
}

bool AcceptRadius(float rHalfChord2,
    // (1/2 chord length)squared
    float rFuzz2,
    // Squared fuzz
    float& rRadius)
    // The radius to accept (or not)
{
    // Ignore NaNs
    Assert(!(rHalfChord2 < rFuzz2));

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

void
GetArcAngle(
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
    constexpr float TWO_PI = 6.2831853071795865;    // 2PI
    float rAngle;

    // The points are on the unit circle, so:
    rCosArcAngle = ptStart * ptEnd;
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

    Assert(cPieces > 0);
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

float
GetBezierDistance(  // Return the distance as a fraction of the radius
    float rDot,    // In: The dot product of the two radius vectors
    float rRadius = 1.0f) // In: The radius of the arc's circle (optional=1)
{
    float rRadSquared = rRadius * rRadius;  // Squared radius
    float rNumerator;
    float rDenominatorSquared, rDenominator;

    // Ignore NaNs
    Assert(!(rDot < -rRadSquared * .1));  // angle < 90 degrees
    Assert(!(rDot > rRadSquared * 1.1));  // as dot product of 2 radius vectors

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

    Assert(pPt);
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
        rRotation = -rRotation * FLOAT(PI_OVER_180);

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
    rBezDist = FLOAT(GetBezierDistance(rCosArcAngle));
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

    Assert(j < 12);
    pPt[j] = Vector2(xEnd, yEnd);

Cleanup:;
}

inline void ClampRoundedRectangleRadius(
    float rDimension,                // The dimension, width or height
    float& rRadius)// The corresponding radius, clamped here
{
    float r = rDimension / 2;
    if (rRadius > r)
    {
        rRadius = r;
    }
}

void InitBufferWithRoundedRectanglePoints(
     Vector2* pts,
    // The point buffer
     const Vector4& rect,
    // The rectangle
    float rRadiusX,
    // The X-radius of the corner (elliptical arc)
    float rRadiusY
    // The Y-radius of the corner (elliptical arc)
)
{
    ClampRoundedRectangleRadius(rect.right - rect.left, rRadiusX);
    ClampRoundedRectangleRadius(rect.bottom - rect.top, rRadiusY);

    // Note "1 - ARC_AS_BEZIER" - because we measure from the edge of the rectangle,
    // not the center of the arc.

    float rBezierX = (1.0f - ARC_AS_BEZIER) * rRadiusX;
    float rBezierY = (1.0f - ARC_AS_BEZIER) * rRadiusY;

    pts[1].x = pts[0].x = pts[15].x = pts[14].x = rect.left;
    pts[2].x = pts[13].x = rect.left + rBezierX;
    pts[3].x = pts[12].x = rect.left + rRadiusX;
    pts[4].x = pts[11].x = rect.right - rRadiusX;
    pts[5].x = pts[10].x = rect.right - rBezierX;
    pts[6].x = pts[7].x = pts[8].x = pts[9].x = rect.right;

    pts[2].y = pts[3].y = pts[4].y = pts[5].y = rect.top;
    pts[1].y = pts[6].y = rect.top + rBezierY;
    pts[0].y = pts[7].y = rect.top + rRadiusY;
    pts[15].y = pts[8].y = rect.bottom - rRadiusY;
    pts[14].y = pts[9].y = rect.bottom - rBezierY;
    pts[13].y = pts[12].y = pts[11].y = pts[10].y = rect.bottom;
}

void Figure::ArcTo(float xRadius, float yRadius, float rRotation, bool fLargeArc, bool fSweepUp, float xEnd, float yEnd)
{
    Vector2 pt[12];
    int cPieces;

    if (xRadius < 0 || yRadius < 0)
    {
        assert(false);
        return;
    }

    // The figure should be started but not closed
    if (IsEmpty() || IsClosed())
    {
        assert(false);
        return;
    }

    // Get the approximation of the arc with Bezier curves
    ;
    ArcToBezier(_Points.back().x,
        _Points.back().y,
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
        LineTo(xEnd, yEnd);
    }
    else if (cPieces > 0)
    {
        if (cPieces > 1)
        {
            BezierTo(&pt[3], 3 *(cPieces - 1));
        }
        BezierTo(pt, 3);
    }
}

void Figure::Reset()
{
	_Points.clear();
	_Types.clear();
	_Closed = false;
}

void Figure::Close()
{
	_Closed = true;
}

Figure Figure::InitAsRectangle(const Vector4& rect)
{
    Figure figure;
    auto pt = figure.AddPoints(4);
    auto pTypes = figure.AddTypes(3);
    pt[0].x = rect.left;
    pt[0].y = rect.top;
    pt[1].x = rect.right;
    pt[1].y = rect.top;
    pt[2].x = rect.right;
    pt[2].y = rect.bottom;
    pt[3].x = rect.left;
    pt[3].y = rect.bottom;
    pTypes[0] = pTypes[1] = pTypes[2] = PointType::Line;
    figure.Close();

    return figure;
}

Figure Figure::InitAsRoundedRectangle(const Vector4& rect, float rRadiusX, float rRadiusY)
{
    Figure figure;
    auto pPoints = figure.AddPoints(16);
    auto pTypes = figure.AddTypes(7);
    InitBufferWithRoundedRectanglePoints(pPoints, rect, rRadiusX, rRadiusY);
    pTypes[0] = pTypes[2] = pTypes[4] = pTypes[6] = PointType::Bezier;
    pTypes[1] = pTypes[3] = pTypes[5] = PointType::Line;
    figure.Close();
    return figure;
}


Figure Figure::InitAsEllipse(float rCenterX, float rCenterY, float rRadiusX, float rRadiusY)
{
    Figure figure;
    auto pPoints = figure.AddPoints(13);
    auto pTypes = figure.AddTypes(4, PointType::Bezier);
    float rMid = rRadiusX * ARC_AS_BEZIER;

    pPoints[0].x = pPoints[1].x = pPoints[11].x = pPoints[12].x = rCenterX + rRadiusX;
    pPoints[2].x = pPoints[10].x = rCenterX + rMid;
    pPoints[3].x = pPoints[9].x = rCenterX;
    pPoints[4].x = pPoints[8].x = rCenterX - rMid;
    pPoints[5].x = pPoints[6].x = pPoints[7].x = rCenterX - rRadiusX;

    rMid = rRadiusY * ARC_AS_BEZIER;
    pPoints[2].y = pPoints[3].y = pPoints[4].y = rCenterY + rRadiusY;
    pPoints[1].y = pPoints[5].y = rCenterY + rMid;
    pPoints[0].y = pPoints[6].y = pPoints[12].y = rCenterY;
    pPoints[7].y = pPoints[11].y = rCenterY - rMid;
    pPoints[8].y = pPoints[9].y = pPoints[10].y = rCenterY - rRadiusY;

    return figure;
}

Vector2* Figure::AddPoints(int count)
{
	auto size = _Points.size();
	_Points.resize(size + count);
	return _Points.data() + size;
}

PointType* Figure::AddTypes(int count, PointType defType)
{
	auto size = _Types.size();
	_Types.resize(size + count, defType);
	return _Types.data() + size;
}

void Geometry::AddFigure(Figure&& figure)
{
    assert(!HaveFlag(Geometry_Flag_Frozen));
	if ((_Flags & Geometry_Flag_Frozen) == 0)
	{
        if (figure.IsClosed())
        {
            AddFlag(Geometry_Flag_Closed);
        }
		_Figures.emplace_back(std::move(figure));
	}
}

void Geometry::SetStrokeWidth(float width)
{
    assert(!HaveFlag(Geometry_Flag_Frozen));
	_StrokeWidth = width;
}

void Geometry::SetFillBrush(Brush& brush)
{
    assert(!HaveFlag(Geometry_Flag_Frozen));
    _FillColor = brush._Color; 
    AddFlag(Geometry_Flag_Fill);
}

void Geometry::SetStrokeBrush(Brush& brush)
{
    assert(!HaveFlag(Geometry_Flag_Frozen));
    _StrokeColor = brush._Color;
    AddFlag(Geometry_Flag_Stroke);
}

Vector4 Geometry::GetBounds()
{
	return Vector4();
}

void Geometry::Freeze()
{
    AddFlag(Geometry_Flag_Frozen);
}

void Geometry::Flatten()
{
	for (auto& figure :_Figures)
	{
        auto ppt = figure._Points.data();
        _FlattenLines.push_back(*ppt);
		for (auto ptt : figure._Types)
		{
			if (ptt == PointType::Line)
			{
                ppt++;
				_FlattenLines.push_back(*ppt);
			}
			else
			{
				FlattenBezier(ppt);
				ppt += 3;
			}
		}
	}
    AddFlag(Geometry_Flag_Flatten);
}

void Geometry::FlattenBezier(Vector2* pPoints)
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
			_FlattenLines.emplace_back(Vector2(points[i].x / 16.0f, points[i].y / 16.0f));
		}
	}
}

void Geometry::RasterizeStroke(uint32_t col)
{
    constexpr float FringeScale = 1.0f;
    auto points_count = _FlattenLines.size();
    if (points_count < 2)
        return;
    const bool closed = _Flags & Geometry_Flag_Closed;
    const int count = closed ? points_count : points_count - 1; 
    const bool thick_line = (_StrokeWidth > FringeScale);

    // Anti-aliased stroke
    const float AA_SIZE = FringeScale;
    const uint32_t col_trans = col & 0xFFFFFF00; // ~IM_COL32_A_MASK;
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
    _StrokeData.pipline = PipelineType::Color;
    _StrokeData.blend = BlendMode::Blend;
    AddFlag(Geometry_Flag_Stroke_RS);
}

void Geometry::RasterizeFill(uint32_t color)
{
    constexpr float _FringeScale = 1.0f;
    auto points_count = _FlattenLines.size();
    if (points_count < 3)
    {
        //assert(false);
        return;
    }
    // Anti-aliased Fill
    const float AA_SIZE = _FringeScale;
    const uint32_t col_trans = color & 0xFFFFFF00;
    const int idx_count = (points_count - 2) * 3 + points_count * 6;
    const int vtx_count = (points_count * 2);
    auto _IdxWritePtr = _FillData.index.Alloc<DrawIndex>(idx_count);
    auto _VtxWritePtr = _FillData.vertex.Alloc<VertexXYColor>(vtx_count);
    // Add indexes for fill
    uint32_t vtx_inner_idx = 0;
    uint32_t vtx_outer_idx = 1;
    for (int i = 2; i < points_count; i++)
    {
        _IdxWritePtr[0] = (DrawIndex)(vtx_inner_idx); _IdxWritePtr[1] = (DrawIndex)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (DrawIndex)(vtx_inner_idx + (i << 1));
        _IdxWritePtr += 3;
    }

    // Compute normals
    auto temp_normals = (Vector2*)_malloca(points_count * sizeof(Vector2));
    //uint32_t mask = 1;
    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
    {
        //if ((1 & mask))
        {
            const Vector2& p0 = _FlattenLines[i0];
            const Vector2& p1 = _FlattenLines[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }
        /*
        else
        {
            temp_normals[i0].x = 0.0f;
            temp_normals[i0].y = 0.0f;
        }
        mask = mask << 1;
        */
    }

    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
    {
        // Average normals
        const Vector2& n0 = temp_normals[i0];
        const Vector2& n1 = temp_normals[i1];
        float dm_x = (n0.x + n1.x) * 0.5f;
        float dm_y = (n0.y + n1.y) * 0.5f;
        IM_FIXNORMAL2F(dm_x, dm_y);
        dm_x *= AA_SIZE * 0.5f;
        dm_y *= AA_SIZE * 0.5f;

        // Add vertices
        _VtxWritePtr[0].pos.x = (_FlattenLines[i1].x - dm_x); _VtxWritePtr[0].pos.y = (_FlattenLines[i1].y - dm_y); _VtxWritePtr[0].col = color;        // Inner
        _VtxWritePtr[1].pos.x = (_FlattenLines[i1].x + dm_x); _VtxWritePtr[1].pos.y = (_FlattenLines[i1].y + dm_y); _VtxWritePtr[1].col = col_trans;  // Outer
        _VtxWritePtr += 2;

        // Add indexes for fringes
        _IdxWritePtr[0] = (DrawIndex)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (DrawIndex)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (DrawIndex)(vtx_outer_idx + (i0 << 1));
        _IdxWritePtr[3] = (DrawIndex)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (DrawIndex)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (DrawIndex)(vtx_inner_idx + (i1 << 1));
        _IdxWritePtr += 6;
    }
    AddFlag(Geometry_Flag_Fill_RS);
}

uint32_t Geometry::Rasterize()
{
    if (HaveFlag(Geometry_Flag_Flatten) == false)
    {
        Flatten();
    }
    if (HaveFlag(Geometry_Flag_Stroke) && !HaveFlag(Geometry_Flag_Stroke_RS))
    {
        RasterizeStroke(_StrokeColor.color);
    }
    if (HaveFlag(Geometry_Flag_Fill) && !HaveFlag(Geometry_Flag_Fill_RS))
    {
        RasterizeFill(0x5F6A3510);
    }
    int count = 0;
    if (HaveFlag(Geometry_Flag_Stroke_RS))
    {
        count++;
    }
    if (HaveFlag(Geometry_Flag_Fill_RS))
    {
        count++;
    }
    return count;
}

const RasterizeData& Geometry::GetRasterizeData(uint32_t index)
{
	assert(index < 2);
    return index == 0 ? _StrokeData : _FillData;
}
