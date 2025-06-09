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
private:
};

