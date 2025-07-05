#pragma once
#include "ISystem.h"
#include "TileMap.h"
class ChaseAISystem :
    public ISystem
{
public:
    ChaseAISystem(SystemContext& systemContext, const TileMap& tileMap);
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();

    //when this event is received, we push entity into system, and he starts chasing.
    void registerToStartChasingEvent();
    //if monsters are chasing, and player moves, we must recalculate path.
    void registerToPlayerMovedEvent();
    void registerToPathAbortedEvent();
    void resetChaseComponent(ChaseAIComponent& chaseComp, Entity* target = nullptr);
    void resetPathComponent(PathComponent& pathComp);
    bool isPathRecalculationDue(const PathComponent& chaseComp) const;
    void askForPathRecalculation(Entity& entity);

    void removeNotChasingEntities();
private:
    const TileMap& mTileMap;
};

