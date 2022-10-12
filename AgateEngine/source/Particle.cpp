#include "Particle.h"

namespace agate
{
    Particles::Particles()
    {
        _Params.generateInterval.min = 10;
        _Params.generateInterval.max = 1000;
        _Params.particleCount = 100;
        _Params.particleLife.min = 1000;
        _Params.particleLife.max = 2000;
        _Translate.type = TranslateAnimationType::Fixed;
        _Translate.params.fixed = { 0.0f, 0.0f };
        _Scaling.type = ScalingAnimationType::Fixed;
        _Scaling.params.fixed = { 1.0f, 1.0f };
        _Rotation.type = RotationAnimationType::Fixed;
        _Rotation.Params.fixed = 0.0f;
        _Color.type = ColorAnimationType::Fixed;
        _Color.params.fixed = { 0xFFFFFFFF };
    }
    void Particles::SetTranslate(const ParticleTranslateParameter& param)
    {
        _Translate = param;
    }

    bool agate::Particles::Update(int64_t time)
    {
        if (time < _Begin)
        {
            return false;
        }
        GenerateInstances();
        _ShowingParticles.clear();
        for (auto& spirite : _Particles)
        {
            if (spirite->Update(time))
            {
                _ShowingParticles.push_back(spirite.get());
            }
        }
        return true;
    }

    void agate::Particles::Draw(DrawingContext& context)
    {
        for (auto& spirite : _ShowingParticles)
        {
            spirite->Draw(context);
        }
    }

    void Particles::SetTexture(const std::wstring& file)
    {
        _Texture = file;
    }

    void Particles::GenerateInstances()
    {
        uint32_t begin = 0;
        while (_Particles.size() < _Params.particleCount)
        {
            auto spirite = std::make_shared<Spirit>();
            spirite->SetSource(_Texture);
            spirite->SetBlendMode(BlendMode::Additive);
            begin += _Params.generateInterval.Random(_Random);
            spirite->Range(begin, begin + _Params.particleLife.Random(_Random));
            AssignTranslate(spirite.get());
            AssignScaling(spirite.get());
            AssignRotation(spirite.get());
            AssignColor(spirite.get());
            _Particles.emplace_back(spirite);
        }
    }

    void Particles::AssignTranslate(Spirit* spirit)
    {
        TranslateAnimationParameter translate;
        translate.type = _Translate.type;
        switch (_Translate.type)
        {
        case(TranslateAnimationType::Fixed):
            translate.params.fixed = _Translate.params.fixed;
            break;
        case(TranslateAnimationType::Random):
            translate.type = TranslateAnimationType::Fixed;
            translate.params.fixed = _Translate.params.random.Random(_Random);
            break;
        case(TranslateAnimationType::FromTo):
            translate.params.from = _Translate.params.from.Random(_Random);
            translate.params.to = _Translate.params.to.Random(_Random);
            break;
        case(TranslateAnimationType::PVA):
            translate.params.base = _Translate.params.base.Random(_Random);
            translate.params.velocity = _Translate.params.velocity.Random(_Random);
            translate.params.acceleration = _Translate.params.acceleration.Random(_Random);
            break;
        case(TranslateAnimationType::DirectionPVA):
        {
            translate.type = TranslateAnimationType::PVA;
            translate.params.base = _Translate.params.base.Random(_Random);
            MinMax<float> direct = _Translate.params.direction;
            if (direct.max < direct.min)
            {
                direct.max += 360.f;
            }
            float angle = direct.Random(_Random) * PI / 180.f;
            float y = sinf(angle);
            float x = cosf(angle);
            float velocity = _Translate.params.dir_velocity.Random(_Random);
            float accele = _Translate.params.dir_acceleration.Random(_Random);
            translate.params.velocity = { velocity * x, velocity * y };
            translate.params.acceleration = { accele * x, accele * y };
        }
        break;
        default:
            assert(false);
            break;
        }
        spirit->SetTranslate(translate);
    }

    void Particles::AssignScaling(Spirit* spirit)
    {
        ScalingAnimationParameter scal;
        scal.type = _Scaling.type;
        switch (_Scaling.type)
        {
        case (ScalingAnimationType::Fixed):
            scal.params.fixed = _Scaling.params.fixed;
            break;
        case(ScalingAnimationType::UniformRandom):
            scal.type = ScalingAnimationType::Fixed;
            scal.params.fixed.x = scal.params.fixed.y = _Scaling.params.uniformRandom.Random(_Random);
            break;
        case(ScalingAnimationType::UniformFromTo):
            scal.params.uniformFrom = _Scaling.params.uniformFrom.Random(_Random);
            scal.params.uniformTo = _Scaling.params.uniformTo.Random(_Random);
            break;
        case(ScalingAnimationType::UniformPVA):
            scal.params.uniformBase = _Scaling.params.uniformBase.Random(_Random);
            scal.params.uniformVelocity = _Scaling.params.uniformVelocity.Random(_Random);
            scal.params.uniformAcceleration = _Scaling.params.uniformAcceleration.Random(_Random);
            break;
        case(ScalingAnimationType::Random):
            scal.type = ScalingAnimationType::Fixed;
            scal.params.fixed = _Scaling.params.random.Random(_Random);
            break;
        case(ScalingAnimationType::FromTo):
            scal.params.from = _Scaling.params.from.Random(_Random);
            scal.params.to = _Scaling.params.to.Random(_Random);
            break;
        case(ScalingAnimationType::PVA):
            scal.params.base = _Scaling.params.base.Random(_Random);
            scal.params.velocity = _Scaling.params.velocity.Random(_Random);
            scal.params.acceleration = _Scaling.params.acceleration.Random(_Random);
            break;
        default:
            assert(false);
            break;
        }
        spirit->SetScaling(scal);
    }

    void Particles::AssignRotation(Spirit* spirit)
    {
        RotationAnimationParameter rotation;
        rotation.type = _Rotation.type;
        switch (_Rotation.type)
        {
        case(RotationAnimationType::Fixed):
            rotation.params.fixed = _Rotation.Params.fixed;
            break;
        case(RotationAnimationType::Random):
            rotation.type = RotationAnimationType::Fixed;
            rotation.params.fixed = _Rotation.Params.random.Random(_Random);
            break;
        case(RotationAnimationType::FromTo):
            rotation.params.from = _Rotation.Params.from.Random(_Random);
            rotation.params.to = _Rotation.Params.to.Random(_Random);
            break;
        case(RotationAnimationType::PVA):
            rotation.params.base = _Rotation.Params.base.Random(_Random);
            rotation.params.velocity = _Rotation.Params.velocity.Random(_Random);
            rotation.params.acceleration = _Rotation.Params.acceleration.Random(_Random);
            break;
        default:
            assert(false);
            break;
        }
        spirit->SetRotation(rotation);
    }

    void Particles::AssignColor(Spirit* spirit)
    {
        ColorAnimationParameter color;
        switch (_Color.type)
        {
        case(ColorAnimationType::Fixed):
            color.type = ColorAnimationType::Fixed;
            color.params.fixed = _Color.params.fixed;
            break;
        case(ColorAnimationType::Random):
            color.type = ColorAnimationType::Fixed;
            color.params.fixed = _Color.params.random.Random(_Random);
            break;
        case(ColorAnimationType::FromTo):
            color.type = ColorAnimationType::FromTo;
            color.params.from = _Color.params.from.Random(_Random);
            color.params.to = _Color.params.to.Random(_Random);
            break;
        default:
            assert(false);
            break;
        }
        spirit->SetColor(color);
    }
    

}