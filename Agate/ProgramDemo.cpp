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
	//BuildComponent();
	LifecycleTest();
}
static uint64_t begin = 0;
void ProgramDemo::Render(agate::DrawingContext& canvs)
{
	if (_Particle.Validate() == false)
	{
		return;
	}
	auto now = std::chrono::steady_clock::now();
	//auto nowTick = std::chrono::duration_cast<std::chrono::milliseconds>(now - _BeginTime).count();
	begin += 16;
	canvs.BeginDraw(true, 0xFF000000);
	//canvs.Draw(_Background);
	//_Program.Update(nowTick);
	//_Program.Draw(canvs);
	auto result = _Particle.Update(begin);
	switch (result)
	{
	case agate::UpdateResult::Nothing:
		break;
	case agate::UpdateResult::NeedRender:
		_Particle.Draw(canvs);
		break;
	case agate::UpdateResult::Destroyable:
		_Particle.Reset();
		_BeginTime = now;
		begin = 0;
		break;
	default:
		break;
	}
	canvs.EndDraw(1);
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

void ProgramDemo::LifecycleTest()
{
	auto resPath = GetModulePath();
	{
		auto params = std::make_shared<agate::ParticleParameter>();
		params->particleCount = 3;
		params->generateInterval.min = 200;
		params->generateInterval.max = 500;
		params->particleLife.min = 300;
		params->particleLife.max = 800;
		params->infinite = false;
		params->inherite = agate::InheriteBehavior::Never;

		agate::ParticleTranslateParameter& translate = params->translate;
		translate.type = agate::TranslateAnimationType::PVA;
		translate.params.base.min = {400, 850};
		translate.params.base.max = { 400, 850 };
		translate.params.velocity.min = {-10, -1000};
		translate.params.velocity.max = { 10, -1300 };
		translate.params.acceleration.max = { 0, 1300 };
		translate.params.acceleration.min = { 0, 1300 };

		agate::RenderParameter& render = params->render;
		render.filePath = resPath + L"Particle01.png";
		render.blend = agate::BlendMode::Additive;
		_Particle.SetParams(params);
	}
	{
		auto params = std::make_shared<agate::ParticleParameter>();
		auto particle = std::make_shared<agate::ParticleComponent>(params);
		
		params->particleCount = 80;
		params->initialDelay = 0;
		params->generateInterval.min = 0;
		params->generateInterval.max = 0;
		params->particleLife.min = 600;
		params->particleLife.max = 800;
		params->infinite = false;
		params->bindParent = true;
		params->startTigger = agate::SpawnStartTrigger::ParentEnding;
		params->inherite = agate::InheriteBehavior::PositionOnCreate;

		agate::ParticleScalingParameter& scaling = params->scaling;
		scaling.type = agate::ScalingAnimationType::UniformRandom;
		scaling.params.uniformRandom.min = 0.2f;
		scaling.params.uniformRandom.max = 0.3f;

		agate::ParticleTranslateParameter& translate = params->translate;
		translate.type = agate::TranslateAnimationType::DirectionPVA;
		translate.params.base.max = {-20, -20};
		translate.params.base.min = { -20, -20 };
		translate.params.direction.min = 0.0f;
		translate.params.direction.max = 360.f;
		translate.params.dir_velocity.min = 400.f;
		translate.params.dir_velocity.max = 800.f;
		translate.params.dir_acceleration.min = -400.0f;
		translate.params.dir_acceleration.min = -800.0f;

		agate::ParticlColorParameter& color = params->color;
		color.type = agate::ColorAnimationType::FromTo;
		color.params.from.min = { 0xFFFF00FF };
		color.params.from.max = { 0xFF00FF00 };
		color.params.to.min = { 0xFF000000 };
		color.params.to.max = { 0xFFFFFFFF };

		agate::RenderParameter& render = params->render;
		render.blend = agate::BlendMode::Additive;
		render.filePath = resPath + L"Particle01.png";
		_Particle.AddChild(std::move(particle));
	}
}

