#pragma once
#include "ISystem.h"

class TileMap;
class PathFollowSystem :
    public ISystem
{
public:
    PathFollowSystem(SystemContext& systemContext, TileMap& tileMap);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToAddToPathFollowSystemEvent();
    std::optional<sf::Vector2i> getPathStep(const PathComponent& pathComp) const;
    std::optional<Direction> getStepDirection(const Entity& ent, const sf::Vector2i& stepCell) const;
    void doStep(Entity& entity, PathComponent& pathComp);

    bool isPathEmpty(const PathComponent& pathComp) const;
    void abortPath(Entity& ent);
    void markAsFinished(Entity* ent);
    void removeFinishedEntites();
private:
    TileMap& mTileMap;
    std::unordered_set<Entity*> mFinishedEntities;
};

