#pragma once
#include "ISpellUpgradeStrategy.h"
class AOEUpgradeStrategy :
    public ISpellUpgradeStrategy
{
public:
    AOEUpgradeStrategy(SpellIdentifier spellId, float increaseValue);

    // Inherited via ISpellUpgradeStrategy
    virtual void upgrade() const override;
    virtual SpellUpgradeInfo getSpellUpgradeInfo() const override;
private:

private:

};

