#pragma once
#include "ITask.h"
class PositionToAttackTask :
    public ITask
{
public:
    PositionToAttackTask(Entity& target);
    // Inherited via ITask
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
private:
    Entity& mTarget;
};

