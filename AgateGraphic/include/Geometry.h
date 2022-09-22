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

struct SimpleBuffer
{
	void* buffer;
	int	count;
	SimpleBuffer() :buffer{}, count{}
	{
	}
	
	template<typename T>
	T* Alloc(int count)
	{
		Free();
		this->count = count;
		buffer = malloc(count * sizeof(T));
		return (T*)buffer;
	}

	void Free()
	{
		if (buffer)
		{
			delete buffer;
			buffer = nullptr;
			count = 0;
		}
	}

	~SimpleBuffer() 
	{
		Free();
	}
};

class Geometry
{
public:
	Geometry() :_Flags{ }, _StrokeWidth{ 1.0f }, _RasterizeData{}
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
	
	void Freeze();

	int Rasterize();

	const RasterizeData& GetRasterizeData(uint32_t index);
private:
	void Flatten();

	void FlattenBezier(Vector2* pPoints);

	void RasterizeStroke(uint32_t color);

private:
	std::vector<std::unique_ptr<FigureData>> _Figures;
	std::vector<Vector2> _FlattenLines;
	
	RasterizeData  _RasterizeData[2];

	SimpleBuffer  _StrokeVertexBuffer;
	SimpleBuffer  _StrokeDrawIndex;

	SimpleBuffer  _FillVertexBuffer;
	SimpleBuffer  _FillDrawIndex;

	uint32_t	_Flags;
	float		_StrokeWidth;

};