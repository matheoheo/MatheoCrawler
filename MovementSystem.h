#pragma once
#include "ISystem.h"

class MovementSystem :
    public ISystem
{
public:
    MovementSystem(SystemContext& systemContext);
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void moveEntity(const Entity& entity, const sf::Time& deltaTime);
    bool isNearTargetPosition(const Entity& entity) const;
    void finishMovement(Entity& entity);
    void eraseFinishedEntities();

    void registerToMoveAllowedEvent();
private:
    const float mThreshold;
};

