#pragma once
#include "ISystem.h"
class TileMap;
class ProjectileSystem :
    public ISystem
{
public:
    ProjectileSystem(SystemContext& systemContext, TileMap& tileMap);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    void registerToEvents();
    void registerToProjectileSpawnedEvent();

    void moveProjectile(const Entity& projectile, const sf::Time& deltaTime);
    bool hasExceededRange(const Entity& projectile);
    bool hasHitSomeone(const Entity& projectile);
    void handleEntityHit(const Entity& projectile);
    void addEntityTargetHit(const Entity& hit, ProjectileComponent& projComp);
    void onHit(Entity& hitEntity, Entity::EntityID projId, ProjectileComponent& projComp, bool wasPlayerHit);
    bool hasHitWall(const Entity& projectile) const;

    //checks if provided entity was already hit by specific projectile.
    bool wasAlreadyHit(const Entity& entity, ProjectileComponent& projComp);
    void markAsFinished(Entity::EntityID id);
    void removeFinished();
private:
    TileMap& mTileMap;
    std::vector<Entity::EntityID> mFinishedProjectiles;
};

