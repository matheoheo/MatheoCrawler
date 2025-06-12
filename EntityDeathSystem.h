#pragma once
#include "ISystem.h"
class EntityDeathSystem :
    public ISystem
{
public:
    EntityDeathSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void markEntityAsDead(Entity& entity);
    void removeFinishedEntities();
private:
    std::vector<Entity*> mDeadEntities;
    std::vector<size_t> mFinishedEntities;
};

