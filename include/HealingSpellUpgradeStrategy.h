#pragma once
#include "ISpellUpgradeStrategy.h"

class HealingSpellUpgradeStrategy :
    public ISpellUpgradeStrategy
{
public:
    HealingSpellUpgradeStrategy(SpellIdentifier id, float increaseValue);
    // Inherited via ISpellUpgradeStrategy
    virtual void upgrade() const override;
    virtual SpellUpgradeInfo getSpellUpgradeInfo() const override;
};

