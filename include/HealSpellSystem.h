#pragma once
#include "ISystem.h"

class HealSpellSystem :
    public ISystem
{
public:
    HealSpellSystem(SystemContext& systemContext);
    virtual void update(const sf::Time& deltaTime);
private:
    void registerToEvents();
    void regToRemoveEntityFromSystem();
    void registerToTriggerHealSpellEvent();

    void heal(const Entity& entity, float healPercent);
private:
};

