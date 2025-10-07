#include "pch.h"
#include "EffectSystem.h"
#include "Utilities.h"
#include "Config.h"
#include "GlowUpVisualEffect.h"
#include "RegenerationVisualEffect.h"

EffectSystem::EffectSystem(SystemContext& systemContext)
    :ISystem(systemContext)
{
    registerToEvents();
}

void EffectSystem::update(const sf::Time& deltaTime)
{
    for (const auto& effect : mEffects)
        effect->update(deltaTime);

    std::erase_if(mEffects, [](const auto& eff)
        {
            return eff->isFinished();
        });
}

void EffectSystem::render(sf::RenderWindow& window)
{
    for (const auto& effect : mEffects)
        effect->render(window);
}

void EffectSystem::registerToEvents()
{
    registerToStartGlowEffect();
    regToEntityDiedEvent();
    registerToSpawnRegenerationEffect();
}

void EffectSystem::registerToStartGlowEffect()
{
    mSystemContext.eventManager.registerEvent<StartGlowUpEffect>([this](const StartGlowUpEffect& data)
        {
            mEffects.emplace_back(std::make_unique<GlowUpVisualEffect>(data.entity, data.duration, data.targetColor));
        });
}

void EffectSystem::regToEntityDiedEvent()
{
    mSystemContext.eventManager.registerEvent<RemoveEntityFromSystemEvent>([this](const RemoveEntityFromSystemEvent& data)
        {
            removeEffects(data.entity.getEntityId());
        });
}

void EffectSystem::registerToSpawnRegenerationEffect()
{
    mSystemContext.eventManager.registerEvent<StartRegenEffect>([this](const StartRegenEffect& data)
        {
            mEffects.emplace_back(std::make_unique<RegenerationVisualEffect>(data.caster, data.duration, data.color));
        });
}

void EffectSystem::removeEffects(Entity::EntityID id)
{
    std::erase_if(mEffects, [id](const auto& effect) {
        return effect->getEntityId() == id;
    });
}
