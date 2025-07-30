#pragma once
#include "ISpellEffect.h"

//This Effect applies movement slow on provided entity for time duration.
class MovementSlowSpellEffect :
    public ISpellEffect
{
public:
    //duration in milliseconds, slowValue is in % (0.4 = 40%)
    MovementSlowSpellEffect(int duration, float slowValue);

    // Inherited via ISpellEffect
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
    virtual bool tryMerge(const ISpellEffect& other) override;
    virtual void onEffectFinish() override;
private:
    void slowEntity(Entity& entity, EventManager& eventManager, float value);
    void revertSlowEffect(Entity& entity, EventManager& eventManager);
    void notifyMoveSpeedChanged(Entity& entity, EventManager& eventManager);
private:
    float mSlowValue;
    bool mSlowApplied;
};

