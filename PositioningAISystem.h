#pragma once
#include "ISystem.h"
#include "TileMap.h"
//This system is gonna be responsible for (as name suggests) positioning entities in the game.
//It is created to handle AI behaviors like: fleeing, strafing, keeping distance
//Will be used mostly for ranged entities.
//Melee entities are using ChaseAISystem - but the word 'chasing' doesn't fit with ranged behaviors,
//that's why this system is being created.

class PositioningAISystem :
    public ISystem
{
public:
    PositioningAISystem(SystemContext& systemContext, TileMap& tileMap);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToRepositionToAttackEvent();

    bool isInOptimalPositionToAttack(const Entity& self, const Entity& target) const;
    std::optional<sf::Vector2i> getOptimalAttackPosition(Entity& entity, const Entity& target) const;
private:
    TileMap& mTileMap;
};

