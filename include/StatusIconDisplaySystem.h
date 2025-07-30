#pragma once
#include "ISystem.h"
#include "AssetManager.h"

class StatusIconDisplaySystem :
    public ISystem
{
public:
    StatusIconDisplaySystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    void registerToEvents();
    void registerToAddSpellEffectEvent();
    void removeFinishedEntities();
    void removeFinishedIcons(Entity& entity);
    void positionIcons(Entity& entity);

    bool entityHasIconAlready(Entity& entity, SpellEffect effectId);
    void addIconToEntity(Entity& entity, SpellEffect effectId);
    std::optional<TextureIdentifier> getTextureIdForEffect(SpellEffect effectId);
private:
    AssetManager<TextureIdentifier, sf::Texture>& mTextures;
    const sf::Vector2f mIconSize;
};

