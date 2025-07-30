#include "pch.h"
#include "FireBurnSpellEffect.h"
#include "SpellIdentifiers.h"
#include "EventManager.h"

FireBurnSpellEffect::FireBurnSpellEffect(int duration, int damagePerTick)
    :TickingSpelllEffect(duration, burnTickInterval),
    mDamagePerTick(damagePerTick)
{
    mSpellEffect = SpellEffect::FireBurn;
}

void FireBurnSpellEffect::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
    if (hasEffectFinished())
    {
        onEffectFinish();
        complete();
        return;
    }

    TickingSpelllEffect::addTimePassed(deltaTime);
    if (isNextTickDue())
    {
        takeTickDamage(entity, eventManager);
        resetTickInterval();
    }
   
}

bool FireBurnSpellEffect::tryMerge(const ISpellEffect& other)
{
    if(other.getSpellEffectId() != getSpellEffectId())
        return false;

    //in case of merging fireBurn, we want to set back to longest duration and highest damage.
    const FireBurnSpellEffect& effect = static_cast<const FireBurnSpellEffect&>(other);
    mDamagePerTick = std::max(mDamagePerTick, effect.mDamagePerTick);
    mDuration = std::max(mDuration, effect.mDuration);
    return true;
}

void FireBurnSpellEffect::onEffectFinish()
{
}

void FireBurnSpellEffect::takeTickDamage(Entity& entity, EventManager& eventManager)
{
    eventManager.notify<HitByTickDamageEvent>(HitByTickDamageEvent(entity, mDamagePerTick));
}
