#pragma once
#include "ISystem.h"
class TileMap;

class SpellSystem :
    public ISystem
{
public:
    SpellSystem(SystemContext& systemContext, const TileMap& tileMap, const sf::Vector2f& mousePos);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToCastSpellEvent();
    void registerToCastAnimationFinished();
    void notifyUISystem(sf::Keyboard::Key key, int& cooldown);
    void notifyAnimationSystem(Entity& entity, int castTime, AnimationIdentifier animId);
    void notifyEffectSystem(const CastSpellEvent& data, const SpellInstance& spell);
    bool canCastSpell(const CastSpellEvent& data, SpellbookComponent& spellbookComp) const;
    void updateLastSpell(SpellbookComponent& spellbookComp, SpellIdentifier id);
    void subtractMana(Entity& entity, SpellbookComponent& spellbookComp);
    bool canCastAtMousePos(const Entity& entity) const;

    void notifyCastFinished(Entity& entity, SpellIdentifier id);
    AnimationIdentifier getAnimationBasedOnSpellType(const SpellData& data) const;

    void onRegenSpellCast(Entity& entity, SpellIdentifier id);

    bool isHealingSpell(SpellIdentifier id) const;
    bool isRegenSpell(SpellIdentifier id) const;
    bool isProjectileSpell(SpellIdentifier id) const;
    bool isAOESpell(SpellIdentifier id) const;
    bool doesSpellDependOnMousePos(SpellIdentifier id) const;

    void addToFinished(Entity* entity);
    void removeFinishedEntities();
private:
    const TileMap& mTileMap;
    const sf::Vector2f& fMousePos;
    std::vector<Entity::EntityID> mFinishedEntities;
};

