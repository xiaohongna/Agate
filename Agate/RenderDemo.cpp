#include "pch.h"
#include "RenderDemo.h"

RenderDemo::RenderDemo()
{
	_Rotation = 0.0;

	Brush pureColor;
	
	{
		Figure figure;
		figure.StartAt(200, 200);
		figure.LineTo({ 400, 300 });
		_Line.SetStrokeWidth(5);
		pureColor.SetColor(0xFFFF0000);
		_Line.SetStrokeBrush(pureColor);
		_Line.AddFigure(std::move(figure));
	}

	{
		Figure figure;
		figure.StartAt(156, 257);
		figure.BezierTo({ 51, 278 }, {252, 114}, {139, 107});
		pureColor.SetColor(0xFF00FF00);
		_Bezier.SetStrokeBrush(pureColor);
		_Bezier.SetStrokeWidth(2);
		_Bezier.AddFigure(std::move(figure));
	}

	_Rectangle.AddFigure(Figure::InitAsRectangle(Vector4(50, 20, 190, 150)));
	pureColor.SetColor(0xFF0000FF);
	_Rectangle.SetFillBrush(pureColor);

	_RoundedRect.AddFigure(Figure::InitAsRoundedRectangle(Vector4(230.5f, 10.5f, 380.5, 180.5), 20, 20));
	pureColor.SetColor(0x5F0000FF);
	_RoundedRect.SetFillBrush(pureColor);
	
	_Ellipse.AddFigure(Figure::InitAsEllipse(150, 400, 100, 20));
	_Ellipse.SetFillBrush(pureColor);

}

void RenderDemo::Render(DrawingContext& canvs)
{
	_Rotation += 0.02;
	canvs.BeginDraw(true, 0xFFFFFFFF);
	auto matrix = Matrix3X2::Rotation(_Rotation, Vector2(300, 250));
	_Line.SetTransform(matrix);
	canvs.Draw(_Bezier);
	auto matrix2 = Matrix3X2::Rotation(_Rotation, Vector2(150, 400));
	_Ellipse.SetTransform(matrix2);
	canvs.Draw(_Ellipse);
	canvs.Draw(_Rectangle);
	//canvs.SetClip({ 250, 20, 390, 500 });
	canvs.Draw(_RoundedRect);
	canvs.Draw(_Line);
	canvs.EndDraw(1);
}
