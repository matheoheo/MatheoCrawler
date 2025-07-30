#include "pch.h"
#include "ProjectileSpellUpgradeStrategy.h"
#include "SpellHolder.h"

ProjectileSpellUpgradeStrategy::ProjectileSpellUpgradeStrategy(SpellIdentifier spellId, float increaseValue)
    :ISpellUpgradeStrategy(spellId, increaseValue)
{
}

void ProjectileSpellUpgradeStrategy::upgrade() const
{
    auto& minDmg = SpellHolder::getInstance().get(mSpellId).projectile.value().minDmg;
    auto& maxDmg = SpellHolder::getInstance().get(mSpellId).projectile.value().maxDmg;

    int increaseInt = static_cast<int>(mIncreaseValue);
    minDmg += increaseInt;
    maxDmg += increaseInt;
}

SpellUpgradeInfo ProjectileSpellUpgradeStrategy::getSpellUpgradeInfo() const
{
    SpellUpgradeInfo upgradeInfo;
    auto& projectile = SpellHolder::getInstance().get(mSpellId).projectile.value();
    int increaseInt = static_cast<int>(mIncreaseValue);
    upgradeInfo.currValue = "[" + std::to_string(projectile.minDmg) + "," + std::to_string(projectile.maxDmg) + "]";
    upgradeInfo.nextValue = "[" + std::to_string(projectile.minDmg + increaseInt) + "," +
        std::to_string(projectile.maxDmg + increaseInt) + "]";
    upgradeInfo.name = SpellHolder::getInstance().getDefinition(mSpellId).spellInfo.name;
    upgradeInfo.unit = "dmg";

    return upgradeInfo;
}
