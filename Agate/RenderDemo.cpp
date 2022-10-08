#include "pch.h"
#include "RenderDemo.h"

RenderDemo::RenderDemo():_Scale{1.0f, 1.0f}
{
	wchar_t pathBuffer[256];
	GetModuleFileName(0, pathBuffer, 256);
	std::wstring path(pathBuffer);
	auto pos = path.rfind(LR"(\)");
	path = path.substr(0, pos + 1);


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

	_Image = Image::CreateFromFile(path + L"R.jpg");
	_Spirit.SetImage(_Image);
	auto& img = _Image->GetImageData();
	Vector4 bounds{};
	bounds.pos = { 200.f, 100.f };
	bounds.size = { (float)img.width, (float)img.height };
	_Spirit.SetClip(Vector4(0.f, 0.f, img.width, img.height));
	_Spirit.SetBounds(bounds);
}

void RenderDemo::Render(DrawingContext& canvs)
{
	canvs.BeginDraw(true, 0xFFFFFFFF);
	//RenderGeomegry(canvs);
	RenderSpirit(canvs);
	canvs.EndDraw(1);
}

void RenderDemo::RenderGeomegry(DrawingContext& canvs)
{
	_Rotation = min(_Rotation + 0.02f, 3.1445f);
	_Scale.x = min(_Scale.x + 0.01f, 2);
	_Scale.y = min(_Scale.y + 0.02f, 2);
	
	auto matrix = Matrix3X2::Rotation(_Rotation, Vector2(300, 250));
	_Line.SetTransform(matrix);
	canvs.Draw(_Bezier);
	auto matrix2 = Matrix3X2::Rotation(_Rotation, Vector2(150, 400)) * Matrix3X2::Scale(_Scale.x, _Scale.y);
	_Ellipse.SetTransform(matrix2);
	canvs.Draw(_Ellipse);
	canvs.Draw(_Rectangle);
	//canvs.SetClip({ 250, 20, 390, 500 });
	canvs.Draw(_RoundedRect);
	canvs.Draw(_Line);
}

void RenderDemo::RenderSpirit(DrawingContext& canvs)
{
	//_Rotation = min(_Rotation + 0.02f, 3.1445f);
	_Rotation = 0.3;
	//_Rotation += 0.01f;
	if (_Rotation >= 3.14159265f)
	{
		_Rotation = 0;
	}
	auto matrix = Matrix3X2::Rotation(_Rotation, Vector2(300, 250));
	//_Spirit.SetTransform(matrix);
	canvs.Draw(_Spirit);
}
