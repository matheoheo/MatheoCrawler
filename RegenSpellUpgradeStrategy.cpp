#include "pch.h"
#include "RegenSpellUpgradeStrategy.h"
#include "SpellHolder.h"

RegenSpellUpgradeStrategy::RegenSpellUpgradeStrategy(SpellIdentifier spellId, float increaseValue)
    :ISpellUpgradeStrategy(spellId, increaseValue)
{
}

void RegenSpellUpgradeStrategy::upgrade() const
{
    bool isHpRegen = mSpellId == SpellIdentifier::HealthRegen;
    auto& value = (isHpRegen) ?
        SpellHolder::getInstance().get(mSpellId).healing->bonusHpRegen :
        SpellHolder::getInstance().get(mSpellId).healing->bonusManaRegen;

    value += static_cast<int>(mIncreaseValue);
}

SpellUpgradeInfo RegenSpellUpgradeStrategy::getSpellUpgradeInfo() const
{
    SpellUpgradeInfo upgradeInfo;
    bool isHpRegen = mSpellId == SpellIdentifier::HealthRegen;
    auto value = (isHpRegen) ?
        SpellHolder::getInstance().get(mSpellId).healing->bonusHpRegen :
        SpellHolder::getInstance().get(mSpellId).healing->bonusManaRegen;
    upgradeInfo.currValue = std::to_string(value);
    upgradeInfo.nextValue = std::to_string(value + static_cast<int>(mIncreaseValue));
    upgradeInfo.name = isHpRegen ? "Health Regeneration" : "Mana Regeneration";
    upgradeInfo.unit = "/2s";
    return upgradeInfo;
}
