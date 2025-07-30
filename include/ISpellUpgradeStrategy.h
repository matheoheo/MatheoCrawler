#pragma once
#include "SpellData.h"

struct SpellUpgradeInfo
{
	std::string currValue;
	std::string nextValue;
	std::string name;
	std::string unit;
};

class ISpellUpgradeStrategy
{
public:
	ISpellUpgradeStrategy(SpellIdentifier spellId, float increaseValue);
	virtual ~ISpellUpgradeStrategy() = default;

	virtual void upgrade() const = 0;
	virtual SpellUpgradeInfo getSpellUpgradeInfo() const = 0;
protected:
	float getClamppedValue(float curr, float max) const;
protected:
	SpellIdentifier mSpellId;
	float mIncreaseValue;
};

