#include "pch.h"
#include "ISpellUpgradeStrategy.h"

ISpellUpgradeStrategy::ISpellUpgradeStrategy(SpellIdentifier spellId, float increaseValue)
	:mSpellId(spellId),
	mIncreaseValue(increaseValue)
{
}

float ISpellUpgradeStrategy::getClamppedValue(float curr, float max) const
{
	if (curr + mIncreaseValue > max)
		return max - curr;
	return mIncreaseValue;
}
