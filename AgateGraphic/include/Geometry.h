#pragma once

#include "Vector.h"
#include "Vertex.h"
#include "Matrix.h"
#include <memory>
#include <vector>
#include "Brush.h"
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

	void LineTo(const Vector2& pt);

	void LineTo(const Vector2* pts, int count);

    void BezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2& endPt);

	void BezierTo(const Vector2* pts, int count);

	void Reset();

	void Close();

	friend class Geometry;
private:
	
	Vector2* AddPoints(int count);
	
	PointType* AddTypes(int count);
	
	bool IsEmpty()
	{
		return _Points.empty();
	}
private:
	std::vector<Vector2> _Points;
	std::vector<PointType> _Types;
	bool  _Closed;
};


template<typename T>
struct FixedBuffer
{
	T* buffer;
	uint32_t count;   //元素数量
	uint32_t preSize;  //单元素大小
	uint32_t size;    //buffer内存大小
	FixedBuffer() :buffer{}, count{}, size{}, preSize{1}
	{

	}
	
	template<typename T1>
	T1* Alloc(int count)
	{
		this->count = count;
		preSize = sizeof(T1);
		size = count * sizeof(T1);
		buffer = (T*)malloc(size);
		return (T1*)buffer;
	}

	void Reset()
	{
		if (buffer)
		{
			delete buffer;
			buffer = nullptr;
			count = 0;
			size = 0;
		}
	}

	template<typename T1>
	T1* As() const
	{
		assert(size == count * sizeof(T1));
		return (T1*)buffer;
	}

	~FixedBuffer() 
	{
		Reset();
	}
};

struct RasterizeData
{
	PiplineType pipline;
	BlendMode   blend;
	FixedBuffer<char> vertex;
	FixedBuffer<DrawIndex> index;
	RasterizeData():pipline{PiplineType::Color}, blend{BlendMode::Blend}
	{
		
	}
};

class Geometry
{
public:
	Geometry() :_Flags{ }, _StrokeWidth{ 1.0f }
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

	uint32_t Rasterize();

	const RasterizeData& GetRasterizeData(uint32_t index);
private:
	void Flatten();

	void FlattenBezier(Vector2* pPoints);

	void RasterizeStroke(uint32_t color);

private:
	std::vector<Figure> _Figures;
	std::vector<Vector2> _FlattenLines;
	
	RasterizeData	_StrokeData;
	RasterizeData	_FillData;

	uint32_t	_Flags;
	float		_StrokeWidth;

};