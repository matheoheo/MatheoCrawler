#include "pch.h"
#include "EffectSystem.h"
#include "Utilities.h"

EffectSystem::EffectSystem(SystemContext& systemContext)
    :ISystem(systemContext)
{
    registerToEvents();
}

void EffectSystem::update(const sf::Time& deltaTime)
{
    for (GlowEffect& effect : mGlowEffects)
    {
        effect.timer += deltaTime.asMilliseconds();
        if (hasEffectFinished(effect))
        {
            onEffectFinish(effect);
            continue;
        }
        float progress = getEffectProgress(effect);
        updateEffect(effect, progress);
    }
    removeFinishedEffects();
}

void EffectSystem::registerToEvents()
{
    registerToStartGlowEffect();
}

void EffectSystem::registerToStartGlowEffect()
{
    mSystemContext.eventManager.registerEvent<StartGlowUpEffect>([this](const StartGlowUpEffect& data)
        {
            mGlowEffects.emplace_back(data.entity, data.targetColor, data.duration);
        });
}

void EffectSystem::removeEffect(Entity::EntityID id)
{
    std::erase_if(mGlowEffects, [id](const GlowEffect& effect)
        {
            return effect.entId == id;
        });
}

void EffectSystem::removeFinishedEffects()
{
    for (const auto& finished : mFinishedGlows)
    {
        removeEffect(finished);
    }
    mFinishedGlows.clear();
}

bool EffectSystem::hasEffectFinished(const GlowEffect& effect) const
{
    return effect.timer >= effect.effectDuration;
}

void EffectSystem::onEffectFinish(GlowEffect& effect)
{
    effect.sprite->setColor(effect.onStartColor);
}

float EffectSystem::getEffectProgress(const GlowEffect& effect) const
{
    return static_cast<float>(effect.timer) / static_cast<float>(effect.effectDuration);
}

void EffectSystem::updateEffect(GlowEffect& effect, float progress)
{
    float wave = std::sin(progress * 3.1415f);
    auto newColor = Utilities::lerpColor(effect.onStartColor, effect.targetColor, wave);
    effect.sprite->setColor(newColor);

}
