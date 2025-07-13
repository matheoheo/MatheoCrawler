#pragma once
#include "ISystem.h"
#include "Pathfinder.h"

class PathRequestSystem :
    public ISystem
{
public:
    PathRequestSystem(SystemContext& systemContext, Pathfinder& pathfinder);

    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToPathRequestEvent();
    void updatePathToTarget(Entity& entity);
    void removeFinishedEntities();

    bool hasRecalculationIntervalPassed(const Entity& entity) const;
private:
    Pathfinder& mPathfinder;
    std::vector<size_t> mFinishedRequestsIds;
};

