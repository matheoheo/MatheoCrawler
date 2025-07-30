#pragma once
#include "ISpellUpgradeStrategy.h"
class RegenSpellUpgradeStrategy :
    public ISpellUpgradeStrategy
{
public:
    RegenSpellUpgradeStrategy(SpellIdentifier spellId, float increaseValue);
    // Inherited via ISpellUpgradeStrategy
    virtual void upgrade() const override;
    virtual SpellUpgradeInfo getSpellUpgradeInfo() const override;
};

