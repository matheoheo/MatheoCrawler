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
private:

};

