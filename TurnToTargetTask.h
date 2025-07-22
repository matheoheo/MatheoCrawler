#pragma once
#include "ITask.h"
class Entity;
class IBehavior;

class TurnToTargetTask :
    public ITask
{
public:
    TurnToTargetTask(Entity& target, IBehavior* behavior);
    // Inherited via ITask
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
private:

private:
    Entity& mTarget;
    //we keep pointer to our behavior, because it has a fallback function on failure.
    //we just gonna call function 'setDirectionTowardsTarget' from iBehavior
    IBehavior* mBehavior; 
};

