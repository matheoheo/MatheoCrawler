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
    virtual void swapToTargetting(Entity& entity) override;
    virtual void swapToAttacking(Entity& entity, Entity& target) override;
    virtual void fallbackOnNoDirection(Entity& self, Entity& target) override;
private:
    void handleLogicIfChasing(Entity& entity, Entity& player);
    void handleLogicIfAttacking(Entity& entity, Entity& player);
private:
};

