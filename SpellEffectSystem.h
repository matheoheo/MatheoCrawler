#pragma once
#include "ISystem.h"
#include "SpellIdentifiers.h"

class ISpellEffect;
class SpellEffectSystem :
    public ISystem
{
public:
    SpellEffectSystem(SystemContext& systemContext);
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToAddSpellEffectEvent();
    void removeFinishedEffects(SpellEffectsComponent& spellEffectsComp);
private:
    using SpellEffectCreator = std::function<std::unique_ptr<ISpellEffect>()>;
    void createEffectRegistry();
private:
    std::unordered_map<SpellEffect, SpellEffectCreator> mEffectRegistry;
};

