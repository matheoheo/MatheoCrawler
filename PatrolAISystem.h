#pragma once
#include "ISystem.h"
#include "TileMap.h"

class PatrolAISystem :
    public ISystem
{
public:
    PatrolAISystem(SystemContext& systemContext, const TileMap& tileMap);
    virtual void update(const sf::Time& deltaTime) override;
private:
    bool isTimeForNextMove(const PatrolAIComponent& patrolComp) const;
    void addToPatrolTimer(PatrolAIComponent& patrolComp, float elapsed) const;
    void resetPatrolTimer(PatrolAIComponent& patrolComp) const;
    void doPatrol(Entity& entity);
    std::optional<Direction> getRandomValidDirection(const Entity& entity) const;

    void registerToEvents();
    void removeNonPatrollingEntities();
private:
    const TileMap& mTileMap;
};

