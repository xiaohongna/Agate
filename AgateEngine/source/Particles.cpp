#include "Particles.h"

namespace agate
{
    Particles::Particles()
    {
        _ParticleCount = 0;
        _LastParticleBeginning = 0;
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
        _Texture.type = TextureAnimationType::Fixed;
        _Texture.UVFrame = {0.0f, 0.0f, 1.0f, 1.0f};
    }

    int agate::Particles::Update(int64_t time)
    {
        if (time < _Begin)
        {
            return -1;
        } 
        GenerateInstances(time);
        _ShowingParticles.clear();
        auto spirite = _Particles.begin();
        while (spirite != _Particles.end())
        {
            int ret = (*spirite)->Update(time);
            if (ret > 0)
            {
               spirite = _Particles.erase(spirite);
            }
            else if(ret == 0)
            {
                _ShowingParticles.push_back(spirite->get());
                spirite++;
            }
            else
            {
                break;
            }
        }
        auto ret = SpiritBase::Update(time);
        if (_ShowingParticles.empty() == false)   //有正在渲染的精灵
        {
            return 0; 
        }
        return ret;
    }

    void agate::Particles::Draw(DrawingContext& context)
    {
        for (auto& spirite : _ShowingParticles)
        {
            spirite->Draw(context);
        }
    }

    void Particles::GenerateInstances(int64_t time)
    {
        //最后的元素已经大于当前时间，没必要更多精灵
        while (_LastParticleBeginning < time && (_Params.infinite || _ParticleCount < _Params.particleCount))
        {
            _LastParticleBeginning += _Params.generateInterval.Random(_Random);
            _End = _LastParticleBeginning + _Params.particleLife.Random(_Random);
            auto spirite = std::make_shared<Spirit>(_LastParticleBeginning, _End);
            spirite->SetRenderParams(_RenderParams);
            //spirite->Range(time, _End);
            //spirite->AddSpirit();
            AssignTranslate(spirite.get());
            AssignScaling(spirite.get());
            AssignRotation(spirite.get());
            AssignColor(spirite.get());
            AssignTexture(spirite.get());
            _Particles.emplace_back(spirite);
            assert(_Particles.size() < 1000);
            _ParticleCount++;
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

    void Particles::AssignTexture(Spirit* spirit)
    {
        TextureAnimationParameter texture = _Texture;
        if (_Texture.type == TextureAnimationType::FrameRandom)
        {
            texture.type = TextureAnimationType::Fixed;
            int frameIndex = _Random.GetRand(0.0f, _Texture.params.frameCount);
            int x = (frameIndex % _Texture.params.lineFrameCount) * _Texture.UVFrame.Width();
            int y = frameIndex / _Texture.params.lineFrameCount * _Texture.UVFrame.Height();
            texture.UVFrame.x += x;
            texture.UVFrame.y += y;
            texture.UVFrame.right += x;
            texture.UVFrame.bottom += y;
        }
        spirit->SetTexture(texture);
    }
    

}