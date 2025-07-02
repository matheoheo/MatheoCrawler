#pragma once
#include "ISpellUpgradeStrategy.h"

class ProjectileSpellUpgradeStrategy :
    public ISpellUpgradeStrategy
{
public:
    ProjectileSpellUpgradeStrategy(SpellIdentifier spellId, float increaseValue);
    // Inherited via ISpellUpgradeStrategy
    virtual void upgrade() const override;
    virtual SpellUpgradeInfo getSpellUpgradeInfo() const override;
};

