#pragma once
#include "ISystem.h"
#include "Pathfinder.h"

class PathRequestSystem :
    public ISystem
{
public:
    PathRequestSystem(SystemContext& systemContext, Pathfinder& pathfinder);

    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    void registerToEvents();
    void registerToPathRequestEvent();
    void updatePathToTarget(Entity& entity);
    void removeFinishedEntities();

    bool hasRecalculationIntervalPassed(const Entity& entity) const;
    void createPathVisual(Entity& entity);
private:
    Pathfinder& mPathfinder;
    std::vector<size_t> mFinishedRequestsIds;
    std::unordered_map<Entity::EntityID, std::vector<sf::RectangleShape>> paths; //for debugging.
};

