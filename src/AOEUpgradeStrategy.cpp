#include "pch.h"
#include "AOEUpgradeStrategy.h"
#include "SpellHolder.h"

AOEUpgradeStrategy::AOEUpgradeStrategy(SpellIdentifier spellId, float increaseValue)
    :ISpellUpgradeStrategy(spellId, increaseValue)
{
}

void AOEUpgradeStrategy::upgrade() const
{
    auto& minDmg = SpellHolder::getInstance().get(mSpellId).aoe->minDmg;
    auto& maxDmg = SpellHolder::getInstance().get(mSpellId).aoe->maxDmg;
    int increaseInt = static_cast<int>(mIncreaseValue);
    minDmg += increaseInt;
    maxDmg += increaseInt;
}

SpellUpgradeInfo AOEUpgradeStrategy::getSpellUpgradeInfo() const
{
    SpellUpgradeInfo upgradeInfo;
    auto& spell = SpellHolder::getInstance().get(mSpellId).aoe;

    int increaseInt = static_cast<int>(mIncreaseValue);
    upgradeInfo.currValue = std::format("[{},{}]", spell->minDmg, spell->maxDmg);
    upgradeInfo.nextValue = std::format("[{},{}]", spell->minDmg + increaseInt, spell->maxDmg + increaseInt);
    upgradeInfo.name = SpellHolder::getInstance().getDefinition(mSpellId).spellInfo.name;
    upgradeInfo.unit = "dmg";

    return upgradeInfo;
}
