#pragma once
#include "ISpellEffect.h"
class TickingSpelllEffect :
    public ISpellEffect
{
public:
    TickingSpelllEffect(int duration, int tickInterval);
    // Inherited via ISpellEffect
    virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) override;
    virtual bool tryMerge(const ISpellEffect& other) override;
    virtual void onEffectFinish() override;
protected:
    virtual void addTimePassed(const sf::Time& deltaTime);
    void resetTickInterval();
    bool isNextTickDue() const;
protected:
    int mTickInterval;
    int mTimeUntilNextTick;

};

