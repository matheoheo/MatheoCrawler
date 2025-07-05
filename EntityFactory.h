#pragma once
#include "EntityManager.h"
#include "AssetManager.h"
#include "TextureIdentifiers.h"
#include "EntityDataRepository.h"

class EventManager;
struct SpawnProjectileEvent;
class EntityFactory
{
public:
	EntityFactory(EntityManager& entityManager, AssetManager<TextureIdentifier, sf::Texture>& textures,
        BehaviorContext& behaviorContext, EventManager& eventManager);

    void spawnEntity(const sf::Vector2i& cellIndex, EntityType entType);
    void spawnProjectileEvent(const SpawnProjectileEvent& data);
private:
    void addCommonComponents(Entity& entity, EntityType entType);
    void addSpriteComponent(Entity& entity, TextureIdentifier textureId, const sf::Vector2f& pos);
    void addAIComponents(Entity& entity);
    void addPositioningComponent(Entity& entity, int minRange);

    void spawnPlayerEntity(const sf::Vector2i& cellIndex);
    void spawnSkletorusEntity(const sf::Vector2i& cellIndex);
    void spawnBonvikEntity(const sf::Vector2i& cellIndex);
    void spawnMorannaEntity(const sf::Vector2i& cellIndex);

    sf::Vector2f cellIndexToPos(const sf::Vector2i& cellIndex) const;
    int getRandomPatrolDelay(int min, int max) const;
    void notifyTileOccupied(Entity& entity);

    //returns combat stats modified by difficulity level.
    CombatStatsComponent getAdjustedCombatStats(EntityType entType) const;

    float getStatMultiplier() const;
private:
    //spawns projectile on provided cell
    void spawnProjectile(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir);
    void spawnWaterBall(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir);
    void spawnPureProjectiles(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir);
    void spawnFireballProjectiles(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex);
    void spawnBloodballProjectiles(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir);

    float getProjectileRotation(Direction dir) const;
    const sf::Texture& getProjectileTexture(SpellIdentifier id, const SpellData& data);
private:
	EntityManager& mEntityManager;
	AssetManager<TextureIdentifier, sf::Texture>& mTextures;
    BehaviorContext& mBehaviorContext;
    EventManager& mEventManager;
	EntityDataRepository mEntityData;
};

