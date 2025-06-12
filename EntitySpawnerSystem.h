#pragma once
#include "ISystem.h"
#include "AssetManager.h"
#include "TextureIdentifiers.h"

class EntitySpawnerSystem :
    public ISystem
{
public:
    EntitySpawnerSystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures);
    virtual void update(const sf::Time& deltaTime) override;
private:
    struct BaseEntityData {
        std::string tag;
        float moveSpeed = 130.f;
        CombatStatsComponent combatStats;
        std::unordered_map<AnimationIdentifier, AttackData> cAttackDataMap;
    };

    void registerToEvents();
    void registerToSpawnEntityEvent();
    void createEntityData();
    void createPlayerEntityData();
    void createSkletorusEntityData();

    void spawnEntity(const sf::Vector2i& cellIndex, EntityType entType);
    void addCommonComponents(Entity& entity, EntityType entType);
    void addSpriteComponent(Entity& entity, TextureIdentifier textureId, const sf::Vector2f& pos);
    void spawnPlayerEntity(const sf::Vector2i& cellIndex);

    sf::Vector2f cellIndexToPos(const sf::Vector2i& cellIndex) const;
private:
    AssetManager<TextureIdentifier, sf::Texture>& mTextures;
    std::unordered_map<EntityType, BaseEntityData> mEntityData;
};

