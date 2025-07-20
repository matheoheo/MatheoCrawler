#include "pch.h"
#include "ISpellEffect.h"
#include "Entity.h"
#include "SpellIdentifiers.h"

ISpellEffect::ISpellEffect(int duration)
    :mDuration(duration),
    mTimePassed(0),
    mSpellEffect(SpellEffect::None)
{
}

SpellEffect ISpellEffect::getSpellEffectId() const
{
    return mSpellEffect;
}

void ISpellEffect::addTimePassed(const sf::Time& deltaTime)
{
    mTimePassed += deltaTime.asMilliseconds();
}

bool ISpellEffect::hasEffectFinished() const
{
    return mTimePassed >= mDuration;
}
