#pragma once
#include "TickingSpelllEffect.h"

class FireBurnSpellEffect :
    public TickingSpelllEffect
{
public:
    FireBurnSpellEffect(int duration, int damagePerTick);
    // Inherited via ISpellEffect
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
    virtual bool tryMerge(const ISpellEffect& other) override;
protected:
    virtual void onEffectFinish() override;
    void takeTickDamage(Entity& entity, EventManager& eventManager);
private:
    int mDamagePerTick;
    static constexpr int burnTickInterval = 1000; //milliseconds
};

