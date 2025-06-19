#pragma once
#include "ISystem.h"

class RegenerationSystem :
    public ISystem
{
public:
    RegenerationSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToRegenerationSpellCasted();
    void addToLastRegenTime(RegenerationComponent& regenComp, const sf::Time& deltaTime);
    bool isRegenerationDue(Entity& player, RegenerationComponent& regenComp) const;
    void regenerate(Entity& player, RegenerationComponent& regenComp);
    void resetRegenerationTimer(RegenerationComponent& regenComp);
    void updateBonusRegeneration(RegenerationComponent& regenComp, const sf::Time& deltaTime);
    void notifyStatusUI();
private:

};

