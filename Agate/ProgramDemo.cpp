#include "pch.h"
#include "ProgramDemo.h"

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
	//AddParticle();
	//AddTextureParticle();
	BuildComponent();
}

void ProgramDemo::Render(agate::DrawingContext& canvs)
{
	auto now = std::chrono::steady_clock::now();
	auto nowTick = std::chrono::duration_cast<std::chrono::milliseconds>(now - _BeginTime).count();
	canvs.BeginDraw(true, 0xFF000000);
	//canvs.Draw(_Background);
	//_Program.Update(nowTick);
	//_Program.Draw(canvs);
	_Particle.Update(nowTick);
	_Particle.Draw(canvs);
	canvs.EndDraw(1);
	if (nowTick > 10000)
	{
		//_BeginTime = now;
	}
}

void ProgramDemo::BuildComponent()
{
	auto resPath = GetModulePath();
	{
		
		auto params = std::make_shared<agate::ParticleParameter>();
		params->particleCount = 1;
		params->generateInterval.min = 50;
		params->generateInterval.max = 200;
		params->particleLife.min = 6000;
		params->particleLife.max = 6000;
		params->infinite = false;
		params->inherite = agate::InheriteBehavior::Never;

		params->rotation.type = agate::RotationAnimationType::Fixed;
		params->rotation.params.fixed = 0.0f;

		agate::ParticleTranslateParameter& translate = params->translate;
		translate.type = agate::TranslateAnimationType::FromTo;
		translate.params.from.min = { 0.0f, 0.0f };
		translate.params.from.max = { 0.f, 0.0f };
		translate.params.to.min = { 800.0f, 0.0f };
		translate.params.to.max = { 800.0f, 0.0f };

		agate::ParticleScalingParameter& scaling = params->scaling;
		scaling.type = agate::ScalingAnimationType::UniformRandom;
		scaling.params.uniformRandom.min = 0.4f;
		scaling.params.uniformRandom.max = 0.5f;

		agate::ParticlColorParameter& color = params->color;
		color.type = agate::ColorAnimationType::Random;
		
		color.params.from.min = { 0xFFFF00FF };
		color.params.from.max = { 0xFF00FF00 };
		color.params.to.min = { 0xFFFF00FF };
		color.params.to.max = { 0xFF00FF00 };
		
		agate::ParticleTextureParameter& texture = params->texture;
		texture.type = agate::TextureAnimationType::Fixed;
		texture.UVFrame = {0.0f, 0.0f, 1.0f, 1.0f};

		agate::RenderParameter& render = params->render;
		render.filePath = resPath + L"Particle01.png";
		render.blend = agate::BlendMode::Additive;
		_Particle.SetParams(params);
	}
	//всаёвс
	{
		auto params = std::make_shared<agate::ParticleParameter>();
		auto particle = std::make_shared<agate::ParticleComponent>(params);
		
		params->particleCount = 1;
		params->generateInterval.min = 30;
		params->generateInterval.max = 100;
		params->particleLife.min = 1000;
		params->particleLife.max = 2000;
		params->infinite = true;
		params->bindParent = true;
		params->inherite = agate::InheriteBehavior::PositionAlways;

		params->rotation.type = agate::RotationAnimationType::Fixed;
		params->rotation.params.fixed = 0.0f;

		agate::ParticleTranslateParameter& translate = params->translate;
		translate.type = agate::TranslateAnimationType::DirectionPVA;
		translate.params.direction.min = 0.0f;
		translate.params.direction.max = 360.f;
		translate.params.dir_velocity.min = 1000.f;
		translate.params.dir_velocity.max = 2000.f;
		translate.params.dir_acceleration.min = 1000.0f;
		translate.params.dir_acceleration.min = 2000.0f;

		agate::ParticleScalingParameter& scaling = params->scaling;
		scaling.type = agate::ScalingAnimationType::UniformRandom;
		scaling.params.uniformRandom.min = 0.2f;
		scaling.params.uniformRandom.max = 0.3f;

		agate::ParticlColorParameter& color = params->color;
		color.type = agate::ColorAnimationType::FromTo;
		color.params.from.min = { 0xFFFF00FF };
		color.params.from.max = { 0xFF00FF00 };
		color.params.to.min = { 0xFF000000 };
		color.params.to.max = { 0xFFFFFFFF };
		

		agate::ParticleTextureParameter& texture = params->texture;
		texture.type = agate::TextureAnimationType::Fixed;
		texture.UVFrame = { 0.0f, 0.0f, 1.0f, 1.0f };

		agate::RenderParameter& render = params->render;
		render.filePath = resPath + L"Particle01.png";
		render.blend = agate::BlendMode::Additive;
		_Particle.AddChild(std::move(particle));
	}
}

