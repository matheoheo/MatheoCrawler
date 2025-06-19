#include "pch.h"
#include "EffectSystem.h"
#include "Utilities.h"
#include "Config.h"

EffectSystem::EffectSystem(SystemContext& systemContext)
    :ISystem(systemContext),
    mRegenEffectInterval(50)
{
    registerToEvents();
}

void EffectSystem::update(const sf::Time& deltaTime)
{
    updateGlowEffects(deltaTime);
    updateRegenerationEffects(deltaTime);
    removeFinishedEffects();
}

void EffectSystem::render(sf::RenderWindow& window)
{
    renderRegenerationEffects(window);
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
            mGlowEffects.emplace_back(data.entity, data.targetColor, data.duration);
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
            mRegenerationEffects.emplace_back(data.caster, data.color, data.duration);
        });
}

void EffectSystem::removeEffects(Entity::EntityID id)
{
    removeGlowEffect(id);
    removeRegenerationEffect(id);
}

void EffectSystem::removeRegenerationEffect(Entity::EntityID id)
{
    std::erase_if(mRegenerationEffects, [id](const RegenerationEffect& effect)
        {
            return effect.entity->getEntityId() == id;
        });
}

void EffectSystem::removeGlowEffect(Entity::EntityID id)
{
    std::erase_if(mGlowEffects, [id](const GlowEffect& effect)
        {
            return effect.entId == id;
        });
}

void EffectSystem::removeFinishedEffects()
{
    for (const auto& finished : mFinishedGlows)
        removeGlowEffect(finished);

    for (const auto& finished : mFinishedRegenerations)
        removeRegenerationEffect(finished);

    mFinishedGlows.clear();
    mFinishedRegenerations.clear();
}

void EffectSystem::updateGlowEffects(const sf::Time& deltaTime)
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

bool EffectSystem::hasEffectFinished(const RegenerationEffect& effect) const
{
    return effect.timer >= effect.effectDuration;
}

bool EffectSystem::updateRegenerationEffects(const sf::Time& deltaTime)
{
    for (auto& effect : mRegenerationEffects)
    {
        ++effect.spawnTimer;
        effect.timer += deltaTime.asMilliseconds();
        if (hasEffectFinished(effect))
        {
            mFinishedRegenerations.emplace_back(effect.entity->getEntityId());
            continue;
        }
        if (isRegenVisualSpawnDue(effect))
            spawnRegenerationVisual(effect);
        updateRegenVisuals(effect, deltaTime);
    }
    return false;
}

bool EffectSystem::isRegenVisualSpawnDue(const RegenerationEffect& effect) const
{
    return effect.spawnTimer % mRegenEffectInterval == 0;
}

void EffectSystem::spawnRegenerationVisual(RegenerationEffect& effect)
{
    auto pos = getSpawnPos(effect);
    effect.visuals.emplace_back(pos, effect.effectColor);
}

void EffectSystem::updateRegenVisuals(RegenerationEffect& effect, const sf::Time& deltaTime)
{
    auto& visuals = effect.visuals;
    for (auto it = std::begin(visuals); it != std::end(visuals);)
    {
        it->update(deltaTime);
        if (it->lifetimePassed())
            it = visuals.erase(it);
        else
            ++it;
    }
}

void EffectSystem::renderRegenerationEffects(sf::RenderWindow& window)
{
    for (auto& effect : mRegenerationEffects)
    {
        for (auto& visual : effect.visuals)
            visual.render(window);
    }
}

sf::Vector2f EffectSystem::getSpawnPos(const RegenerationEffect& effect) const
{
    auto entCell = Utilities::getEntityCell(*effect.entity);
    sf::Vector2f cellPos{ entCell.x * Config::getCellSize().x, entCell.y * Config::getCellSize().y };

    int minXPos = static_cast<int>(cellPos.x) + static_cast<int>(Config::getCellSize().x * 0.25f);
    int maxXPos = minXPos + static_cast<int>(Config::getCellSize().x * 0.75f);

    int minYPos = static_cast<int>(cellPos.y);
    int maxYPos = minYPos + static_cast<int>(Config::getCellSize().y * 0.5f);

    int x = Random::get(minXPos, maxXPos);
    int y = Random::get(minYPos, maxYPos);
    return { static_cast<float>(x), static_cast<float>(y) };
}
