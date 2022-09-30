#pragma once

#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"
#include <memory>
#include <vector>
#include "Brush.h"
#include "Drawingable.h"
/// <summary>
/// 描述线段组合
/// </summary>
///
///  
enum class PointType
{
	Line,
	Bezier
};

class Figure
{
public:
	Figure();

	Figure(const Figure& figure);

	Figure(Figure&& figure);

	void Reserve(int ptCount, int typeCount);

	void StartAt(float x, float y);

	void StartAt(const Vector2& pt);

	void LineTo(float x, float y);

	void LineTo(const Vector2& pt)
	{
		LineTo(pt.x, pt.y);
	}

	void LineTo(const Vector2* pts, int count);
    /// <summary>
    /// 3次曲线， 验证  https://www.bezier-curve.com/ 
    /// </summary>
    /// <param name="ctr1"></param>
    /// <param name="ctr2"></param>
    /// <param name="endPt"></param>
    void BezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2& endPt);

	void BezierTo(const Vector2* pts, int count);
	/// <summary>
	/// 2次曲线
	/// </summary>
	/// <param name="ctrl"></param>
	/// <param name="endPt"></param>
	void QuadBezierTo(const Vector2& ctrl, const Vector2& endPt);

	void ArcTo(float xRadius,  float yRadius,  float rRotation,  bool fLargeArc,  bool fSweepUp,  float xEnd,  float yEnd);    

	void Reset();

	void Close();

	bool IsClosed()
	{
		return _Closed;
	}

	static Figure InitAsRectangle(const Vector4& rect);

	static Figure InitAsRoundedRectangle(const Vector4& rect, float rRadiusX, float rRadiusY);

	static Figure InitAsEllipse(float rCenterX, float rCenterY,float rRadiusX, float rRadiusY);

	friend class Geometry;
private:
	
	Vector2* AddPoints(int count);
	
	PointType* AddTypes(int count, PointType defType = PointType::Line);
	
	bool IsEmpty()
	{
		return _Points.empty();
	}
private:
	std::vector<Vector2> _Points;
	std::vector<PointType> _Types;
	bool  _Closed;
};

class Geometry: public Drawingable
{
public:
	Geometry() :_Flags{ }, 
		_StrokeWidth{ 1.0f }, 
		_StrokeColor{},
		_FillColor{},
		_Matrix{}
	{
		_Matrix._11 = 1.0f;
		_Matrix._22 = 1.0f;
	}
	
	void AddFigure(Figure&& figure);

	void SetTransform(const Matrix3X2& transition);

	void SetStrokeWidth(float width);

	float GetStrokeWidth()
	{
		return _StrokeWidth;
	}

	void SetFillBrush(Brush& brush);

	void SetStrokeBrush(Brush& brush);
	
	//void SetBlendMode(BlendMode blend);

	Vector4 GetBounds();
	
	void Freeze();

	uint32_t Rasterize() override;

	const RasterizeData& GetRasterizeData(uint32_t index) override;
private:

	void AddFlag(uint32_t flag)
	{
		_Flags = _Flags | flag;
	}

	void RemoveFlag(uint32_t flag)
	{
		_Flags = _Flags & ~flag;
	}
	
	bool HaveFlag(uint32_t flag)
	{
		return (_Flags & flag) == flag;
	}

	void Flatten();

	void FlattenBezier(Vector2* pPoints);

	void RasterizeStroke(uint32_t color);

	void RasterizeFill(uint32_t color);
private:
	std::vector<Figure> _Figures;
	std::vector<Vector2> _FlattenLines;
	
	RasterizeData	_StrokeData;
	RasterizeData	_FillData;
	Color			_StrokeColor;
	Color			_FillColor;

	uint32_t	_Flags;
	float		_StrokeWidth;
	Matrix3X2		_Matrix;

};