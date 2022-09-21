#pragma once

#include "Vector.h"
#include "Verext.h"

/// <summary>
/// ÃèÊöÏß¶Î×éºÏ
/// </summary>
/// 
class FigureData;

class Figure
{
public:
	Figure();
	
	void StartAt(float x, float y);

	void StartAt(const Vector2& pt);

	void LineTo(const Vector2& pt);

	void LineTo(const Vector2* pts, int count);

    void BezierTo(const Vector2& ctr1, const Vector2& ctr2, const Vector2 endPt);

	void BezierTo(const Vector2* pts, int count);

	void Reset();

	void Close();

private:
	FigureData* _Data;
};

class Geometry
{

};

