#pragma once
#include "ISystem.h"
class TileMap;

class CollisionSystem :
    public ISystem
{
public:
    CollisionSystem(SystemContext& systemContext, const TileMap& tileMap);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    bool hasEntityMovementdBlocked(const Entity& entity) const;
    sf::Vector2f getNextTilePosition(const Entity& entity) const;
private:
    const TileMap& mTileMap;
};

