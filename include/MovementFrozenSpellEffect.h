#pragma once
#include "ISpellEffect.h"
class MovementFrozenSpellEffect :
    public ISpellEffect
{
public:
    MovementFrozenSpellEffect(int duration);

    // Inherited via ISpellEffect
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
    virtual bool tryMerge(const ISpellEffect& other) override;
    virtual void onEffectFinish() override;
private:
    void applyEffect(Entity& entity);
    void revertEffect(Entity& entity);
private:
    bool mEffectApplied;
};

