#include "pch.h"
#include "ProgramDemo.h"
#include "Spirit.h"
#include "Particle.h"

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
	auto bk = agate::Texture::CreateFromFile(GetModulePath() + L"bk.jpg");
	auto& imgbk = bk->GetImageData();
	_Background.SetTexture(bk);
	agate::Vector4 bounds{ .0f, .0f, (float)imgbk.width, (float)imgbk.height };
	_Background.SetClip(bounds);
	_Background.SetBounds(bounds);

	_BeginTime = std::chrono::steady_clock::now();
	//AddSpirit();
	AddParticle();
	AddTextureParticle();
}

void ProgramDemo::Render(agate::DrawingContext& canvs)
{
	auto now = std::chrono::steady_clock::now();
	auto nowTick = std::chrono::duration_cast<std::chrono::milliseconds>(now - _BeginTime).count();
	canvs.BeginDraw(true, 0xFF000000);
	//canvs.Draw(_Background);
	_Program.Update(nowTick);
	_Program.Draw(canvs);
	canvs.EndDraw(1);
	if (nowTick > 10000)
	{
		//_BeginTime = now;
	}
}

void ProgramDemo::AddSpirit()
{
	auto resPath = GetModulePath();
	//from_to 动画
	{
		auto spirit = std::make_shared<agate::Spirit>();
		agate::RenderParameter render;
		render.filePath = resPath + L"R.jpg";
		spirit->SetRenderParams(render);
		agate::RotationAnimationParameter rotation{};
		rotation.type = agate::RotationAnimationType::FromTo;
		rotation.params.from = 0.f;
		rotation.params.to = 1.45f;
		rotation.center.x = 200.f;
		rotation.center.y = 100.f;
		spirit->SetRotation(rotation);

		agate::ScalingAnimationParameter scaling{};
		scaling.type = agate::ScalingAnimationType::FromTo;
		scaling.params.from = { 1.0f, 1.0f };
		scaling.params.to = { 2.0f, 2.0f };
		spirit->SetScaling(scaling);

		spirit->Range(100, 10 * 1000);
		_Program.AddSpirit(spirit);
	}
	//速度，加速动画
	{
		auto spirit = std::make_shared<agate::Spirit>();
		agate::RenderParameter render;
		render.filePath = resPath + L"Wind01.png";
		spirit->SetRenderParams(render);

		agate::RotationAnimationParameter rotation{};
		rotation.type = agate::RotationAnimationType::PVA;
		rotation.params.base = 0.2f;
		rotation.params.velocity = 0.1f;
		rotation.params.acceleration = 0.3f;
		rotation.center.x = 64.f;
		rotation.center.y = 64.f;

		agate::ScalingAnimationParameter scaling{};
		scaling.type = agate::ScalingAnimationType::PVA;
		scaling.params.base = { 1.0f, 1.0f };
		scaling.params.velocity = { 0.1f, 0.1f };
		scaling.params.acceleration = { 0.1f, 0.1f };
		spirit->SetScaling(scaling);

		spirit->SetBlendMode(agate::BlendMode::Additive);
		spirit->SetRotation(rotation);
		spirit->Range(1000, 10 * 1000);
		_Program.AddSpirit(spirit);
	}

	//移动
	{
		auto spirit = std::make_shared<agate::Spirit>();
		agate::RenderParameter render;
		render.filePath = resPath + L"Wind01.png";
		spirit->SetRenderParams(render);
		agate::TranslateAnimationParameter translate;
		translate.type = agate::TranslateAnimationType::PVA;
		translate.params.base = { 100.f, 100.f };
		translate.params.velocity = { 200.f, 200.f };
		translate.params.acceleration = { 0.0f, 100.0f };
		spirit->SetTranslate(translate);


		spirit->SetBlendMode(agate::BlendMode::Additive);

		spirit->Range(0, 10 * 1000);
		_Program.AddSpirit(spirit);
	}

	{
		auto spirit = std::make_shared<agate::Spirit>();
		agate::RenderParameter render;
		render.filePath = resPath + L"Wind01.png";
		spirit->SetRenderParams(render);

		agate::ColorAnimationParameter color;
		color.type = agate::ColorAnimationType::FromTo;
		color.params.from = { 0xFFFFFFFF };
		color.params.to = { 0xFF000000 };
		spirit->SetColor(color);
		spirit->SetBlendMode(agate::BlendMode::Additive);
		spirit->Range(0, 5 * 1000);
		_Program.AddSpirit(spirit);
	}
}

void ProgramDemo::AddParticle()
{
	auto resPath = GetModulePath();
	//fromto 精灵
	{
		auto particle = std::make_shared<agate::Particles>();
		agate::ParticleParameter params;
		params.particleCount = 10;
		params.generateInterval.min = 50;
		params.generateInterval.max = 200;
		params.particleLife.min = 6000;
		params.particleLife.max = 6000;
		particle->SetParams(params);

		agate::ParticleTranslateParameter translate;
		translate.type = agate::TranslateAnimationType::FromTo;
		translate.params.from.min = { 0.0f, 0.0f };
		translate.params.from.max = { 800.0f, 0.0f };
		translate.params.to.min = { 0.0f, 400.0f };
		translate.params.to.max = { 800.0f, 400.0f };
		particle->SetTranslate(translate);

		agate::ParticleScalingParameter scaling;
		scaling.type = agate::ScalingAnimationType::UniformRandom;
		scaling.params.uniformRandom.min = 0.4f;
		scaling.params.uniformRandom.max = 0.5f;
		particle->SetScaling(scaling);

		agate::ParticlColorParameter color;
		color.type = agate::ColorAnimationType::FromTo;
		color.params.from.min = { 0xFFFF00FF };
		color.params.from.max = { 0xFF00FF00 };
		color.params.to.min = { 0x00000000 };
		color.params.to.max = { 0x00000000 };
		particle->SetColor(color);

		agate::RenderParameter render;
		render.filePath = resPath + L"Particle01.png";
		render.blend = agate::BlendMode::Additive;
		particle->SetRenderParams(render);
		_Program.AddSpirit(particle);
	}

	//scale 精灵
	{
		auto particle = std::make_shared<agate::Particles>();
		agate::ParticleParameter params;
		params.particleCount = 20;
		params.generateInterval.min = 50;
		params.generateInterval.max = 200;
		params.particleLife.min = 500;
		params.particleLife.max = 1000;
		particle->SetParams(params);

		agate::ParticleTranslateParameter translate;
		translate.type = agate::TranslateAnimationType::Fixed;
		translate.params.fixed = { 900.0f, 200.0f };
		particle->SetTranslate(translate);

		agate::ParticleScalingParameter scale;
		scale.type = agate::ScalingAnimationType::UniformPVA;
		scale.params.uniformBase.min = 0.2f;
		scale.params.uniformBase.max = 0.5f;
		scale.params.uniformVelocity.min = 2.2f;
		scale.params.uniformVelocity.max = 3.2f;
		scale.params.uniformAcceleration.min = 1.1f;
		scale.params.uniformAcceleration.max = 3.2f;
		particle->SetScaling(scale);

		agate::RenderParameter render;
		render.filePath = resPath + L"Particle01.png";
		render.blend = agate::BlendMode::Additive;
		particle->SetRenderParams(render);
		_Program.AddSpirit(particle);
	}

	{
		auto particle = std::make_shared<agate::Particles>();
		agate::ParticleParameter params;
		params.particleCount = 400;
		params.infinite = false;
		params.generateInterval.min = 200;
		params.generateInterval.max = 300;
		params.particleLife.min = 2000;
		params.particleLife.max = 3000;
		particle->SetParams(params);

		agate::ParticleTranslateParameter translate;
		translate.type = agate::TranslateAnimationType::PVA;
		translate.params.velocity.min = { 200.0f, - 430.0f };
		translate.params.velocity.max = { 350.0f, - 534.0f };
		translate.params.acceleration.min = { -40.0f, 250.0f };
		translate.params.acceleration.max = { 40.0f, 250.0f };
		translate.params.base.min = translate.params.base.max = { 400.0f, 600.0f };
		particle->SetTranslate(translate);

		agate::ParticleScalingParameter scaling;
		scaling.type = agate::ScalingAnimationType::UniformRandom;
		scaling.params.uniformRandom.min = 0.4f;
		scaling.params.uniformRandom.max = 0.5f;
		particle->SetScaling(scaling);

		agate::ParticlColorParameter color;
		color.type = agate::ColorAnimationType::Random;
		color.params.random.min = { 0xFFFF00FF };
		color.params.random.max = { 0xFF00FF00 };
		particle->SetColor(color);

		agate::RenderParameter render;
		render.filePath = resPath + L"Particle01.png";
		render.blend = agate::BlendMode::Additive;
		particle->SetRenderParams(render);
		_Program.AddSpirit(particle);
	}
}

void ProgramDemo::AddTextureParticle()
{
	auto resPath = GetModulePath();
	{
		auto particle = std::make_shared<agate::Particles>();
		agate::ParticleParameter params;
		params.particleCount = 1;
		params.infinite = false;
		params.generateInterval.min = 10;
		params.generateInterval.max = 10;
		params.particleLife.min = 100000;
		params.particleLife.max = 100000;
		particle->SetParams(params);

		agate::ParticleTranslateParameter translate;
		translate.type = agate::TranslateAnimationType::Fixed;
		translate.params.fixed = { 400, 400 };
		particle->SetTranslate(translate);

		agate::ParticleScalingParameter scaling;
		scaling.type = agate::ScalingAnimationType::Fixed;
		scaling.params.fixed = { 1.0f, 1.0f };
		particle->SetScaling(scaling);

		agate::ParticlColorParameter color;
		color.type = agate::ColorAnimationType::Random;
		color.params.random.min = { 0xFFFF00FF };
		color.params.random.max = { 0xFF00FF00 };
		//particle->SetColor(color);

		agate::RenderParameter render;
		render.filePath = resPath + L"man.png";
		render.blend = agate::BlendMode::Blend;
		render.size = { 150.f, 291.0f };
		particle->SetRenderParams(render);

		agate::ParticleTextureParameter texture;
		texture.type = agate::TextureAnimationType::FramebyFrame;
		texture.UVFrame = {0.0f, 0.0f, 150.0f / 1055.0f, 291.0f / 582.0f};
		texture.params.frameCount = 14;
		texture.params.lineFrameCount = 7;
		texture.params.stepX = 150.f / 1055.0f;
		texture.params.stepY = 291.0f / 582.0f;
		texture.params.frameRate = 3;
		particle->SetTexture(texture);
		_Program.AddSpirit(particle);
	}
}

