#include "Geometry.h"


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
	

private:
	Vector2* AddPoints(size_t count)
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

Figure::Figure()
{
}

void Figure::StartAt(float x, float y)
{
}

void Figure::StartAt(const Vector2& pt)
{
}

void Figure::LineTo(const Vector2& pt)
{
}

void Figure::LineTo(const Vector2* pts, int count)
{
}

void Figure::BezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2 endPt)
{
}

void Figure::BezierTo(const Vector2* pts, int count)
{
}

void Figure::Reset()
{
}

void Figure::Close()
{
}

void Figure::StartAt(const Vector2 pt)
{
}
