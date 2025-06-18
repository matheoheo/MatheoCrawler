#pragma once
#include "ISystem.h"
#include "AssetManager.h"
#include "TextureIdentifiers.h"
#include "EntityFactory.h"
class SpellHolder;

class EntitySpawnerSystem :
    public ISystem
{
public:
    EntitySpawnerSystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures,
        BehaviorContext& behaviorContext, SpellHolder& spellHolder);
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToSpawnEntityEvent();
private:
    AssetManager<TextureIdentifier, sf::Texture>& mTextures;
    BehaviorContext& mBehaviorContext;
    EntityFactory mEntityFactory;
};

