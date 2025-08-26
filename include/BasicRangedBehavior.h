#pragma once
#include "IBehavior.h"
class BasicRangedBehavior :
    public IBehavior
{
public:
    BasicRangedBehavior(BehaviorContext& behaviorContext);
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
    bool canReachEntity(const Entity& entity, const Entity& target) const;
    bool isTargetTooClose(Entity& self, Entity& target) const;
    bool isTargetTooFar(Entity& self, Entity& target) const;
    bool isTargetCellValid(Entity& self, Entity& target) const;
    void setupBehaviorMap();
};

