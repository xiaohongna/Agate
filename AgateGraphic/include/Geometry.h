#pragma once

#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"
#include <memory>
#include <vector>
#include "Brush.h"
/// <summary>
/// ÃèÊöÏß¶Î×éºÏ
/// </summary>
/// 
class FigureData;

class Figure
{
public:
	Figure();

	Figure(const Figure& figure);

	Figure(Figure&& figure);

	void Reserve(int ptCount, int typeCount);

	void StartAt(float x, float y);

	void StartAt(const Vector2& pt);

	void LineTo(const Vector2& pt);

	void LineTo(const Vector2* pts, int count);

    void BezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2& endPt);

	void BezierTo(const Vector2* pts, int count);

	void Reset();

	void Close();
	friend class Geometry;
private:
	std::unique_ptr<FigureData> _Data;
};

class Geometry
{
public:
	Geometry() :_Frozen{ false }, _StrokeWidth{1.0f}
	{

	}
	void AddFigure(Figure&& figure);

	void SetStrokeWidth(float width);

	float GetStrokeWidth()
	{
		return _StrokeWidth;
	}

	void SetFillBrush(Brush& brush);

	void SetStrokeBrush(Brush& brush);
		
	Vector4 GetBounds();

	const std::vector<Vector2>& Flatten();
	
	void Freeze()
	{
		_Frozen = true;
	}

	int Rasterize();

	const RasterizeData& getRasterizeData(int index);
	
private:
	std::vector<std::unique_ptr<FigureData>> _Figures;
	std::vector<Vector2> _FlattenLines;
	
	RasterizeData  _RasterizeData[2];

	std::vector<byte>  _StrokeVertexBuffer;
	std::vector<DrawIndex> _StrokeDrawIndex;

	std::vector<byte>  _FillVertexBuffer;
	std::vector<DrawIndex> _FillDrawIndex;

	bool		_Frozen;
	float		_StrokeWidth;

};