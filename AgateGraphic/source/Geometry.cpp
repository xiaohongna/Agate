#include "Geometry.h"
#include "bezier.h"

enum class PointType
{
	Line,
	Bezier
};

class FigureData
{
public:
	FigureData() :_Closed{false} 
	{

	};

	void Reserve(int ptCount, int typeCount)
	{
		_Points.reserve(ptCount);
		_Types.reserve(typeCount);
	}

	FigureData(const FigureData& from)
	{
		_Points.reserve(from._Points.size());
		_Types.reserve(from._Types.size());
		_Points.assign(from._Points.begin(), from._Points.end());
		_Types.assign(from._Types.begin(), from._Types.end());
		_Closed = from._Closed;
	}


	void StartAt(float x, float y)
	{
		assert(IsEmpty());
		_Points.emplace_back(Vector2(x, y));
	}

	bool IsEmpty()
	{
		return _Points.empty();
	}

	void Close()
	{
		_Closed = true;
	}

	Vector2* AddPoints(int count)
	{
		auto size = _Points.size();
		_Points.resize(size + count);
		return _Points.data() + size;

	}
	
	PointType* AddTypes(int count)
	{
		auto size = _Types.size();
		_Types.resize(size + count);
		return _Types.data() + size;
	}
private:
	std::vector<Vector2> _Points;
	std::vector<PointType> _Types;
	bool  _Closed;
	
};

Figure::Figure(): _Data{std::make_unique<FigureData>()}
{
}

Figure::Figure(const Figure& figure):_Data{ std::make_unique<FigureData>(*figure._Data)}
{

}

Figure::Figure(Figure&& figure)
{
	_Data = std::move(figure._Data);
}

void Figure::Reserve(int ptCount, int typeCount)
{
	_Data->Reserve(ptCount, typeCount);
}

void Figure::StartAt(float x, float y)
{
	_Data->StartAt(x, y);
}

void Figure::StartAt(const Vector2& pt)
{
	_Data->StartAt(pt.x, pt.y);
}

void Figure::LineTo(const Vector2& pt)
{
	auto ppt = _Data->AddPoints(1);
	auto ptype = _Data->AddTypes(1);
	ppt->x = pt.x;
	ppt->y = pt.y;
	*ptype = PointType::Line;
}

void Figure::LineTo(const Vector2* pts, int count)
{
	auto ppt = _Data->AddPoints(count);
	auto ptype = _Data->AddTypes(count);
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
	auto ppt = _Data->AddPoints(3);
	auto ptype = _Data->AddTypes(1);
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
	auto ppt = _Data->AddPoints(bCount * 3);
	auto ptype = _Data->AddTypes(bCount);
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

void Figure::Reset()
{
	_Data = std::make_unique<FigureData>();
}

void Figure::Close()
{
	_Data->Close();
}

void Geometry::AddFigure(Figure&& figure)
{
	assert(!_Frozen);
	if (!_Frozen)
	{
		_Figures.emplace_back(std::move(figure._Data));
	}
}

void Geometry::SetStrokeWidth(float width)
{
	assert(!_Frozen);
	_StrokeWidth = width;
}

void Geometry::SetFillBrush(Brush& brush)
{
	assert(!_Frozen);
	
}

void Geometry::SetStrokeBrush(Brush& brush)
{
	assert(!_Frozen);
}

Vector4 Geometry::GetBounds()
{
	return Vector4();
}

const std::vector<Vector2>& Geometry::Flatten()
{
	return _FlattenLines;
}

int Geometry::Rasterize()
{
	return 0;
}

const RasterizeData& Geometry::getRasterizeData(int index)
{
	// TODO: 在此处插入 return 语句
}
