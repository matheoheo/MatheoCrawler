#include "pch.h"
#include "HealingSpellUpgradeStrategy.h"
#include "SpellHolder.h"

HealingSpellUpgradeStrategy::HealingSpellUpgradeStrategy(SpellIdentifier id, float increaseValue)
    :ISpellUpgradeStrategy(id, increaseValue)
{
}

void HealingSpellUpgradeStrategy::upgrade() const
{
    auto& healValue = SpellHolder::getInstance().get(mSpellId).healing->healValue;
    auto increase = getClamppedValue(healValue, 1.f);
    healValue += increase;
}

SpellUpgradeInfo HealingSpellUpgradeStrategy::getSpellUpgradeInfo() const
{
    SpellUpgradeInfo spellInfo;
    auto value = SpellHolder::getInstance().get(mSpellId).healing->healValue;
    spellInfo.currValue = std::format("{:.0f}%", value * 100.f);
    spellInfo.nextValue = std::format("{:.0f}%", (value + getClamppedValue(value, 1.f)) * 100.f);
    spellInfo.name = (mSpellId == SpellIdentifier::QuickHeal) ? "Quick Heal" : "Major Heal";

    return spellInfo;
}
