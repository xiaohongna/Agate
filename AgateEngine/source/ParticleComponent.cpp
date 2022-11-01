#include "ParticleComponent.h"

namespace agate
{
	ParticleComponent::ParticleComponent()
	{
        _ParticleCount = 0;
        _LastParticleBeginning = 0;
        _BaseScale = 1.0f;
	}

    ParticleComponent::ParticleComponent(std::shared_ptr<ParticleParameter>& param)
    {
        _ParticleCount = 0;
        _LastParticleBeginning = 0;
        _Params = param;
    }

    UpdateResult ParticleComponent::Update(std::shared_ptr<Sprite>& parent, int64_t time)
    {
        if (time < _Params->initialDelay)
        {
            return UpdateResult::Nothing;
        }
        GenerateInstances(parent, time);
        _ShowingParticles.clear();
        auto sprite = _Particles.begin();
        while (sprite != _Particles.end())
        {
            auto result = (*sprite)->Update(time);
            switch (result)
            {
            case agate::UpdateResult::Nothing:
                sprite++;
                break;
            case agate::UpdateResult::NeedRender:
                _ShowingParticles.push_back(sprite->get());
                sprite++;
                break;
            case agate::UpdateResult::Destroyable:
                sprite = _Particles.erase(sprite);
                break;
            default:
                break;
            }
        }
        auto childResult = UpdateChildren(time);
        if (_ShowingParticles.empty() == false || childResult == UpdateResult::NeedRender)
        {
            return UpdateResult::NeedRender;
        }
        else if (_Particles.empty() == false)
        {
            return UpdateResult::Nothing;
        }
        else
        {
            return UpdateResult::Destroyable;
        }
    }

    UpdateResult ParticleComponent::Update(int64_t time)
    {
        auto parent = std::shared_ptr<Sprite>();
        return Update(parent, time);
    }
    void ParticleComponent::Draw(DrawingContext& context)
    {
        for (auto& spirite : _ShowingParticles)
        {
            spirite->Draw(context);
        }
        for (auto& spirite : _ShowingParticles)
        {
            spirite->DrawChild(context);
        }
        for (auto& child : _Children)
        {
            child->Draw(context);
        }
    }

    std::shared_ptr<ParticleComponent> ParticleComponent::ReferenceClone()
    {
        auto instance = std::make_shared<ParticleComponent>(_Params);
        for (auto& child : _Children)
        {
            instance->AddChild(std::move(child->ReferenceClone()));
        }
        return instance;
    }

    void ParticleComponent::Reset()
    {
        _Particles.clear();
        _ShowingParticles.clear();
        _ParticleCount = 0;
        _LastParticleBeginning = 0;
        for (auto& item : _Children)
        {
            item->Reset();
        }
    }

    UpdateResult ParticleComponent::UpdateChildren(int64_t time)
    {
        if (_Children.empty())
        {
            return UpdateResult::Destroyable;
        }

        UpdateResult result = UpdateResult::Nothing;
        for (auto& child : _Children)
        {
            if (child.get()->_Params->bindParent == false)
            {
                result = result | child->Update(time);
            }
        }
        if (result == UpdateResult::Nothing)
        {
            return result;
        }
        else if ((result & UpdateResult::NeedRender) == UpdateResult::NeedRender)
        {
            return UpdateResult::NeedRender;
        }
        else
        {
            return UpdateResult::Destroyable;
        }
    }
    void ParticleComponent::GenerateInstances(std::shared_ptr<Sprite>& parent, int64_t time)
	{
        //最后的元素已经大于当前时间，没必要更多精灵
        while (_LastParticleBeginning < time && (_Params->infinite || _ParticleCount < _Params->particleCount))
        {
            _LastParticleBeginning += _Params->generateInterval.Random(_Random);
            auto ending = _LastParticleBeginning + _Params->particleLife.Random(_Random);
            auto sprite = std::make_shared<Sprite>(_LastParticleBeginning, ending);
            sprite->SetRenderParams(_Params->render);
            if (_Params->bindParent && parent != nullptr)
            {
                sprite->SetParent(parent);
                sprite->SetInheriteBehavior(_Params->inherite);
            }
            AssignTranslate(sprite.get());
            AssignScaling(sprite.get());
            AssignRotation(sprite.get());
            AssignColor(sprite.get());
            AssignTexture(sprite.get());
            for (auto& child : _Children)
            {
                if (child->_Params->bindParent)
                {
                    sprite->AddComponent(child->ReferenceClone());
                }
            }
            _Particles.emplace_back(std::move(sprite));
            assert(_Particles.size() < 1000);
            _ParticleCount++;
        }
	}
    void ParticleComponent::AssignTranslate(Sprite* sprite)
    {
        Vector2 base = _BasePosition;
        if (_Params->bindParent && (_Params->inherite & InheriteBehavior::PositionOnCreate))
        {
            assert(sprite->GetParent() != nullptr);
            base = base + sprite->GetParent()->GetInfo().translate;
        }
        TranslateAnimationParameter translate;
        ParticleTranslateParameter& PTranslate = _Params->translate;
        translate.type = PTranslate.type;
        switch (PTranslate.type)
        {
        case(TranslateAnimationType::Fixed):
            translate.params.fixed = PTranslate.params.fixed + base;
            break;
        case(TranslateAnimationType::Random):
            translate.type = TranslateAnimationType::Fixed;
            translate.params.fixed = PTranslate.params.random.Random(_Random) + base;
            break;
        case(TranslateAnimationType::FromTo):
            translate.params.from = PTranslate.params.from.Random(_Random) + base;
            translate.params.to = PTranslate.params.to.Random(_Random) + base;
            break;
        case(TranslateAnimationType::PVA):
            translate.params.base = PTranslate.params.base.Random(_Random) + base;
            translate.params.velocity = PTranslate.params.velocity.Random(_Random) + base;
            translate.params.acceleration = PTranslate.params.acceleration.Random(_Random) + base;
            break;
        case(TranslateAnimationType::DirectionPVA):
        {
            translate.type = TranslateAnimationType::PVA;
            translate.params.base = PTranslate.params.base.Random(_Random) + base;
            MinMax<float> direct = PTranslate.params.direction;
            if (direct.max < direct.min)
            {
                direct.max += 360.f;
            }
            float angle = direct.Random(_Random) * PI / 180.f;
            float y = sinf(angle);
            float x = cosf(angle);
            float velocity = PTranslate.params.dir_velocity.Random(_Random);
            float accele = PTranslate.params.dir_acceleration.Random(_Random);
            translate.params.velocity = { velocity * x, velocity * y };
            translate.params.acceleration = { accele * x, accele * y };
        }
        break;
        default:
            assert(false);
            break;
        }
        sprite->SetTranslate(translate);
    }
    void ParticleComponent::AssignScaling(Sprite* spirit)
    {
        ScalingAnimationParameter scal;
        ParticleScalingParameter& Scaling = _Params->scaling;
        scal.type = Scaling.type;
        switch (Scaling.type)
        {
        case (ScalingAnimationType::Fixed):
            scal.params.fixed = Scaling.params.fixed;
            break;
        case(ScalingAnimationType::UniformRandom):
            scal.type = ScalingAnimationType::Fixed;
            scal.params.fixed.x = scal.params.fixed.y = Scaling.params.uniformRandom.Random(_Random);
            break;
        case(ScalingAnimationType::UniformFromTo):
            scal.params.uniformFrom = Scaling.params.uniformFrom.Random(_Random);
            scal.params.uniformTo = Scaling.params.uniformTo.Random(_Random);
            break;
        case(ScalingAnimationType::UniformPVA):
            scal.params.uniformBase = Scaling.params.uniformBase.Random(_Random);
            scal.params.uniformVelocity = Scaling.params.uniformVelocity.Random(_Random);
            scal.params.uniformAcceleration = Scaling.params.uniformAcceleration.Random(_Random);
            break;
        case(ScalingAnimationType::Random):
            scal.type = ScalingAnimationType::Fixed;
            scal.params.fixed = Scaling.params.random.Random(_Random);
            break;
        case(ScalingAnimationType::FromTo):
            scal.params.from = Scaling.params.from.Random(_Random);
            scal.params.to = Scaling.params.to.Random(_Random);
            break;
        case(ScalingAnimationType::PVA):
            scal.params.base = Scaling.params.base.Random(_Random);
            scal.params.velocity = Scaling.params.velocity.Random(_Random);
            scal.params.acceleration = Scaling.params.acceleration.Random(_Random);
            break;
        default:
            assert(false);
            break;
        }
        spirit->SetScaling(scal);
    }
    void ParticleComponent::AssignRotation(Sprite* spirit)
    {
        RotationAnimationParameter rotation;
        ParticleRotationParameter& PRotation = _Params->rotation;
        rotation.type = PRotation.type;
        switch (PRotation.type)
        {
        case(RotationAnimationType::Fixed):
            rotation.params.fixed = PRotation.params.fixed;
            break;
        case(RotationAnimationType::Random):
            rotation.type = RotationAnimationType::Fixed;
            rotation.params.fixed = PRotation.params.random.Random(_Random);
            break;
        case(RotationAnimationType::FromTo):
            rotation.params.from = PRotation.params.from.Random(_Random);
            rotation.params.to = PRotation.params.to.Random(_Random);
            break;
        case(RotationAnimationType::PVA):
            rotation.params.base = PRotation.params.base.Random(_Random);
            rotation.params.velocity = PRotation.params.velocity.Random(_Random);
            rotation.params.acceleration = PRotation.params.acceleration.Random(_Random);
            break;
        default:
            assert(false);
            break;
        }
        spirit->SetRotation(rotation);
    }
    void ParticleComponent::AssignColor(Sprite* spirit)
    {
        ColorAnimationParameter color;
        ParticlColorParameter& PColor = _Params->color;;
        switch (PColor.type)
        {
        case(ColorAnimationType::Fixed):
            color.type = ColorAnimationType::Fixed;
            color.params.fixed = PColor.params.fixed;
            break;
        case(ColorAnimationType::Random):
            color.type = ColorAnimationType::Fixed;
            color.params.fixed = PColor.params.random.Random(_Random);
            break;
        case(ColorAnimationType::FromTo):
            color.type = ColorAnimationType::FromTo;
            color.params.from = PColor.params.from.Random(_Random);
            color.params.to = PColor.params.to.Random(_Random);
            break;
        default:
            assert(false);
            break;
        }
        spirit->SetColor(color);
    }
    void ParticleComponent::AssignTexture(Sprite* spirit)
    {
        TextureAnimationParameter texture = _Params->texture;
        if (texture.type == TextureAnimationType::FrameRandom)
        {
            texture.type = TextureAnimationType::Fixed;
            int frameIndex = _Random.GetRand(0.0f, texture.params.frameCount);
            int x = (frameIndex % texture.params.lineFrameCount) * texture.UVFrame.Width();
            int y = frameIndex / texture.params.lineFrameCount * texture.UVFrame.Height();
            texture.UVFrame.x += x;
            texture.UVFrame.y += y;
            texture.UVFrame.right += x;
            texture.UVFrame.bottom += y;
        }
        spirit->SetTexture(texture);
    }
}
