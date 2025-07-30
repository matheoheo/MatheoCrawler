#pragma once
#include "ITask.h"

class WaitUntilIdleTask :
    public ITask
{
public:
    // Inherited via ITask
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
};

