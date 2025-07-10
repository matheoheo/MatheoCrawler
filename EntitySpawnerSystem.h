#pragma once
#include "ISystem.h"
#include "AssetManager.h"
#include "TextureIdentifiers.h"
#include "EntityFactory.h"
#include "EntityTypes.h"
#include "IMapGenerator.h"

class EntitySpawnerSystem :
    public ISystem
{
public:
    EntitySpawnerSystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures,
        BehaviorContext& behaviorContext);
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToSpawnEntityEvent();
    void registerToSpawnProjectileEvent();
    void registerToGenerateEntitiesEvent();
private:
    struct WeightedSpawn {
        EntityType type;
        int weight;
    };
    EntityType getRandomWeightedSpawn(std::span<const WeightedSpawn> spawns);
    void spawnEntities(const IMapGenerator::SpawnPoints& spawnPoints);
private:
    AssetManager<TextureIdentifier, sf::Texture>& mTextures;
    BehaviorContext& mBehaviorContext;
    EntityFactory mEntityFactory;
};

