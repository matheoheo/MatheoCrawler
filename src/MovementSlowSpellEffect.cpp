#include "pch.h"
#include "MovementSlowSpellEffect.h"
#include "Entity.h"
#include "EventManager.h"

MovementSlowSpellEffect::MovementSlowSpellEffect(int duration, float slowValue)
    :ISpellEffect(duration),
    mSlowValue(slowValue),
    mSlowApplied(false)
{
    mSpellEffect = SpellEffect::MovementSpeedSlow;
    
    mSlowValue = std::clamp(mSlowValue, 0.f, 1.0f);
}

void MovementSlowSpellEffect::update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime)
{
    if (hasEffectFinished())
    {
        revertSlowEffect(entity, eventManager);
        complete();
        return;
    }
    addTimePassed(deltaTime);
    if (!mSlowApplied)
    {
        slowEntity(entity, eventManager, mSlowValue);
        mSlowApplied = true;
    }
}

bool MovementSlowSpellEffect::tryMerge(const ISpellEffect& other)
{
    if (getSpellEffectId() != other.getSpellEffectId())
        return false;

    //in case of merging we just refresh duration of current effect.
    const MovementSlowSpellEffect& effect = static_cast<const MovementSlowSpellEffect&>(other);
    mDuration = effect.mDuration;
    return true;
}

void MovementSlowSpellEffect::onEffectFinish()
{

}

void MovementSlowSpellEffect::slowEntity(Entity& entity, EventManager& eventManager, float value)
{
    constexpr float moveSpeedLimit = 20.f; //cannot slow lower than this amount
    constexpr float fullSpeedMultiplier = 1.0f;
    auto& moveComp = entity.getComponent<MovementComponent>();
    float currSpeed = moveComp.cMoveSpeed;
    currSpeed *= (fullSpeedMultiplier - value);

    if (currSpeed < moveSpeedLimit)
        moveComp.cMoveSpeed = moveSpeedLimit;
    else
        moveComp.cMoveSpeed = std::ceilf(currSpeed); //remove fractions preferably.
    notifyMoveSpeedChanged(entity, eventManager);
}

void MovementSlowSpellEffect::revertSlowEffect(Entity& entity, EventManager& eventManager)
{
    auto& moveComp = entity.getComponent<MovementComponent>();
    moveComp.cMoveSpeed = moveComp.cBaseMoveSpeed;
    notifyMoveSpeedChanged(entity, eventManager);
}

void MovementSlowSpellEffect::notifyMoveSpeedChanged(Entity& entity, EventManager& eventManager)
{
    eventManager.notify<MoveSpeedChangedEvent>(MoveSpeedChangedEvent(entity));
}
