#pragma once
#include "ITask.h"

class DelayTask :
    public ITask
{
public:
    //interval should be in milliseconds
    DelayTask(int interval);

    // Inherited via ITask
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
private:
    int mDelayInterval;


};

