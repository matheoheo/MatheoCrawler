#pragma once
#include "ITask.h"
#include "AnimationIdentifiers.h"

class AttackTask :
    public ITask
{
public:
    AttackTask(AnimationIdentifier animId);
    // Inherited via ITask
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
private:
    AnimationIdentifier mAnimId;
};

