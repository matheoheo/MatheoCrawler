#pragma once
#include "ISystem.h"

class OnHitSystem :
    public ISystem
{
public:
    OnHitSystem(SystemContext& systemContext);
    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void registerToHitByAttackEvent();
    int calculateDamage(const Entity& attacker, const Entity& target) const;

    //returns taken damage(if target goes under 0 hp then the damage is modified to reflect this situation)
    //also returns true if entities hp went to 0, false otherwise
    std::pair<int, bool> takeDamage(const Entity& target, int damage);
    void notifyEntityDied(Entity& entity);
    void notifyPlayerAttacked(int dmg);
    void notifyPlayerHit(int dmg);
    void notifyHealthBarSystem(Entity& entity);
private:

};

