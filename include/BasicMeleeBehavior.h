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
    
    virtual void handleTargettingLogic(Entity& self, Entity& target) override;
    virtual void handleAttackingLogic(Entity& self, Entity& target) override;
private:
    void setupBehaviorsMap();
};

