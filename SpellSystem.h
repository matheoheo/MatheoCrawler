#pragma once
#include "ISystem.h"
class SpellSystem :
    public ISystem
{
public:
    SpellSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToCastSpellEvent();
    void registerToCastAnimationFinished();
    void notifyAnimationSystem(Entity& entity, int castTime);
    void notifyEffectSystem(const CastSpellEvent& data, const SpellInstance& spell);
    bool canCastSpell(const CastSpellEvent& data, SpellbookComponent& spellbookComp) const;
    void updateLastSpell(SpellbookComponent& spellbookComp, SpellIdentifier id);
    void subtractMana(Entity& entity, SpellbookComponent& spellbookComp);

    void notifyCastFinished(Entity& entity, SpellIdentifier id);
    void addToFinished(Entity* entity);
    void removeFinishedEntities();
private:
    std::vector<Entity::EntityID> mFinishedEntities;
};

