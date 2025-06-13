#pragma once
#include "ISystem.h"
#include "AssetManager.h"
#include "TextureIdentifiers.h"

class EntitySpawnerSystem :
    public ISystem
{
public:
    EntitySpawnerSystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures,
        BehaviorContext& behaviorContext);
    virtual void update(const sf::Time& deltaTime) override;
private:
    using AttackDataMap = std::unordered_map<AnimationIdentifier, AttackData>;
    struct BaseEntityData {
        std::string tag;
        float moveSpeed = 130.f;
        int fovRange = 1;
        CombatStatsComponent combatStats;
        AttackDataMap cAttackDataMap;
    };

    void registerToEvents();
    void registerToSpawnEntityEvent();
    void createEntityData();
    void createPlayerEntityData();
    void createSkletorusEntityData();

    void spawnEntity(const sf::Vector2i& cellIndex, EntityType entType);
    void addCommonComponents(Entity& entity, EntityType entType);
    void addSpriteComponent(Entity& entity, TextureIdentifier textureId, const sf::Vector2f& pos);
    void addAIComponents(Entity& entity);

    void spawnPlayerEntity(const sf::Vector2i& cellIndex);
    void spawnSkletorusEntity(const sf::Vector2i& cellIndex);

    void notifyTileOccupied(Entity& entity);
    sf::Vector2f cellIndexToPos(const sf::Vector2i& cellIndex) const;
    AttackDataMap getBasicMeleeAttackDataMap() const;
    AttackDataMap getPlayerAttackDataMap() const;

    int getRandomPatrolDelay(int min, int max) const;
private:
    AssetManager<TextureIdentifier, sf::Texture>& mTextures;
    BehaviorContext& mBehaviorContext;
    std::unordered_map<EntityType, BaseEntityData> mEntityData;
};

