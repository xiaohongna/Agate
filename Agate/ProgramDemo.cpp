#include "pch.h"
#include "ProgramDemo.h"
#include "Spirit.h"

std::wstring GetModulePath()
{
	wchar_t pathBuffer[256];
	GetModuleFileName(0, pathBuffer, 256);
	std::wstring path(pathBuffer);
	auto pos = path.rfind(LR"(\)");
	return path.substr(0, pos + 1);
}

int64_t GetNowTick()
{
	auto now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

ProgramDemo::ProgramDemo()
{
	_BeginTime = std::chrono::steady_clock::now();
	AddSpirit();
}


void ProgramDemo::Render(agate::DrawingContext& canvs)
{
	auto now = std::chrono::steady_clock::now();
	auto nowTick = std::chrono::duration_cast<std::chrono::milliseconds>(now - _BeginTime).count();
	canvs.BeginDraw(true, 0xFFFFFFFF);
	_Program.Update(nowTick);
	_Program.Draw(canvs);
	canvs.EndDraw(1);
}

void ProgramDemo::AddSpirit()
{
	auto spirit = std::make_shared<agate::Spirit>();
	GetModulePath();
	spirit->SetSource(GetModulePath() + L"R.jpg");
	agate::RotationAnimationParameter rotation{};
	rotation.type = agate::RotationAnimationType::FromTo;
	rotation.Params.from = 0.f;
	rotation.Params.to = 1.45f;
	rotation.pointer.x = 200.f;
	rotation.pointer.y = 100.f;
	
	spirit->SetRotation(rotation);
	spirit->Range(100, 10 * 1000);
	_Program.AddSpirit(spirit);
}
