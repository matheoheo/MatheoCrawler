#pragma once
#include "ISystem.h"
class PlayerRegenerationSystem :
    public ISystem
{
public:
    PlayerRegenerationSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void addToLastRegenTime(RegenerationComponent& regenComp, const sf::Time& deltaTime);
    bool isRegenerationDue(Entity& player, RegenerationComponent& regenComp) const;
    void regenerate(Entity& player, RegenerationComponent& regenComp);
    void notifyStatusUI();
private:

};

