#pragma once
#include "ISystem.h"
class TileMap;
class IAOESpell;

class AreaOfEffectSpellSystem :
    public ISystem
{
public:
    AreaOfEffectSpellSystem(SystemContext& systemContext, const TileMap& tileMap);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render(sf::RenderWindow& window) override;
private:
    void registerToEvents();
    void registerToCastAOESpellEvent();
private:
    using AOESpellCreator = std::function<std::unique_ptr<IAOESpell>(const CastAOESpellEvent&)>;
    void createSpellsRegistry();
private:
    const TileMap& mTileMap;
    std::vector<std::unique_ptr<IAOESpell>> mActiveSpells;
    std::unordered_map<SpellIdentifier, AOESpellCreator> mSpellsRegistry;
};

