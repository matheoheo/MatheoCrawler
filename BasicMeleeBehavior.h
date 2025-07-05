#pragma once
#include "IBehavior.h"
#include "AnimationIdentifiers.h"

class BasicMeleeBehavior :
    public IBehavior
{
public:
    BasicMeleeBehavior(BehaviorContext& behaviorContext);
    // Inherited via IBehavior
    virtual void update(Entity& entity, const sf::Time& deltaTime) override;
protected:
    virtual void determineNextTask(Entity& entity) override;
private:
    bool canChaseEntity(const Entity& entity, const Entity& target) const;
    bool canCastLOS(const Entity& entity) const;

    void swapToChase(Entity& target);
    void swapToAttacking(Entity& entity, Entity& target);

    void handleLogicIfPatrolling(Entity& entity, Entity& player);
    void handleLogicIfChasing(Entity& entity, Entity& player);
    void handleLogicIfAttacking(Entity& entity, Entity& player);
    void resetLOSTimer(const Entity& entity) const;

    AnimationIdentifier getRandomAttack(const Entity& entity);
private:
};

