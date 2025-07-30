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

    //returns gold that drops from this entity.
    int getGoldValue(Entity& entity) const;
    void addGoldToPlayer(Entity& deadEntity);
    void notifyUISystem();
private:
    std::vector<Entity*> mDeadEntities;
    std::vector<size_t> mFinishedEntities;
};

