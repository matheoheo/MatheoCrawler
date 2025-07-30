#pragma once
#include "ITask.h"
#include "Entity.h"
//Task responsible for sending an StartChaseEvent.
//ChaseAISystem gets notified and takes care of chase functionality.

class ChaseTask :
    public ITask
{
public:
    ChaseTask(Entity& targetEntity);
    // Inherited via ITask
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
private:
    Entity& mTarget;
};

