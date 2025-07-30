#include "pch.h"
#include "MovementFrozenSpellEffect.h"
#include "Entity.h"
#include "EventManager.h"

MovementFrozenSpellEffect::MovementFrozenSpellEffect(int duration)
    :ISpellEffect(duration),
    mEffectApplied(false)
{
    mSpellEffect = SpellEffect::MovementFrozen;
}

void MovementFrozenSpellEffect::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
    addTimePassed(deltaTime);
    if (!mEffectApplied)
    {
        applyEffect(entity);
        mEffectApplied = true;
    }

    if (hasEffectFinished())
    {
        revertEffect(entity);
        complete();
    }
}

bool MovementFrozenSpellEffect::tryMerge(const ISpellEffect& other)
{
    if(other.getSpellEffectId() != getSpellEffectId())
      return false;

    const MovementFrozenSpellEffect& effect = static_cast<const MovementFrozenSpellEffect&>(other);
    mTimePassed = 0;
    mDuration = effect.mDuration;
    return true;
}

void MovementFrozenSpellEffect::onEffectFinish()
{
    
}

void MovementFrozenSpellEffect::applyEffect(Entity& entity)
{
    auto& movementComp = entity.getComponent<MovementComponent>();
    movementComp.cMovementBlocked = true;
}

void MovementFrozenSpellEffect::revertEffect(Entity& entity)
{
    auto& movementComp = entity.getComponent<MovementComponent>();
    movementComp.cMovementBlocked = false;
}
