#pragma once
#include "IBehavior.h"

class BasicMeleeBehavior :
    public IBehavior
{
public:
    BasicMeleeBehavior(BehaviorContext& behaviorContext);
    // Inherited via IBehavior
    virtual void update(Entity& entity, const sf::Time& deltaTime) override;
private:
    void determineNextTask(Entity& entity);
    bool canChaseEntity(const Entity& entity, const Entity& target) const;
    bool canCastLOS(const Entity& entity) const;
    bool canAttack(const Entity& entity, const Entity& target) const;

    void swapToPatrol();
    void swapToChase(Entity& target);
    void swapToAttacking(Entity& entity, Entity& target);

    void handleLogicIfPatrolling(Entity& entity, Entity& player);
    void handleLogicIfChasing(Entity& entity, Entity& player);
    void handleLogicIfAttacking(Entity& entity, Entity& player);
    void resetLOSTimer(const Entity& entity) const;
private:
};

