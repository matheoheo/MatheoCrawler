#include "pch.h"
#include "TickingSpelllEffect.h"

TickingSpelllEffect::TickingSpelllEffect(int duration, int tickInterval)
    :ISpellEffect(duration),
    mTickInterval(tickInterval),
    mTimeUntilNextTick(tickInterval)
{
}

void TickingSpelllEffect::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
}

bool TickingSpelllEffect::tryMerge(const ISpellEffect& other)
{
    return false;
}

void TickingSpelllEffect::onEffectFinish()
{
}

void TickingSpelllEffect::addTimePassed(const sf::Time& deltaTime)
{
    ISpellEffect::addTimePassed(deltaTime);
    mTimeUntilNextTick -= deltaTime.asMilliseconds();
}

void TickingSpelllEffect::resetTickInterval()
{
    mTimeUntilNextTick += mTickInterval;
}

bool TickingSpelllEffect::isNextTickDue() const
{
    return mTimeUntilNextTick <= 0;
}

