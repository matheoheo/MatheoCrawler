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
    void resetChaseComponent(ChaseAIComponent& chaseComp, Entity* target = nullptr);
    void resetPathComponent(PathComponent& pathComp);
    std::optional<sf::Vector2i> getPathStep(PathComponent& pathComp) const;
    bool isStepWalkable(const Entity& entity, const sf::Vector2i& stepCell) const;
    void doStep(Entity& entity, PathComponent& pathComp, const sf::Vector2i& stepCell);
    std::optional<Direction> getStepDirection(const sf::Vector2i& fromCell, const sf::Vector2i& toCell) const;
    bool isPathRecalculationDue(const ChaseAIComponent& chaseComp) const;
    void askForPathRecalculation(Entity& entity);

    void removeNotChasingEntities();
private:
    const TileMap& mTileMap;
    const float mPathRecalculationCooldown;
};

